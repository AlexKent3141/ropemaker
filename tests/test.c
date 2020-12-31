#include "ropemaker.h"
#include "stdio.h"

#define NUM_PRIMES 10000

void* calc_primes(void* data)
{
  int* primes = (int*)data;
  primes[0] = 2;
  int num_primes = 1;

  int p = 3;
  while (num_primes < NUM_PRIMES)
  {
    bool is_prime = true;
    for (int i = 0; i < num_primes && is_prime; i++)
    {
      is_prime = p % primes[i] != 0;
    }

    if (is_prime)
    {
      primes[num_primes++] = p;
    }

    ++p;
  }
}

void* say_hello(void* data)
{
  printf("Hello world!\n");
}

int main()
{
  int primes[NUM_PRIMES] = { 0 };

  rmk_thread_init();

  rmk_thread_t worker1;
  rmk_thread_create(&worker1, RMK_JOINABLE, &calc_primes, primes);

  rmk_thread_t worker2;
  rmk_thread_create(&worker2, RMK_DETACHED, &say_hello, NULL);

  rmk_thread_join(worker1);

  printf("Got primes, the %dth one is: %d\n", NUM_PRIMES, primes[NUM_PRIMES-1]);

  rmk_thread_shutdown();

  return 0;
}
