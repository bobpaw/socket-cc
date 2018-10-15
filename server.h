#include <string>
#include <iostream>
#include <exception>
#include <thread>

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
				throw sock_exception("bind");
			}
			if (listen(fd, 10) != 0) {
				throw sock_exception("listen");
			}
		}
		~sock_stream_serv () {
			if (lfd != -1) close(lfd);
		}
		void s_accept (void) {
			lfd = fd;
			fd = accept(lfd, NULL, NULL);
			if (fd == -1) throw sock_exception("accept");
		}
	};
}
