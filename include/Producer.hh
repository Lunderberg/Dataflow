#ifndef _PRODUCER_H_
#define _PRODUCER_H_

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <map>
#include <memory>
#include <mutex>

template<typename T>
class Receiver;

template<typename T>
class Producer {
public:
  Producer() : max_queue_size(10), force_stop(false) { }

  virtual ~Producer() {
    ForceStop();
  }

  void Push(std::unique_ptr<T> item){
    std::unique_lock<std::mutex> lock(queue_mutex);
    while(queue.size() >= max_queue_size && !force_stop){
      can_write_data.wait_for(lock, std::chrono::seconds(1));
    }

    if(force_stop){
      return;
    }

    queue.push_back(std::move(item));
    can_read_data.notify_one();
  }

protected:
  void ForceStop(){
    force_stop = true;
  }

private:
  friend class Receiver<T>;
  const std::shared_ptr<T> Pop(Receiver<T>* requester){
    std::unique_lock<std::mutex> lock(queue_mutex);

    // New clients start at the beginning of the queue.
    if(!queue_locations.count(requester)){
      queue_locations[requester] = queue.begin();
    }

    auto& location = queue_locations[requester];

    while(location==queue.end() && !force_stop){
      can_read_data.wait_for(lock, std::chrono::seconds(1));
    }

    if(force_stop){
      return NULL;
    }

    auto output = *location;
    location++;
    UpdateQueue();
    return output;
  }

  void UpdateQueue(){
    bool can_pop = true;
    for(auto& val : queue_locations){
      if(val.second == queue.begin()){
        can_pop = false;
        break;
      }
    }

    if(can_pop){
      queue.pop_front();
      can_write_data.notify_one();
    }
  }

  void Deregister(Receiver<T>* requester){
    queue_locations.erase(requester);
  }

  std::deque<std::shared_ptr<T> > queue;
  std::map<Receiver<T>*, typename std::deque<std::shared_ptr<T> >::iterator> queue_locations;
  size_t max_queue_size;

  std::condition_variable can_read_data;
  std::condition_variable can_write_data;
  std::mutex queue_mutex;

  std::atomic_bool force_stop;
};

#endif /* _PRODUCER_H_ */
