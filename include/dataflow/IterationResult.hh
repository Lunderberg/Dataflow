#ifndef ITERATIONRESULT_H
#define ITERATIONRESULT_H

namespace dataflow {
  enum class IterationResult {
    CONTINUE,
      STOP,
      PAUSE,
      DELAY
      };
}

#endif /* ITERATIONRESULT_H */
