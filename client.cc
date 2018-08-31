#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <exception>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "socket.h"

namespace networking {
	class sock_stream_cli : public sock_stream {
	public:
		sock_stream_cli(char *host, char *port) : sock_stream(host, port) {
			if (connect(fd, result->ai_addr, result->ai_addrlen) == -1) {
				throw sock_exception("connect", std::strerror(errno));
			}
		}
	};
}

int main (int argc, char *argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " host port" << std::endl;
		exit(EXIT_FAILURE);
	}
	int exit_status = EXIT_SUCCESS;
	try {
	  networking::sock_stream_cli a(argv[1], argv[2]);
		int me = fork();
		std::string buf;
		while (waitpid(-1, NULL, WNOHANG) != 0) {
			if (me == 0) {
				a.read(64);
				std::cout << a.data << std::endl;
			} else {
				std::getline(std::cin, buf);
				a.writep(buf.c_str(), buf.length());
			}
		}
	} catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	  exit_status = EXIT_FAILURE;
	}
	exit(exit_status);
}
