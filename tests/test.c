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

int main()
{
  int primes[NUM_PRIMES] = { 0 };

  rmk_thread_t worker;
  rmk_thread_create(&worker, &calc_primes, primes);

  rmk_thread_join(&worker);

  printf("Got primes, the %dth one is: %d\n", NUM_PRIMES, primes[NUM_PRIMES-1]);

  return 0;
}
