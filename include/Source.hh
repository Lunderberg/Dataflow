#ifndef _SOURCE_H_
#define _SOURCE_H_

#include <memory>

#include "StoppableThread.hh"
#include "Producer.hh"

template<typename T>
class Source : public StoppableThread {
public:
  Source(std::function<std::unique_ptr<T>() > func)
    : func(func), producer(std::make_shared<Producer<T> >()) {
    Start();
  }

  std::shared_ptr<Producer<T> > GetProducer(){
    return producer;
  }

private:
  void Iteration(){
    producer->Push(func());
  }

  std::function<std::unique_ptr<T>() > func;
  std::shared_ptr<Producer<T> > producer;
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
