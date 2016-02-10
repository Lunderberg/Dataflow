#ifndef INDICES_H
#define INDICES_H

namespace dataflow {
  template<int...>
  struct indices { };

  template<int N, int... index>
  struct build_indices : build_indices<N-1, N-1, index...> { };

  template<int... index>
  struct build_indices<0, index...> : indices<index...> { };
}

#endif /* INDICES_H */
