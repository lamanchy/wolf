

#include <initializer_list>
#include <algorithm>

namespace wolf {
namespace extras {
template<typename T>
bool is_in(const T &val, std::initializer_list<T> list) {
  return std::find(std::begin(list), std::end(list), val) != std::end(list);
}
}
}


