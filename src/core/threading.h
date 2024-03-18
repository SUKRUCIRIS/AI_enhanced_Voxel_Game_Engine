#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

  // Forward declaration of the thread handle
  typedef struct Thread Thread;

  // Function pointer type for thread functions
  typedef void (*ThreadFunction)(void *);

  // Function to create a new thread
  Thread *create_thread(ThreadFunction func, void *arg);

  // Function to join a thread
  void join_thread(Thread *thread);

#ifdef __cplusplus
}
#endif