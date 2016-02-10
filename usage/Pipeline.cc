#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

#include "dataflow/ThreadPool.hh"

dataflow::IterationResult read_data(OutputQueue<int>& out) {
  if(out.size() >= out.max_size()){
    return dataflow::IterationResult::DELAY;
  }

  int value;
  {
    static std::mutex mutex;
    static int i = 0;
    std::lock_guard<std::mutex> lock(mutex);
    i++;
    value = i;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  if(value < 10){
    out.push(value);
    return dataflow::IterationResult::CONTINUE;
  } else {
    return dataflow::IterationResult::STOP;
  }
}

dataflow::IterationResult process_data(InputQueue<int>& in, OutputQueue<int>& out) {
  if(out.size() >= out.max_size()){
    return dataflow::IterationResult::DELAY;
  }

  int input;
  bool has_data = in.try_pop(input);

  if(has_data){
    out.push(input*input);
    return dataflow::IterationResult::CONTINUE;
  } else {
    return dataflow::IterationResult::DELAY;
  }
}

dataflow::IterationResult write_data(InputQueue<int>& in) {
  int input;
  bool has_data = in.try_pop(input);

  if(has_data){
    std::cout << input << std::endl;
    return dataflow::IterationResult::CONTINUE;
  } else {
    return dataflow::IterationResult::DELAY;
  }
}

int main(){
  dataflow::ThreadPool pool(5);
  auto& raw_data = pool.make_queue<int>();
  auto& processed_data = pool.make_queue<int>();

  auto& read_task = pool.make_task(read_data, raw_data);
  read_task.set_mode(dataflow::TaskMode::SINGLE);
  pool.make_task(raw_data, process_data, processed_data);

  auto& write_task = pool.make_task(processed_data, write_data);
  write_task.set_mode(dataflow::TaskMode::SINGLE);

  pool.start();
  std::this_thread::sleep_for(std::chrono::seconds(5));
}
