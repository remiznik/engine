#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stack>
#include <set>
#include <unordered_map>
#include <cstdio>
#include <cstring>
#include <unordered_map>


using uint16_t = std::uint16_t;

using string = std::string;


template<typename T, class _Alloc = std::allocator<T>>
using vector = std::vector<T, _Alloc>;

template <class Key, class T, class Comparator = std::less<Key>>
using map = std::map<Key, T, Comparator>;

template <class Key, class T>
using unordered_map = std::unordered_map<Key, T>;

template <class T, class Comparator = std::less<T>>
using set = std::set<T, Comparator>;

template <class T>
using shared_ptr = std::shared_ptr<T>;

template<class T>
using stack = std::stack<T>;


template<class T, class _Alloc = std::allocator<T>, class... _Types>
shared_ptr<T> makeShared(_Types&&... _Args)
{
    static const _Alloc alloc;
    return std::allocate_shared<T, _Alloc>(alloc, std::forward<_Types>(_Args)...);
}


template<class T>
class ScopeGuard
{
public:
	ScopeGuard(T& v)
		: value(v)
	{
		oldvalue = v;

	}

	~ScopeGuard()
	{
		value = oldvalue;
	}
private:
	T& value;
	T oldvalue;
};