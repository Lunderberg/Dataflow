#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <memory>
#include <thread>
#include <tuple>
#include <type_traits>
#include <vector>

#include "dataflow/DirectTask.hh"
#include "dataflow/Indices.hh"
#include "dataflow/MakeUnique.hh"
#include "dataflow/IterationResult.hh"
#include "dataflow/ThreadsafeQueue.hh"
#include "dataflow/Task.hh"

namespace dataflow{
  /// The main class, in which one defines the jobs to be done.
  class ThreadPool {
  public:
    ThreadPool(int num_threads = 5);

    ~ThreadPool();

    /// Construct a queue.
    /**
       Not threadsafe to call `make_queue` on multiple threads.
       Should only be called from the setup thread.
     */
    template<typename T>
    ThreadsafeQueue<T>& make_queue() {
      auto new_queue = make_unique<ThreadsafeQueue<T> >();
      ThreadsafeQueue<T>& output = *new_queue;
      queues.push_back(std::move(new_queue));
      return output;
    }

    /// Constructs a task
    /**
       Not threadsafe to call `make_task` on multiple threads.
       Should only be called from the setup thread.
     */
    template<typename... Args>
    Task& make_task(Args&&... args) {
      return TaskParser<
        std::tuple<>,
        std::tuple<>,
        std::tuple<>,
        Args...>::construct(*this,
                            std::tuple<>(),
                            std::tuple<>(),
                            std::tuple<>(),
                            std::forward<Args>(args)...);
    }

    void start();


  private:

    template<typename... Args>
    struct TaskParser { };

    // If no function has been found, and the next argument is a queue,
    // add it to the list of input queues.
    template<typename... Inputs,
             typename Current, typename... Remaining>
    struct TaskParser<std::tuple<Inputs...>,
                      std::tuple<>,
                      std::tuple<>,
                      ThreadsafeQueue<Current>&, Remaining...> {

      static Task& construct(ThreadPool& pool,
                             std::tuple<InputQueue<Inputs>&...> inputs,
                             std::tuple<> functions,
                             std::tuple<> outputs,
                             ThreadsafeQueue<Current>& current,
                             Remaining&&... args) {
        return helper(pool,
                      build_indices<sizeof...(Inputs)>(),
                      inputs, functions, outputs,
                      current, std::forward<Remaining>(args)...);
      }

      template<int... Indices>
      static Task& helper(ThreadPool& pool,
                          indices<Indices...>,
                          std::tuple<InputQueue<Inputs>&...> inputs,
                          std::tuple<> /*functions*/,
                          std::tuple<> /*outputs*/,
                          ThreadsafeQueue<Current>& current,
                          Remaining&&... args) {
        // Stop gcc from complaining if inputs is empty.
        (void)inputs;

        typedef TaskParser<
          std::tuple<Inputs..., Current>,
          std::tuple<>,
          std::tuple<>,
          Remaining...> next_iter;
        typedef std::tuple<InputQueue<Inputs>&..., InputQueue<Current>&> input_tuple;
        return next_iter::construct(
          pool,
          input_tuple(std::get<Indices>(inputs)..., current),
          std::tuple<>(),
          std::tuple<>(),
          std::forward<Remaining>(args)...
        );
      }
    };

    // If no function has been found, and the next argument is not a queue.
    // Add it to the functions.
    template<typename... Inputs,
             typename Callable, typename... Remaining>
    struct TaskParser<std::tuple<Inputs...>,
                      std::tuple<>,
                      std::tuple<>,
                      Callable, Remaining...> {

      static Task& construct(ThreadPool& pool,
                             std::tuple<InputQueue<Inputs>&...> inputs,
                             std::tuple<> /*functions*/,
                             std::tuple<> /*outputs*/,
                             Callable&& callable,
                             Remaining&&... args) {
        typedef TaskParser<
          std::tuple<Inputs...>,
          std::tuple<Callable>,
          std::tuple<>,
          Remaining...> next_iter;
        return next_iter::construct(
          pool,
          inputs,
          std::tuple<Callable>(std::forward<Callable>(callable)),
          std::tuple<>(),
          std::forward<Remaining>(args)...
          );
      }
    };

    // If a function has been found, and the next argument is a queue.
    // Add it to the output queues.
    template<typename... Inputs,
             typename Function,
             typename... Outputs,
             typename Current, typename... Remaining>
    struct TaskParser<std::tuple<Inputs...>,
                      std::tuple<Function>,
                      std::tuple<Outputs...>,
                      ThreadsafeQueue<Current>&, Remaining...> {

      static Task& construct(ThreadPool& pool,
                             std::tuple<InputQueue<Inputs>&...> inputs,
                             std::tuple<Function> functions,
                             std::tuple<OutputQueue<Outputs>&...> outputs,
                             ThreadsafeQueue<Current>& current,
                             Remaining&&... args) {
        return helper(pool,
                      build_indices<sizeof...(Outputs)>(),
                      inputs, functions, outputs,
                      current,
                      std::forward<Remaining>(args)...);
      }

      template<int... Indices>
      static Task& helper(ThreadPool& pool,
                          indices<Indices...>,
                          std::tuple<InputQueue<Inputs>&...> inputs,
                          std::tuple<Function&&> functions,
                          std::tuple<OutputQueue<Outputs>&...> outputs,
                          ThreadsafeQueue<Current>& current,
                          Remaining&&... args) {
        // Prevent gcc from complaining when outputs is an empty tuple
        (void)outputs;

        typedef TaskParser<
          std::tuple<Inputs...>,
          std::tuple<Function>,
          std::tuple<Outputs..., Current>,
          Remaining...> next_iter;
        typedef std::tuple<OutputQueue<Outputs>&..., OutputQueue<Current>&> output_tuple;
        return next_iter::construct(
          pool,
          inputs,
          functions,
          output_tuple(std::get<Indices>(outputs)..., current),
          std::forward<Remaining>(args)...
        );
      }
    };

    // All arguments have been unpacked, we are set.
    template<typename... Inputs,
             typename Function,
             typename... Outputs>
    struct TaskParser<std::tuple<Inputs...>,
                      std::tuple<Function>,
                      std::tuple<Outputs...> > {

      static Task& construct(ThreadPool& pool,
                             std::tuple<InputQueue<Inputs>&...> inputs,
                             std::tuple<Function> functions,
                             std::tuple<OutputQueue<Outputs>&...> outputs) {
        typedef DirectTask<std::tuple<Inputs...>,
                           std::tuple<Outputs...> > task_type;
        std::unique_ptr<Task> new_task = make_unique<task_type>(inputs, std::get<0>(functions), outputs);
        return pool.add_task(std::move(new_task));
      }
    };

    Task& add_task(std::unique_ptr<Task> new_task){
      Task& output = *new_task;
      tasks.push_back(std::move(new_task));
      return output;
    }

    void worker_thread();

    std::vector<std::thread> workers;
    std::vector<std::unique_ptr<ThreadsafeQueueBase> > queues;
    std::vector<std::unique_ptr<Task> > tasks;
    std::atomic_bool running;
  };
}

#endif /* THREADPOOL_H */
