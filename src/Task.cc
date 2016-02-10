#include "dataflow/Task.hh"

dataflow::Task::Task()
  : priority_(0), mode_(TaskMode::MULTIPLE) { }

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
