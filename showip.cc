#include <iostream>

#include "socket.h"
#include "cmdline-showip.h"

int main (int argc, char *argv[]) {
	struct gengetopt_args_info args;
	cmdline_parser(argc, argv, &args);
	if (args.inputs_num < 1) {
		std::cerr << "Please supply at least one server\n" << std::endl;
		cmdline_parser_free(&args);
		return -1;
	}
	struct addrinfo hints, *result = NULL;
	struct sockaddr_in *addr = NULL;
	int r = 0;
	if (args.ipv4_given) {
		hints.ai_family = AF_INET;
	} else if (args.ipv6_given) {
		hints.ai_family = AF_INET6;
	} else {
		hints.ai_family = AF_UNSPEC;
	}
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = 0;
	for (unsigned int i = 0; i < args.inputs_num; ++i) {
		r = getaddrinfo(args.inputs[i], NULL, &hints, &result);
		if (r != 0) {
			std::cerr << "Error: getaddrinfo" << std::endl;
			std::cerr << gai_strerror(r) << std::endl;
			cmdline_parser_free(&args);
			return -1;
		}
			for (auto rp = result; rp != NULL; rp = rp->ai_next) {
			addr = (struct sockaddr_in*) result->ai_addr;
			std::cout << "IP Address: " << inet_ntoa(addr->sin_addr) << std::endl;
		}
		freeaddrinfo(result);
	}
	cmdline_parser_free(&args);
	return 0;
}
