#ifndef _SOURCE_H_
#define _SOURCE_H_

#include "Producer.hh"

template<typename Output>
class Source : public Producer<Output> {
public:
  Source(std::function<std::unique_ptr<Output>() > func)
    : Producer<Output>(func) {
    Producer<Output>::Start();
  }
};



template<typename Output>
std::shared_ptr<Source<Output> > make_source(std::function<std::unique_ptr<Output>()> func){
  return std::make_shared<Source<Output> >(func);
}

template<typename Output>
std::shared_ptr<Source<Output> > make_source(std::unique_ptr<Output> (*func)() ){
  return std::make_shared<Source<Output> >(func);
}

#endif /* _SOURCE_H_ */
