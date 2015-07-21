#ifndef _PRODUCER_H_
#define _PRODUCER_H_

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

template<typename T>
class Receiver;

template<typename T>
class Producer {
public:
  Producer() : max_queue_size(10), force_stop(false) { }

  virtual ~Producer() { }

protected:
  void ForceStop(){
    force_stop = true;
  }

  void Push(std::unique_ptr<T> item){
    std::unique_lock<std::mutex> lock(queue_mutex);
    while(queue.size() >= max_queue_size && !force_stop){
      can_write_data.wait_for(lock, std::chrono::seconds(1));
    }

    if(force_stop){
      return;
    }

    queue.push(std::move(item));
    can_read_data.notify_one();
  }

private:
  friend class Receiver<T>;
  std::unique_ptr<T> Pop(){
    std::unique_lock<std::mutex> lock(queue_mutex);
    while(queue.empty() && !force_stop){
      can_read_data.wait_for(lock, std::chrono::seconds(1));
    }

    if(force_stop){
      return NULL;
    }

    auto output = std::move(queue.front());
    queue.pop();
    can_write_data.notify_one();
    return output;
  }

  std::queue<std::unique_ptr<T> > queue;
  size_t max_queue_size;

  std::condition_variable can_read_data;
  std::condition_variable can_write_data;
  std::mutex queue_mutex;

  std::atomic_bool force_stop;
};

#endif /* _PRODUCER_H_ */
