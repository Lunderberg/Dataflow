#include "dataflow/ThreadPool.hh"

#include <chrono>

dataflow::ThreadPool::ThreadPool(int num_threads)
  : running(true) {
  for(int i=0; i<num_threads; i++){
    workers.emplace_back(&ThreadPool::worker_thread, this);
  }
}

dataflow::ThreadPool::~ThreadPool() {
  running = false;
  for(auto& thread : workers) {
    thread.join();
  }
}

void dataflow::ThreadPool::worker_thread() {
  while(running) {
    bool processed = false;
    for(auto& task : tasks){
      IterationResult res = task->apply();
      if(res == IterationResult::CONTINUE) {
        processed = true;
        break;
      }
    }

    if(!processed){
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }
}

void dataflow::ThreadPool::start() { }
