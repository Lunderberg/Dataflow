#ifndef TASK_H
#define TASK_H

#include <mutex>

#include "dataflow/IterationResult.hh"
#include "dataflow/TaskMode.hh"
#include "dataflow/TaskState.hh"

namespace dataflow {
  class Task {
  public:
    Task();
    virtual ~Task() { }

    double priority() const;
    void set_priority(double value);

    TaskMode mode() const;
    void set_mode(TaskMode new_mode);

    TaskState state() const;

    IterationResult attempt();

  private:
    virtual IterationResult apply() = 0;

    double priority_;
    TaskMode mode_;
    TaskState state_;
    std::mutex mutex;
  };
}

#endif /* TASK_H */
