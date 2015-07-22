#ifndef _SINK_H_
#define _SINK_H_

#include <memory>

#include "StoppableThread.hh"
#include "Producer.hh"
#include "Receiver.hh"

template<typename T>
class Sink : public StoppableThread  {
public:
  Sink(std::function<void(const std::shared_ptr<T>)> func, std::shared_ptr<Producer<T> > source)
    : func(func), receiver(std::make_shared<Receiver<T> >(source)) {
    Start();
  }

  std::shared_ptr<Receiver<T> > GetReceiver(){
    return receiver;
  }

private:
  void Iteration(){
    func(receiver->Read());
  }

  std::function<void(const std::shared_ptr<T>)> func;
  std::shared_ptr<Receiver<T> > receiver;
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
