Goals
=====

- Threadpool, dividing up tasks to different thread.
- Each task reads zero or more items from zero or more queues.
- Each task outputs zero or more items to zero or more queues.
- When an item is added to a queue, it generates a task that will remove it from the queue.
- Tasks that do not read from a queue are repeated indefinitely.
- When defined, a task may specify that it is to be run on a single instance
   or on multiple cores simultaneously.

- Items on queue are user-defined types.
- These items must be copyable.
- All reads/writes on queues must be checked at compile-time.




Desired Interaction
===================
```
dataflow::ThreadPool pool(5);
auto& raw_data = pool.make_queue<int>();
auto& processed_data = pool.make_queue<int>();

pool.make_task(read_data, raw_data);
pool.make_task(raw_data, process, processed_data);
auto& write_task = pool.make_task(processed_data, write);
write_task.set_mode(dataflow::TaskMode::SINGLE);

pool.start();
```

Notes:
  - All queues are owned by the pool, and may be viewed by reference.
  - make_queue is templated on the type that it holds.
  - make_tasks has three sections of arguments.
    First, a list of all queues that serve as input.
    Second, the function to be run.
    Third, a list of all queues that serve as output.
  - make_tasks returns a reference to the task, for use in configuration.
  - Each task returns a boolean, indicating whether it should continue to be run.



Possible Implementation
=======================

- ThreadPool holds a list of std::unique_ptr<ThreadsafeQueueBase>.
  ThreadsafeQueue<T> inherits from ThreadsafeQueueBase.
  ThreadsafeQueue<T> implements the abstract classes InputQueue<T> and OutputQueue<T>
- make_task is defined as follows, in its most general form.
  Additional overloads will be added to simplify the construction of the std::function.
// Overload for a transformative task.
// This task reads from a queue, then optionally outputs onto a queue.
// It continues so long as there is input.
template<typename... InTypes, typename... OutTypes>
dataflow::Task& dataflow::ThreadPool::make_task(
  InputQueue<InTypes>&... in_types,
  std::function<dataflow::IterationResult(InputQueue<InTypes>&..., OutputQueue<OutTypes>&...)> func,
  OutputQueue<OutTypes>&... out_types);

- The iteration result indicates how the next iteration of the task should be handled.
  It will be an enum with values of CONTINUE, DELAY, and STOP.

- Tasks hold references to their input/output queues, and their function.
  A task implements operator(), which passed the queues into the function.
