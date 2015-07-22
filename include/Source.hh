#ifndef _SOURCE_H_
#define _SOURCE_H_

#include "StoppableThread.hh"
#include "Producer.hh"

template<typename T>
class Source : public Producer<T>, public StoppableThread {
public:
  Source(std::function<std::unique_ptr<T>() > func)
    : func(func) {
    Start();
  }

  virtual ~Source(){
    Producer<T>::ForceStop();
  }

private:
  void Iteration(){
    Producer<T>::Push(func());
  }

  std::function<std::unique_ptr<T>() > func;
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
