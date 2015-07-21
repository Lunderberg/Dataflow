#ifndef _SINK_H_
#define _SINK_H_

#include <atomic>
#include <thread>

#include "Producer.hh"
#include "Receiver.hh"

template<typename T>
class Sink : public Receiver<T> {
public:
  Sink(std::function<void(const std::shared_ptr<T>)> func, std::shared_ptr<Producer<T> > source)
    : Receiver<T>(source), func(func), running(false) {
    Start();
  }

  virtual ~Sink(){
    Stop();
  }

  void Start(){
    if(!running){
      running = true;
      thread = std::thread(&Sink<T>::Run, this);
    }
  }

  void Stop(){
    if(running){
      running = false;
      thread.join();
    }
  }

private:
  void Run(){
    while(running){
      func(Receiver<T>::Read());
    }
  }

  std::function<void(const std::shared_ptr<T>)> func;
  std::thread thread;
  std::atomic_bool running;
};

template<typename T, typename U>
std::shared_ptr<Sink<T> > make_sink(std::function<void(const std::shared_ptr<T>)> func,
                                    std::shared_ptr<U> source){
  return std::make_shared<Sink<T> >(func, source);
}

template<typename T, typename U>
std::shared_ptr<Sink<T> > make_sink(void (*func)(const std::shared_ptr<T>),
                                    std::shared_ptr<U> source){
  return std::make_shared<Sink<T> >(func, source);
}

#endif /* _SINK_H_ */
