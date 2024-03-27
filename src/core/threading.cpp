// thread_wrapper.cpp
#include "threading.h"
#include <thread>
#include <mutex>

// Define the structure for the thread handle
struct Thread
{
  std::thread native_thread;
};

struct Mutex
{
  std::mutex m;
};

// Function to create a new thread
Thread *create_thread(ThreadFunction func, void *arg)
{
  Thread *thread = new Thread;
  thread->native_thread = std::thread(func, arg);
  return thread;
}

// Function to join a thread
void join_thread(Thread *thread)
{
  thread->native_thread.join();
  delete thread;
}

Mutex *create_mutex(void)
{
  Mutex *m = new Mutex;
  return m;
}

void destroy_mutex(Mutex *m)
{
  delete m;
}

void lock_mutex(Mutex *m)
{
  m->m.lock();
}

void unlock_mutex(Mutex *m)
{
  m->m.unlock();
}