#pragma once

#include <map>
#include <string>
#include <vector>
#include "UniquePtr.hpp"
#include "Any.hpp"

typedef std::vector<std::string> KeyList;

class HashMap: public std::map<std::string, UniquePtr<Any> > {
public:
	KeyList getKeys() const;
	UniquePtr<Any> &at(std::string key);
	const UniquePtr<Any> &at(std::string key) const;
};