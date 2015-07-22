#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <functional>
#include <memory>

#include "StoppableThread.hh"
#include "Producer.hh"
#include "Receiver.hh"

template<typename Input, typename Output>
class Transform : public StoppableThread {
public:
  Transform(std::function<std::unique_ptr<Output>(const std::shared_ptr<Input>) > func,
            std::shared_ptr<Producer<Input> > source)
    : func(func),
      receiver(std::make_shared<Receiver<Input> >(source)),
      producer(std::make_shared<Producer<Output> >()) {
    Start();
  }

  std::shared_ptr<Receiver<Input> > GetReceiver(){
    return receiver;
  }

  std::shared_ptr<Producer<Output> > GetProducer(){
    return producer;
  }

private:
  void Iteration(){
    auto obj = receiver->Read();
    producer->Push(func(obj));
  }

  std::function<std::unique_ptr<Output>(const std::shared_ptr<Input>) > func;
  std::shared_ptr<Receiver<Input> > receiver;
  std::shared_ptr<Producer<Output> > producer;
};

template<typename Input, typename Output, typename U>
std::shared_ptr<Transform<Input, Output> > make_transform(std::function<std::unique_ptr<Output> (const std::shared_ptr<Input>)> func,
                                                          std::shared_ptr<U> source){
  return std::make_shared<Transform<Input, Output> >(func, source);
}

template<typename Input, typename Output, typename U>
std::shared_ptr<Transform<Input, Output> > make_transform(std::unique_ptr<Output> (*func)(const std::shared_ptr<Input>),
                                                          std::shared_ptr<U> source){
  return std::make_shared<Transform<Input, Output> >(func, source);
}



#endif /* _TRANSFORM_H_ */
