#pragma once

#include <string>
#include <map>
#include <stdexcept>

class Any;

class Dictionary: public std::multimap<std::string, Any *> {
public:
	typedef std::multimap<std::string, Any *>::iterator				iterator;
	typedef std::multimap<std::string, Any *>::const_iterator		const_iterator;
	typedef std::pair<Dictionary::iterator, Dictionary::iterator>   pair_iterator;

	Any * operator[](std::string key);
};

class Any {
public:
	virtual ~Any() {};
	// template <class T> const T & get() const;
	// template <class T, class U> void setValue(const U &rhs);
	operator int&();
	operator std::string&();
	operator Dictionary&();
	// template <class T> explicit operator T&() const;
};

template <typename T>
class Parameter: public Any {
public:
	Parameter(const T& rhs): value(rhs) {}
	// const T& get() const {
	//     return value;
	// }
	// void setValue(const T& rhs) {
	//     value=rhs;
	// }
	T value;
};

// template<class T> const T& Any::get() const {
//     return dynamic_cast<const Parameter<T>&>(*this).get();
// }

// template<class T, class U> void Any::setValue(const U& rhs) {
//     return dynamic_cast<Parameter<T>&>(*this).setValue(rhs);
// }

Any::operator int&() {
	return dynamic_cast<Parameter<int>&>(*this).value;
};

Any::operator std::string&() {
	return dynamic_cast<Parameter<std::string>&>(*this).value;
};

// template <class T> Any::operator T&() const {
//     return dynamic_cast<const Parameter<T>&>(*this).value;
// };

Any::operator Dictionary&() {
	return dynamic_cast<Parameter<Dictionary>&>(*this).value;
};

Any * Dictionary::operator[](std::string key) {
	pair_iterator range = this->equal_range(key);
	Dictionary::const_iterator start = range.first;
	Dictionary::const_iterator end = range.second;
	if (std::distance(start, end) == 0)
		throw std::runtime_error("no such key");
	if (std::distance(start, end) == 1)
		return range.first->second;
	else
		return range.first->second;
};