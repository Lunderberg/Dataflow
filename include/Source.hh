#ifndef _SOURCE_H_
#define _SOURCE_H_

#include <atomic>
#include <thread>

#include "Producer.hh"

template<typename T>
class Source : public Producer<T> {
public:
  Source(std::function<std::unique_ptr<T>() > func)
    : func(func), running(false) {
    Start();
  }

  virtual ~Source(){
    Producer<T>::ForceStop();
    Stop();
  }

  void Start(){
    if(!running){
      running = true;
      thread = std::thread(&Source<T>::Run, this);
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
      Producer<T>::Push(func());
    }
  }

  std::function<std::unique_ptr<T>() > func;
  std::thread thread;
  std::atomic_bool running;
};

template<typename T>
std::shared_ptr<Source<T> > make_source(std::function<std::unique_ptr<T>()> func){
  return std::make_shared<Source<T> >(func);
}

template<typename T>
std::shared_ptr<Source<T> > make_source(std::unique_ptr<T> (*func)() ){
  return std::make_shared<Source<T> >(func);
}

#endif /* _SOURCE_H_ */
