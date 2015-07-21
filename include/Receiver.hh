#ifndef _RECEIVER_H_
#define _RECEIVER_H_

#include <atomic>
#include <memory>

#include "Producer.hh"

template<typename T>
class Receiver {
public:
  Receiver(std::shared_ptr<Producer<T> > source)
    : source(source) { }

  virtual ~Receiver() { }

protected:
  const std::shared_ptr<T> Read(){
    return source->Pop();
  }

private:
  void Write(){
    while(running){
      auto obj = source->Pop();
      std::shared_ptr<T> shared_obj(std::move(obj));
      func(shared_obj);
    }
  }

  std::shared_ptr<Producer<T> > source;
  std::function<void(const std::shared_ptr<T>)> func;
  std::atomic_bool running;
  std::thread thread;
};

#endif /* _RECEIVER_H_ */
