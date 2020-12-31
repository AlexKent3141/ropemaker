#include "ropemaker.h"
#include "assert.h"
#include "pthread.h"
#include "stdlib.h"

/* Begin map implementation. */
struct thread_map_node
{
  pthread_t thread;
  bool stop;
  struct thread_map_node* next;
};

struct thread_map
{
  struct thread_map_node* head;
  struct thread_map_node* tail;
};

struct thread_map_node* thread_map_find(
  struct thread_map* map,
  pthread_t thread)
{
  struct thread_map_node* node = map->head;
  while (node != NULL && pthread_equal(thread, node->thread) == 0)
  {
    node = node->next;
  }

  return node;
}

void thread_map_add(
  struct thread_map* map,
  pthread_t thread)
{
  /* First check whether there is already a thread stored in the map with this
     identifier.
     I'm concerned about the scenario where we created a thread previously but it
     didn't get removed from the map, in which case we could theoretically have
     another thread with the same id. */
  struct thread_map_node* node = thread_map_find(map, thread);
  if (node != NULL)
  {
    node->stop = false;
    return;
  }

  node = (struct thread_map_node*)malloc(sizeof(struct thread_map_node));
  node->thread = thread;
  node->stop = false;
  node->next = NULL;

  if (map->head == NULL)
  {
    /* We are creating the first node. */
    map->head = node;
    map->tail = node;
  }
  else
  {
    map->tail->next = node;
    map->tail = node;
  }
}

void thread_map_remove(
  struct thread_map* map,
  pthread_t thread)
{
  struct thread_map_node* prev = NULL;
  struct thread_map_node* node = map->head;
  while (node != NULL && pthread_equal(thread, node->thread) == 0)
  {
    prev = node;
    node = node->next;
  }

  assert(node != NULL);

  if (prev != NULL)
  {
    prev->next = node->next;
  }

  if (map->head == node)
  {
    map->head = node->next;
  }

  if (map->tail == node)
  {
    map->tail = prev;
  }

  free(node);
}

void thread_map_destroy(
  struct thread_map* map)
{
  if (map == NULL) return;

  struct thread_map_node* node = map->head;
  struct thread_map_node* next;
  while (node != NULL)
  {
    next = node->next;
    free(node);
    node = next;
  }

  map = NULL;
}
/* End map implementation  */

static struct thread_map map = { NULL, NULL };

bool rmk_mutex_create(rmk_mutex_t* mutex)
{
  pthread_mutex_init(mutex, NULL);
}

void rmk_mutex_lock(rmk_mutex_t* mutex)
{
  pthread_mutex_lock(mutex);
}

void rmk_mutex_unlock(rmk_mutex_t* mutex)
{
  pthread_mutex_unlock(mutex);
}

void rmk_mutex_destroy(rmk_mutex_t* mutex)
{
  pthread_mutex_destroy(mutex);
}

void rmk_thread_init()
{
}

void rmk_thread_shutdown()
{
  thread_map_destroy(&map);
}

bool rmk_thread_create(
  rmk_thread_t* thread,
  enum rmk_thread_creation_flags flags,
  rmk_thread_func func,
  void* data)
{
  pthread_attr_t attr;
  pthread_attr_init(&attr);

  if (flags == RMK_JOINABLE)
  {
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  }
  else if (flags == RMK_DETACHED)
  {
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  }

  int success = pthread_create(
    thread,
    &attr,
    func,
    data) == 0;

  pthread_attr_destroy(&attr);

  if (success)
  {
    thread_map_add(&map, *thread);
  }

  return success;
}

void rmk_thread_request_stop(rmk_thread_t thread)
{
  struct thread_map_node* node = thread_map_find(&map, thread);
  if (node != NULL)
  {
    node->stop = true;
  }
}

bool rmk_thread_stop_requested()
{
  pthread_t thread = pthread_self();
  struct thread_map_node* node = thread_map_find(&map, thread);
  return node != NULL
    ? node->stop
    : false;
}

void rmk_thread_join(rmk_thread_t thread)
{
  pthread_join(thread, NULL);
  thread_map_remove(&map, thread);
}
