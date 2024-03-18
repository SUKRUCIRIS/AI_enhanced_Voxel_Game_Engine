// thread_wrapper.cpp
#include "threading.h"
#include <thread>

// Define the structure for the thread handle
struct Thread
{
  std::thread native_thread;
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