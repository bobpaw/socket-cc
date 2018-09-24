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
	const unsigned char END = 192;
	const unsigned char ESC = 219;
	const unsigned char ESC_END = 220;
	const unsigned char ESC_ESC = 221;
	struct sock_exception_e : public std::exception {
	private:
		std::string err_str_;
	public:
		sock_exception_e (const char *func, const char *errstr) {
			err_str_ = "Socket error (";
			err_str_.append(func);
			err_str_.append("): ");
			err_str_.append(errstr);
		}
		const char *what() const noexcept {
			return err_str_.c_str();
		}
	};

	struct sock_exception : public std::exception {
	private:
	  std::string err_str_;
	public:
		sock_exception (const char *func) : err_str_() {
			int e = errno;
			err_str_ = "Socket error (";
			err_str_.append(func);
			err_str_.append("): ");
			err_str_.append(std::strerror(e));
		}
		const char *what() const noexcept {
			return err_str_.c_str();
		}
	}

	class sock_stream {
	protected:
		int fd;
		size_t count_;
		struct addrinfo *result;
	public:
		sock_stream () : sock_stream(NULL, NULL) {
			#ifndef NDEBUG
			std::cerr << "Default socket constructor called." << std::endl;
			#endif
		}
		std::string data;
		size_t count (void) const {return count_;}
		sock_stream (char *host, char *port, int ai_flags = 0, int ai_family = AF_UNSPEC, int ai_socktype = SOCK_STREAM, int ai_protocol = 0) {
			#ifndef NDEBUG
			std::cerr << "Other socket constructor called." << std::endl;
			#endif
			fd = -1;
			result = NULL;
			int s;
			struct addrinfo hints;
			hints.ai_family = ai_family;
			hints.ai_socktype = ai_socktype;
			hints.ai_protocol = ai_protocol;
			hints.ai_flags = ai_flags;
			s = getaddrinfo(host, port, &hints, &result);
			if (s != 0) {
				throw sock_exception_e("getaddrinfo", gai_strerror(s));
			}
			fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
			if (fd < 0) {
				throw sock_exception("socket");
			}
		}
		void sanitize (void) {
			for (int i = 0; i < data.length(); ++i) {
				switch (data[i]) {
				END:
					data[i] = ESC;
					++i;
					data.insert(i, 1, ESC_END);
					break;
				ESC:
					++i;
					data.insert(i, 1, ESC_ESC);
					break;
				}
			}
		}
		void desanitize (void) {
			for (int i = 0; i < data.length(); ++i) {
				if (data[i] == ESC) {
					if (data[i+1] == ESC_ESC) {
						data[i] = ESC;
						data.erase(i+1, 1);
					} else if (data[i+1] == ESC_END) {
						data[i] = END;
						data.erase(i+1, 1);
					}
				}
			}
		}

		// Returns total number of read characters (including END), which will not equal count!
		size_t read (size_t num) {
			data.reserve(num);
			ssize_t total = 0;
			for (ssize_t line = 0; line > -1; total += line) {
				line = recv(fd, data + total, (num - total) < 0 : 0 ? (num - total), 0);
				if (line < 0) {
					throw sock_exception("recv");
				}
				if (total > num || data[data.length() - 1] == END) break;
			}
			if (data[data.length() - 1] == END) data.pop_back();
			desanitize();
			count_ = (size_t) data.length();
			return total;
		}

		size_t write (size_t num) {
			sanitize();
			ssize_t ret = send(fd, data.c_str(), data.length(), 0);
		        char e = END;
			if (ret < 0 || send(fd, &e, 1, 0) != 1) {
				throw sock_exception("send");
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
