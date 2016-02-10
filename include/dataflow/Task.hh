#ifndef TASK_H
#define TASK_H

#include <mutex>

#include "dataflow/IterationResult.hh"
#include "dataflow/TaskMode.hh"

namespace dataflow {
  class Task {
  public:
    Task();
    virtual ~Task() { }

    double priority() const;
    void set_priority(double value);

    TaskMode mode() const;
    void set_mode(TaskMode new_mode);

    IterationResult attempt();

  private:
    virtual IterationResult apply() = 0;

    double priority_;
    TaskMode mode_;
    std::mutex mutex;
  };
}

#endif /* TASK_H */
