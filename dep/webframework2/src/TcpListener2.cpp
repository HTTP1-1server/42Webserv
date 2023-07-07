#include "TcpListener.hpp"

template<>
TcpListener::Socket TcpListener::accept<Quanda>(void) {
	return Select;
};
