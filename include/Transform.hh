#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <functional>
#include <memory>

#include "Producer.hh"
#include "Receiver.hh"

template<typename Input, typename Output>
class Transform : public Receiver<Input>, public Producer<Output> {
public:
  Transform(std::function<std::unique_ptr<Output>(const std::shared_ptr<Input>) > func)
    : func(func) {
    Receiver<Input>::Start();
    Producer<Output>::Start();
  }

private:
  std::function<std::unique_ptr<Output>(const std::shared_ptr<Input>) > func;
};

#endif /* _TRANSFORM_H_ */
