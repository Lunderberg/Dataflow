#ifndef _SINK_H_
#define _SINK_H_

#include "Receiver.hh"

template<typename Input>
class Sink : public Receiver<Input> {
public:
  Sink(std::function<void(const std::shared_ptr<Input>)> func, std::shared_ptr<Source<Input> > source)
    : Receiver<Input>(func, source) {
    Receiver<Input>::Start();
  }
};



template<typename Input>
std::shared_ptr<Sink<Input> > make_sink(std::function<void(const std::shared_ptr<Input>)> func,
                                        std::shared_ptr<Source<Input> > source){
  return std::make_shared<Sink<Input> >(func, source);
}

template<typename Input>
std::shared_ptr<Sink<Input> > make_sink(void (*func)(const std::shared_ptr<Input>),
                                        std::shared_ptr<Source<Input> > source){
  return std::make_shared<Sink<Input> >(func, source);
}

#endif /* _SINK_H_ */
