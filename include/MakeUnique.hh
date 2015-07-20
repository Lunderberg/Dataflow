#ifndef _MAKEUNIQUE_H_
#define _MAKEUNIQUE_H_

#include <memory>

template<typename T, typename... Params>
inline std::unique_ptr<T> make_unique(Params&&... params){
  return std::unique_ptr<T>(new T(std::forward<Params>(params)...));
}

#endif /* _MAKEUNIQUE_H_ */
