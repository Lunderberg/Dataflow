#ifndef _PRODUCER_H_
#define _PRODUCER_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>

template<typename Output>
class Producer {
public:
  Producer(std::function<std::unique_ptr<Output>() > func)
    : func(func), running(false)  { }

  virtual ~Producer(){
    if(running){
      running = false;
      thread.join();
    }
  }

  std::unique_ptr<Output> Pop(){
    std::unique_lock<std::mutex> lock(queue_mutex);
    has_data.wait(lock, [this](){return !queue.empty();} );

    auto output = std::move(queue.front());
    queue.pop();
    return output;
  }

protected:
  void Start() {
    if(!running){
      thread = std::thread(&Producer::Read, this);
      running = true;
    }
  }

private:
  void Read(){
    while(running){
      PushToQueue(func());
    }
  }

  void PushToQueue(std::unique_ptr<Output> item){
    std::lock_guard<std::mutex> lock(queue_mutex);
    queue.push(std::move(item));
    has_data.notify_one();
  }

  std::function<std::unique_ptr<Output>() > func;
  std::queue<std::unique_ptr<Output> > queue;
  std::condition_variable has_data;
  std::mutex queue_mutex;
  std::atomic_bool running;
  std::thread thread;
};

#endif /* _PRODUCER_H_ */
