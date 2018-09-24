#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>

int main (int argc, char *argv[]) {
  if (argc != 2) return -1;
	int fd = -1, lfd = -1;
	struct addrinfo *result = NULL;
	char *data = NULL;
	int s;
	struct addrinfo hints;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	hints.ai_flags = AI_PASSIVE;
	s = getaddrinfo(NULL, argv[1], &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		return -1;
	}
	fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (fd < 0) {
		perror("socket");
		return -1;
	}
	if (bind(fd, result->ai_addr, result->ai_addrlen) != 0) {
		perror("bind");
		return -1;
	}
	if (listen(fd, 10) != 0) {
		perror("listen");
		return -1;
	}
	lfd = fd;
	fd = accept(lfd, NULL, NULL);
	if (fd == -1) {
		perror("accept");
		return -1;
	}
	data = realloc(data, 32);
	memset(data, 0, 32);
	int ret;
	while (1) {
		ret = recv(fd, data, 32, 0);
		if (ret == -1) {
			perror("recv");
			return -1;
		}
		if (ret == 0 || strchr(data, 4) != NULL) {
			fputs(data, stdout);
		} else break;
	}
	close(fd);
	close(lfd);
	freeaddrinfo(result);
}
