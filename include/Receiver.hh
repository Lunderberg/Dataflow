#ifndef _RECEIVER_H_
#define _RECEIVER_H_

#include <atomic>
#include <memory>

#include "Source.hh"

template<typename Input>
class Receiver {
public:
  Receiver(std::function<void(const std::shared_ptr<Input>)> func, std::shared_ptr<Source<Input> > source)
    : source(source), func(func), running(true), thread(&Receiver::Write, this) { }

  ~Receiver(){
    if(running){
      running = false;
      thread.join();
    }
  }

protected:
  void Start(){
    if(!running){
      thread = std::thread(&Receiver::Write, this);
    }
  }

private:
  void Write(){
    while(running){
      auto obj = source->Pop();
      std::shared_ptr<Input> shared_obj(std::move(obj));
      func(shared_obj);
    }
  }

  std::shared_ptr<Source<Input> > source;
  std::function<void(const std::shared_ptr<Input>)> func;
  std::atomic_bool running;
  std::thread thread;
};

#endif /* _RECEIVER_H_ */
