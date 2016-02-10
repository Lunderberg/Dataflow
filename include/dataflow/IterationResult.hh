#ifndef ITERATIONRESULT_H
#define ITERATIONRESULT_H

namespace dataflow {
  enum class IterationResult {
    // Work has been done.  The task is available to be performed again immediately
    SUCCESS,

    // This particular time, the task cannot be completed.
    // It should still be checked the next time.
    // e.g. The task may only be run once at a time, and a thread is already going.
      SKIP,

    // This particular time, the task cannot be completed.
    // Wait before trying this task again.
    // e.g. The input queue is empty.
      DELAY,

    // The task should never be attempted again.
    // e.g. The file has been entirely looped over.
      STOP
      };
}

#endif /* ITERATIONRESULT_H */
