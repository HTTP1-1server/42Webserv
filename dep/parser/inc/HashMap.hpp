#pragma once

#include <map>
#include <string>
#include <vector>
#include "UniquePtr.hpp"
#include "Any.hpp"

typedef std::vector<std::string> KeyList;

class HashMap: public std::map<std::string, UniquePtr<Any> > {
public:
	KeyList getKeys() const {
		KeyList keys;
		for (HashMap::const_iterator iter = this->begin(); iter != this->end(); ++iter) {
			keys.push_back(iter->first);
		}
		return keys;
	}
};