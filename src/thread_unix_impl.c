#include "pthread.h"
#include "time.h"
#include "errno.h"

void rmk_sleep_ms(int ms)
{
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000;
  while (nanosleep(&ts, &ts) == -1 && errno == EINTR);
}

bool rmk_mutex_create(rmk_mutex_t* mutex)
{
  return pthread_mutex_init(mutex, NULL) == 0;
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

bool thread_equal(pthread_t t1, pthread_t t2)
{
  return pthread_equal(t1, t2) != 0;
}

bool rmk_thread_create(
  rmk_thread_t* thread,
  enum rmk_thread_creation_flags flags,
  rmk_thread_func func,
  void* data)
{
  pthread_attr_t attr;
  pthread_attr_init(&attr);

  if (flags & RMK_JOINABLE)
  {
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  }
  else if (flags & RMK_DETACHED)
  {
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  }

  int ret = pthread_create(
    thread,
    &attr,
    func,
    data);

  pthread_attr_destroy(&attr);

  if (ret == 0 && (flags & RMK_JOINABLE))
  {
    thread_map_add(&map, *thread, &thread_equal);
  }

  return ret == 0;
}

void rmk_thread_request_stop(rmk_thread_t thread)
{
  struct thread_map_entry* node = thread_map_find(&map, thread, &thread_equal);
  if (node != NULL)
  {
    node->stop = true;
  }
}

bool rmk_thread_stop_requested()
{
  pthread_t thread = pthread_self();
  struct thread_map_entry* node = thread_map_find(&map, thread, &thread_equal);
  return node != NULL
    ? node->stop
    : false;
}

void rmk_thread_join(rmk_thread_t thread)
{
  pthread_join(thread, NULL);
  thread_map_remove(&map, thread, &thread_equal);
}
