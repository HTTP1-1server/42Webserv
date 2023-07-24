#include "HashMap.hpp"

KeyList HashMap::getKeys() const {
	KeyList keys;
	for (HashMap::const_iterator iter = this->begin(); iter != this->end(); ++iter) {
		keys.push_back(iter->first);
	}
	return keys;
}