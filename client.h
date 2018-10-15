#include <string>
#include <iostream>
#include <exception>
#include <thread>

#include <sys/socket.h>
#include <sys/types.h>
#include "socket.h"

namespace networking {
	class sock_stream_cli : public sock_stream {
	public:
		sock_stream_cli(char *host, char *port) : sock_stream(host, port) {
		  #ifndef NDEBUG
		  std::cerr << "Client socket started" << std::endl;
		  #endif
			if (connect(fd, result->ai_addr, result->ai_addrlen) == -1) {
				throw sock_exception("connect");
			}
		}
	};
}
