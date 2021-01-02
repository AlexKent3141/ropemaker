#include "windows.h"

void rmk_sleep_ms(int ms)
{
  Sleep(ms);
}

bool rmk_mutex_create(rmk_mutex_t* mutex)
{
  *mutex = CreateMutexA(NULL, FALSE, NULL);
  return *mutex != NULL;
}

void rmk_mutex_lock(rmk_mutex_t* mutex)
{
  WaitForSingleObject(*mutex, INFINITE);
}

void rmk_mutex_unlock(rmk_mutex_t* mutex)
{
  ReleaseMutex(*mutex);
}

void rmk_mutex_destroy(rmk_mutex_t* mutex)
{
  CloseHandle(*mutex);
}

bool thread_equal(rmk_thread_t t1, rmk_thread_t t2)
{
  return t1.id == t2.id;
}

bool rmk_thread_create(
  rmk_thread_t* thread,
  enum rmk_thread_creation_flags flags,
  rmk_thread_func func,
  void* data)
{
  thread->handle = CreateThread(
    NULL,         /* default security attributes */
    0,            /* default stack size */
    (LPTHREAD_START_ROUTINE)func,
    data,
    0,            /* default creation flags */
    &thread->id); /* we use the thread id to find this thread again later */

  if (thread->handle != NULL)
  {
    if (flags & RMK_DETACHED)
    {
      CloseHandle(thread->handle);
    }

    thread_map_add(&map, *thread, &thread_equal);
  }

  return thread->handle != NULL;
}

void rmk_thread_request_stop(rmk_thread_t thread)
{
  struct thread_map_node* node = thread_map_find(&map, thread, &thread_equal);
  if (node != NULL)
  {
    node->stop = true;
  }
}

bool rmk_thread_stop_requested()
{
  DWORD id = GetCurrentThreadId();
  rmk_thread_t thread = { NULL, id };
  struct thread_map_node* node = thread_map_find(&map, thread, &thread_equal);
  return node != NULL
    ? node->stop
    : false;
}

void rmk_thread_join(rmk_thread_t thread)
{
  if (thread.handle == NULL) return;

  WaitForSingleObject(thread.handle, INFINITE);
  thread_map_remove(&map, thread, &thread_equal);
}
