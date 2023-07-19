#pragma once

#include <string>
#include <map>

class Any;

typedef std::map<std::string, Any *>	Dictionary;

class Any {
public:
	virtual ~Any() {};
	template <class T> operator T&();
};

template <typename T>
class AnyType: public Any {
public:
	AnyType(const T& rhs): value(rhs) {}
	T value;
};

template <class T> inline Any::operator T &() {
	return dynamic_cast<AnyType<T>&>(*this).value;
}
