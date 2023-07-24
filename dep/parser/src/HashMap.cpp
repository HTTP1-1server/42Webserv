#include "HashMap.hpp"
#include <stdexcept>

KeyList HashMap::getKeys() const {
	KeyList keys;
	for (HashMap::const_iterator iter = this->begin(); iter != this->end(); ++iter) {
		keys.push_back(iter->first);
	}
	return keys;
}

const UniquePtr<Any> &HashMap::at(std::string key) const {
	try {
		return std::map<std::string, UniquePtr<Any> >::at(key);
	} catch(const std::exception& e) {
		throw std::runtime_error("key not found: \"" + key + "\"");
	}
}
