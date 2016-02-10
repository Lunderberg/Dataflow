#ifndef DIRECTTASK_H
#define DIRECTTASK_H

#include <functional>
#include <tuple>

#include "dataflow/Indices.hh"
#include "dataflow/Task.hh"
#include "dataflow/ThreadsafeQueue.hh"

namespace dataflow {
  template<typename T, typename U>
  class DirectTask;

  template<typename... InTypes, typename... OutTypes>
  class DirectTask<std::tuple<InTypes...>,
                   std::tuple<OutTypes...> > : public Task {
  public:
    DirectTask(std::tuple<InputQueue<InTypes>&...> in_types,
               std::function<dataflow::IterationResult(InputQueue<InTypes>&...,
                                                       OutputQueue<OutTypes>&...)> func,
               std::tuple<OutputQueue<OutTypes>&...> out_types)
      : inputs(in_types),
        func(func),
        outputs(out_types)
      { }

    virtual ~DirectTask() { }

    IterationResult apply() {
      return apply_helper(build_indices<sizeof...(InTypes)>(),
                          build_indices<sizeof...(OutTypes)>());
    }

  private:
    template<int... InIndices, int... OutIndices>
    IterationResult apply_helper(indices<InIndices...>, indices<OutIndices...>) {
      return func(std::get<InIndices>(inputs)...,
                  std::get<OutIndices>(outputs)...);
    }



    std::tuple<InputQueue<InTypes>&...> inputs;
    std::function<dataflow::IterationResult(InputQueue<InTypes>&..., OutputQueue<OutTypes>&...)> func;
    std::tuple<OutputQueue<OutTypes>&...> outputs;
  };
}

#endif /* DIRECTTASK_H */
