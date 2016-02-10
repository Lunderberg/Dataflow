#include "dataflow/Task.hh"

dataflow::Task::Task()
  : priority_(0), mode_(TaskMode::MULTIPLE) { }

dataflow::IterationResult dataflow::Task::attempt() {
  if(mode_ == TaskMode::SINGLE) {
    // Grab the mutex, then run apply().
    // If a different thread has the mutex, bail out.
    std::unique_lock<std::mutex> lock(mutex, std::defer_lock);
    bool has_lock = lock.try_lock();
    if(has_lock){
      return apply();
    } else {
      return dataflow::IterationResult::SKIP;
    }
  } else {
    // Run apply() immediately, without grabbing the mutex.
    return apply();
  }
}

double dataflow::Task::priority() const {
  return priority_;
}

void dataflow::Task::set_priority(double value) {
  priority_ = value;
}

dataflow::TaskMode dataflow::Task::mode() const {
  return mode_;
}

void dataflow::Task::set_mode(dataflow::TaskMode value) {
  mode_ = value;
}
