
bool rmk_mutex_create(rmk_mutex_t* mutex)
{
}

void rmk_mutex_lock(rmk_mutex_t* mutex)
{
}

void rmk_mutex_unlock(rmk_mutex_t* mutex)
{
}

void rmk_mutex_destroy(rmk_mutex_t* mutex)
{
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
}

void rmk_thread_request_stop(rmk_thread_t thread)
{
}

bool rmk_thread_stop_requested()
{
}

void rmk_thread_join(rmk_thread_t thread)
{
}
