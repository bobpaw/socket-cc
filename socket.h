#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <exception>
#include <unistd.h>
#include <sys/types.h>

#ifndef SOCKET_H_
#define SOCKET_H_

namespace networking {
	struct sock_exception : public std::exception {
	private:
	  std::string err_str_;
	public:
	  sock_exception (const char *func, const char *errstr) {
	    err_str_ = "Socket error (";
	    err_str_.append(func);
	    err_str_.append("): ");
	    err_str_.append(errstr);
	  }
		const char *what() const noexcept {
			return err_str_.c_str();
		}
	};

	class sock_stream {
	protected:
		int fd;
		size_t count_;
		struct addrinfo *result;
	public:
		sock_stream () : sock_stream(NULL, NULL) {
		  std::cerr << "Default socket constructor called." << std::endl;
		}
		char *data;
		size_t count (void) const {return count_;}
		sock_stream (char *host, char *port, int ai_flags = 0, int ai_family = AF_UNSPEC, int ai_socktype = SOCK_STREAM, int ai_protocol = 0) {
			std::cerr << "Other socket constructor called." << std::endl;
			fd = -1;
			result = NULL;
			data = NULL;
			int s;
			struct addrinfo hints;
			hints.ai_family = ai_family;
			hints.ai_socktype = ai_socktype;
			hints.ai_protocol = ai_protocol;
			hints.ai_flags = ai_flags;
			s = getaddrinfo(host, port, &hints, &result);
			if (s != 0) {
				throw sock_exception("getaddrinfo", gai_strerror(s));
			}
			fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
			if (fd < 0) {
				throw sock_exception("socket", std::strerror(errno));
			}
		}
		size_t read (size_t num) {
			if (data) delete[] data;
			data = (char *) new char[num];
			std::memset(data, 0, num);
			ssize_t ret = recv(fd, data, num, 0);
			if (ret < 0) {
				throw sock_exception("recv", std::strerror(errno));
			}
			count_ = (size_t) ret;
			return count_;
		}
		size_t write (size_t num) {
			ssize_t ret = send(fd, data, num, 0);
			if (ret < 0) {
				throw sock_exception("send", std::strerror(errno));
			}
			return (size_t) ret;
		}
		size_t readp (void * ptr, size_t n) const {
			ssize_t ret = recv(fd, ptr, n, 0);
			if (ret < 0) {
				throw sock_exception("recv", std::strerror(errno));
			}
			return (size_t) ret;
		}
		size_t writep (const void *ptr, size_t n) const {
			ssize_t ret = send(fd, ptr, n, 0);
			if (ret < 0) {
				throw sock_exception("send", std::strerror(errno));
			}
			return (size_t) ret;
		}
		~sock_stream() {
			if (data != NULL) {
			delete[] data;
			}
			if (result!=NULL) {
			freeaddrinfo(result);
			}
			if (fd != -1) close(fd);
		}
	}; // class sock_stream
} // namespace networking
#endif
