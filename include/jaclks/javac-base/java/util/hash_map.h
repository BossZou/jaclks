#pragma once

#include <unordered_map>

#include "jaclks/javac-base/java/util/map.h"

namespace jaclks::javac_base {

template <typename K, typename V>
class HashMap : Map<K, V> {
private:
  std::unordered_map<K, V> map_;
};

}
