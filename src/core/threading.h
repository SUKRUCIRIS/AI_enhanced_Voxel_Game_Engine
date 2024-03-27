#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct Thread Thread;

  typedef struct Mutex Mutex;

  typedef void (*ThreadFunction)(void *);

  Thread *create_thread(ThreadFunction func, void *arg);

  void join_thread(Thread *thread);

  Mutex *create_mutex(void);

  void destroy_mutex(Mutex *m);

  void lock_mutex(Mutex *m);

  void unlock_mutex(Mutex *m);

#ifdef __cplusplus
}
#endif