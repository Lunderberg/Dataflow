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
  std::shared_ptr<Producer<T> > source;
  std::atomic_bool running;
};

#endif /* _RECEIVER_H_ */
