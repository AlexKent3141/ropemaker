#include "ropemaker.h"
#include "assert.h"
#include "stdio.h"

int alice_total_calls = 0;
rmk_mutex_t mutex;

struct caller_data
{
  char* name;
  int num_calls_made;
};

void call_alice(struct caller_data* caller)
{
  rmk_mutex_lock(&mutex);

  printf("Received call from %s\n", caller->name);
  fflush(stdout);

  ++alice_total_calls;
  ++caller->num_calls_made;

  rmk_mutex_unlock(&mutex);
}

void* bob_worker(void* data)
{
  struct caller_data* bob_data = (struct caller_data*)data;
  while (!rmk_thread_stop_requested())
  {
    call_alice(bob_data);
  }

  return NULL;
}

void* claire_worker(void* data)
{
  struct caller_data* claire_data = (struct caller_data*)data;
  while (!rmk_thread_stop_requested())
  {
    call_alice(claire_data);
  }

  return NULL;
}

int main()
{
  if (!rmk_mutex_create(&mutex))
  {
    printf("Failed to create mutex\n");
    return -1;
  }

  struct caller_data bob = { "Bob", 0 };
  struct caller_data claire = { "Claire", 0 };

  rmk_thread_t bth, cth;

  if (!rmk_thread_create(&bth, RMK_JOINABLE, &bob_worker, &bob))
  {
    printf("Failed to create Bob's thread\n");
    return -2;
  }

  if (!rmk_thread_create(&cth, RMK_JOINABLE, &claire_worker, &claire))
  {
    printf("Failed to create Claire's thread\n");
    return -3;
  }

  rmk_sleep_ms(100);

  rmk_thread_request_stop(bth);
  rmk_thread_request_stop(cth);

  rmk_thread_join(bth);
  rmk_thread_join(cth);

  printf("Alice received %d calls\n", alice_total_calls);
  printf("%d of them were from Bob\n", bob.num_calls_made);
  printf("%d of them were from Claire\n", claire.num_calls_made);

  assert(alice_total_calls == bob.num_calls_made + claire.num_calls_made);

  return 0;
}
