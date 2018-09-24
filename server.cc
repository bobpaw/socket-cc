#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <csignal>
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
	class sock_stream_serv : public sock_stream {
	protected:
		int lfd;
	public:
		sock_stream_serv () : sock_stream_serv(NULL) {}
		sock_stream_serv (char *port) : sock_stream(NULL, port, AI_PASSIVE) {
		  std::cerr << "server constructor called." << std::endl;
		  lfd = -1;
			if (bind(fd, result->ai_addr, result->ai_addrlen) != 0) {
				throw sock_exception("bind", std::strerror(errno));
			}
			if (listen(fd, 10) != 0) {
				throw sock_exception("listen", std::strerror(errno));
			}
		}
		~sock_stream_serv () {
			if (lfd != -1) close(lfd);
		}
		void s_accept (void) {
			lfd = fd;
			fd = accept(lfd, NULL, NULL);
			if (fd == -1) throw sock_exception("accept", std::strerror(errno));
		}
	};
}

void kill_child (void) {
  kill(0, SIGTERM);
}

sig_atomic_t usr1given = 0;

void handle (int a) {
if (a == SIGTERM) usr1given = 1;
}
int main (int argc, char *argv[]) {
  atexit(*kill_child);
  signal(SIGTERM, *handle);
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " port" << std::endl;
		return -1;
	}
	try {
		networking::sock_stream_serv a(argv[1]);
		a.s_accept();
		int me = fork();
		std::string buf;
		while (waitpid(-1, NULL, WNOHANG) == 0 && usr1given != 1) {
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
		return -1;
	}
  return 0;
}
