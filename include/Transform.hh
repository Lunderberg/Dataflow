#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <functional>
#include <memory>

#include "StoppableThread.hh"
#include "Producer.hh"
#include "Receiver.hh"

template<typename Input, typename Output>
class Transform : public Receiver<Input>, public Producer<Output>, public StoppableThread {
public:
  Transform(std::function<std::unique_ptr<Output>(const std::shared_ptr<Input>) > func,
            std::shared_ptr<Producer<Input> > source)
    : Receiver<Input>(source), func(func) {
    Start();
  }

  virtual ~Transform(){
    Producer<Output>::ForceStop();
  }

private:
  void Iteration(){
    auto obj = Receiver<Input>::Read();
    Producer<Output>::Push(func(obj));
  }

  std::function<std::unique_ptr<Output>(const std::shared_ptr<Input>) > func;
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
