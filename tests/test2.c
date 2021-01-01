#include "ropemaker.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"

/* Calculate pi using the Gregory-Leibnitz series. */
void* calc_pi(void* data)
{
  double total = 0;
  int deno = 1;
  int sign = 1;
  const double num = 4;
  while (!rmk_thread_stop_requested())
  {
    total += sign * num / deno;
    sign *= -1;
    deno += 2;
  }

  double* res = (double*)data;
  *res = total;
}

double error(double estimate)
{
  return fabs(estimate - M_PI);
}

/*
This program will kick off several workers calculating pi and then stop each
on in turn after a short delay.
If everything works as expected each successive worker should give a better
estimate for pi.
*/
int main()
{
  rmk_thread_init();

  rmk_thread_t worker1, worker2, worker3;
  double result1, result2, result3;

  if (!rmk_thread_create(&worker1, RMK_JOINABLE, &calc_pi, &result1))
  {
    printf("Failed to create worker1\n");
    return -1;
  }

  if (!rmk_thread_create(&worker2, RMK_JOINABLE, &calc_pi, &result2))
  {
    printf("Failed to create worker2\n");
    return -1;
  }

  if (!rmk_thread_create(&worker3, RMK_JOINABLE, &calc_pi, &result3))
  {
    printf("Failed to create worker3\n");
    return -1;
  }

  rmk_sleep_ms(10);

  rmk_thread_request_stop(worker1);
  rmk_thread_join(worker1);

  printf("Worker1 estimates: %.10lf, error: %.10lf\n", result1, error(result1));

  rmk_sleep_ms(100);

  rmk_thread_request_stop(worker2);
  rmk_thread_join(worker2);

  printf("Worker2 estimates: %.10lf, error: %.10lf\n", result2, error(result2));

  rmk_sleep_ms(1000);

  rmk_thread_request_stop(worker3);
  rmk_thread_join(worker3);

  printf("Worker3 estimates: %.10lf, error: %.10lf\n", result3, error(result3));

  rmk_thread_shutdown();

  return 0;
}
