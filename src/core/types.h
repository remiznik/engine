#pragma once
#include <string>
#include <vector>
#include <map>

using string = std::string;

template<typename T>
using vector = std::vector<T>;

template <class Key, class T, class Comparator = std::less<Key>>
using map = std::map<Key, T, Comparator>;
