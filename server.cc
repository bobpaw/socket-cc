#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <exception>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "socket.h"

namespace networking {
	struct sock_exception : public std::exception {
		const char *what() const noexcept {
			return "Socket error";
		}
	};

	class sock_stream_serv : public sock_stream {
	protected:
		int fd;
		int lfd;
		struct addrinfo *result;
	public:
	char *data;
	size_t count_;
	size_t count (void) {return count_;}
		sock_stream_serv () : sock_stream() {
			lfd = 0;
		}
		sock_stream_serv (char *port) : sock_stream_serv(), sock_stream(host, port, AI_PASSIVE) {
			if (bind(fd, rp->ai_addr, rp->ai_addrlen) != 0) {
				throw sock_exception("bind", std::strerror(errno));
			}
			if (listen(fd, 20) != 0) {
				throw sock_exception("listen", std::strerror(errno));
			}
		}
		~sock_stream_serv () {
			if (lfd != -1) close(lfd);
		}
		void s_accept (void) {
			lfd = fd;
			fd = accept(lfd, NULL, NULL);
		}
	};
}

int main (int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << "port" << std::endl;
		exit(EXIT_FAILURE);
	}
	try {
		::sock_stream_serv a(argv[1]);
		a.s_accept();
		int me = fork();
		std::string buf;
		while (!a.eof() && waitpid(-1, NULL, WNOHANG) != 0) {
			if (me == 0) {
				a.read(64);
				std::cout << a.data << std::endl;
			} else {
				std::getline(std::cin, buf);
				std::fwrite(buf.c_str(), buf.length(), 1, a.stream);
			}
		}
	} catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
