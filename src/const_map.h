#ifndef CONST_MAP
#define CONST_MAP

#include <algorithm>

template<typename K, typename V>
class const_map {
public:
  constexpr const_map(const std::initializer_list<std::pair<K, V>> il) : il(il) {
  }
  V at(K k) const {
    for(auto entry : il) {
      if (entry.first == k)
        return entry.second;
    }
    throw std::out_of_range("No such key");
  }
private:
  std::initializer_list<std::pair<K, V>> il;
};

template<typename T1, typename T2>
struct const_pair {
  template<T1 First, T2 Second>
  struct instance {
    static const T1 first = First;
    static const T2 second = Second;
  };
};

template<typename K, typename V>
V const_map_at(K k) {
  throw std::out_of_range("No such key");
}

template<typename K, typename V, typename Entry, typename ... Entries>
V const_map_at(K k) {
  if (Entry::first == k)
    return Entry::second;
  else
    return const_map_at<K, V, Entries...>(k);
}

template<typename K, typename V, typename ... Entries>
struct ConstMapping
{
  static V at(K k) {
    return const_map_at<K, V, Entries...>(k);
  }
};

template<auto k, auto v> using CMEntry =
  typename const_pair<decltype(k), decltype(v)>::template instance<k, v>;


#endif
