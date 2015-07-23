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

  virtual ~Receiver() {
    source->Deregister(this);
  }

  const std::shared_ptr<T> Read(){
    return source->Pop(this);
  }

private:
  std::shared_ptr<Producer<T> > source;
};

#endif /* _RECEIVER_H_ */
