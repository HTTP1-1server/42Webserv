#pragma once

#include <string>
#include <map>
#include <vector>

typedef std::vector<std::string>		KeyList;

class Any {
public:
	virtual ~Any() {};
	template <class T> operator T&();
};

template <typename T>
class AnyType: public Any {
public:
	AnyType(const T& rhs): value(rhs) {};
	T value;
};

template <class T> inline Any::operator T &() {
	return dynamic_cast<AnyType<T>&>(*this).value;
}

class HashMap: public std::map<std::string, Any *> {
public:
	KeyList getKeys() const {
		KeyList keys;
		for (HashMap::const_iterator iter = this->begin(); iter != this->end(); ++iter) {
			keys.push_back(iter->first);
		}
		return keys;
	}
};