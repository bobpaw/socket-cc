#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <netinet/ip.h>
#include "cmdline-showip.h"

int main (int argc, char * argv[]) {
	struct gengetopt_args_info args = {0};
	if (cmdline_parser(argc, argv, &args) != 0) {
	  fputs("cmdline_parser failed\n", stderr);
	  exit(EXIT_FAILURE);
	}
  struct addrinfo hints = {0}, *result = NULL;
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
  hints.ai_protocol = 0;
  hints.ai_flags = 0;
  for (unsigned int i = 0; i < args.inputs_num; ++i) {
  	r = getaddrinfo(args.inputs[i], NULL, &hints, &result);
  	if (r != 0) {
    	fputs("getaddrinfo", stderr);
    	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
    	exit(EXIT_FAILURE);
  	}
  	fputs("IP Address:", stdout);
  	for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next) {
  	  putchar(' ');
  	  addr = (struct sockaddr_in*) rp->ai_addr;
  	  printf("%s", inet_ntoa(addr->sin_addr));
  	}
  	putchar('\n');
  	freeaddrinfo(result);
  }
  cmdline_parser_free(&args);
  exit(EXIT_SUCCESS);
}
