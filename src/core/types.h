#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

using string = std::string;

template<typename T>
using vector = std::vector<T>;

template <class Key, class T, class Comparator = std::less<Key>>
using map = std::map<Key, T, Comparator>;

template <class T>
using shared_ptr = std::shared_ptr<T>;