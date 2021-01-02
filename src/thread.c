#include "ropemaker.h"
#include "assert.h"
#include "stdbool.h"

#define MAX_THREADS 128

struct thread_map_entry
{
  rmk_thread_t thread;
  bool stop;
  bool used;
};

struct thread_map
{
  struct thread_map_entry entries[MAX_THREADS];
};

struct thread_map_entry* thread_map_find(
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

static struct thread_map map = { 0 };

#ifdef _MSC_VER
#include "thread_win_impl.c"
#elif __GNUC__
#include "thread_unix_impl.c"
#endif

/* Begin map implementation. */
struct thread_map_entry* thread_map_find(
  struct thread_map* map,
  rmk_thread_t thread,
  bool (*thread_equal)(rmk_thread_t, rmk_thread_t))
{
  struct thread_map_entry* entry;
  for (int i = 0; i < MAX_THREADS; i++)
  {
    entry = &map->entries[i];
    if (entry->used && thread_equal(thread, entry->thread))
    {
      return entry;
    }
  }

  return NULL;
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
  struct thread_map_entry* entry = thread_map_find(map, thread, thread_equal);
  if (entry != NULL)
  {
    entry->stop = false;
    return;
  }

  /* Find the first unused slot in the map. */
  for (int i = 0; i < MAX_THREADS; i++)
  {
    entry = &map->entries[i];
    if (!entry->used) break;
  }

  if (entry != NULL)
  {
    entry->thread = thread;
    entry->stop = false;
    entry->used = true;
  }

  /* TODO: need to report an error if we have run out of entries. */
}

void thread_map_remove(
  struct thread_map* map,
  rmk_thread_t thread,
  bool (*thread_equal)(rmk_thread_t, rmk_thread_t))
{
  struct thread_map_entry* entry = thread_map_find(map, thread, thread_equal);
  if (entry != NULL)
  {
    entry->used = false;
  }
}

/* End map implementation  */
