#include "ropemaker.h"
#include "pthread.h"

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

bool rmk_thread_create(
  rmk_thread_t* thread,
  rmk_thread_func func,
  void* data)
{
  return pthread_create(
    thread,
    NULL, /* default attributes */
    func,
    data) == 0;
}

void rmk_thread_request_stop(rmk_thread_t* thread)
{
  /* TODO */
}

bool rmk_thread_stop_requested()
{
  /* TODO */
}

void rmk_thread_join(rmk_thread_t* thread)
{
  pthread_join(*thread, NULL);
}
