#include "ropemaker.h"
#include "assert.h"
#include "stdbool.h"
#include "stdlib.h"

struct thread_map_node
{
  rmk_thread_t thread;
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
  rmk_thread_t thread,
  bool (*thread_equal)(rmk_thread_t, rmk_thread_t));

void thread_map_add(
  struct thread_map* map,
  rmk_thread_t thread,
  bool (*thread_equal)(rmk_thread_t, rmk_thread_t));

void thread_map_remove(
  struct thread_map* map,
  rmk_thread_t thread,
  bool (*thread_equal)(rmk_thread_t, rmk_thread_t));

void thread_map_destroy(
  struct thread_map* map);

static struct thread_map map = { NULL, NULL };

#ifdef _MSC_VER
#include "thread_win_impl.c"
#elif __GNUC__
#include "thread_unix_impl.c"
#endif

/* Begin map implementation. */
struct thread_map_node* thread_map_find(
  struct thread_map* map,
  rmk_thread_t thread,
  bool (*thread_equal)(rmk_thread_t, rmk_thread_t))
{
  struct thread_map_node* node = map->head;
  while (node != NULL && !thread_equal(thread, node->thread))
  {
    node = node->next;
  }

  return node;
}

void thread_map_add(
  struct thread_map* map,
  rmk_thread_t thread,
  bool (*thread_equal)(rmk_thread_t, rmk_thread_t))
{
  /* First check whether there is already a thread stored in the map with this
     identifier.
     I'm concerned about the scenario where we created a thread previously but it
     didn't get removed from the map, in which case we could theoretically have
     another thread with the same id. */
  struct thread_map_node* node = thread_map_find(map, thread, thread_equal);
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
  rmk_thread_t thread,
  bool (*thread_equal)(rmk_thread_t, rmk_thread_t))
{
  struct thread_map_node* prev = NULL;
  struct thread_map_node* node = map->head;
  while (node != NULL && !thread_equal(thread, node->thread))
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
