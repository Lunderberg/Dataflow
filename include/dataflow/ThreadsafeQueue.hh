#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>

class ThreadsafeQueueBase {
public:
  virtual ~ThreadsafeQueueBase() { }
  virtual size_t size() const = 0;
  virtual size_t max_size() const = 0;
};

template<typename T>
class InputQueue {
public:
  virtual ~InputQueue() { }
  virtual bool try_pop(T& output) = 0;
  virtual size_t size() const = 0;
  virtual size_t max_size() const = 0;
};

template<typename T>
class OutputQueue {
public:
  virtual ~OutputQueue() { }
  virtual void push(T obj) = 0;
  virtual size_t size() const = 0;
  virtual size_t max_size() const = 0;
};

template<typename T>
class ThreadsafeQueue : public ThreadsafeQueueBase, public InputQueue<T>, public OutputQueue<T> {
public:
  ThreadsafeQueue()
    : max_queue_size(1000) { }

  virtual ~ThreadsafeQueue() { }

  void push(T obj) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(obj);
  }

  bool try_pop(T& output) {
    std::lock_guard<std::mutex> lock(mutex);
    if(queue.size()){
      output = queue.front();
      queue.pop();
      return true;
    } else {
      return false;
    }
  }

  size_t size() const {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.size();
  }

  size_t max_size() const {
    return max_queue_size;
  }

private:
  mutable std::mutex mutex;
  std::queue<T> queue;

  size_t max_queue_size;
};

#endif /* THREADSAFEQUEUE_H */
