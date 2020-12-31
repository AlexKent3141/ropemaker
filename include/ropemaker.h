#ifndef __ROPEMAKER_H_INCLUDED__
#define __ROPEMAKER_H_INCLUDED__

#include "stdbool.h"

#ifdef _MSC_VER
#define EXPORT __declspec(dllexport)
/* TODO */
#elif __GNUC__
#define EXPORT __attribute__ ((visibility("default")))
#include "pthread.h"
typedef pthread_mutex_t rmk_mutex_t;
typedef pthread_t rmk_thread_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void*(*rmk_thread_func)(void*);

EXPORT bool rmk_mutex_create(rmk_mutex_t*);
EXPORT void rmk_mutex_lock(rmk_mutex_t*);
EXPORT void rmk_mutex_unlock(rmk_mutex_t*);
EXPORT void rmk_mutex_destroy(rmk_mutex_t*);

EXPORT bool rmk_thread_create(rmk_thread_t*, rmk_thread_func, void*);

/* Signal that the specified thread should stop when it's convenient. */
EXPORT void rmk_thread_request_stop(rmk_thread_t*);

/* Check whether the current thread has been requested to stop. */
EXPORT bool rmk_thread_stop_requested();

/* Wait for the specified thread to stop. */
EXPORT void rmk_thread_join(rmk_thread_t*);

#ifdef __cplusplus
}
#endif

#endif /* __ROPEMAKER_H_INCLUDED__ */
