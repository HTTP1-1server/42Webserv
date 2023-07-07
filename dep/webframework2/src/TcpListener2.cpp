#include "TcpListener.hpp"
#include "AdvancedTcpListener.hpp"

template<>
TcpListener::Socket TcpListener::accept<Quanda>(void) {
	return 1;
};
