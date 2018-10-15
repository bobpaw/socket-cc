#include <iostream>
#include <string>
#include <exception>
#include <mutex>

#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>

#ifndef SOCKET_H_
#define SOCKET_H_

namespace networking {
	const unsigned char END_MSG = 192;
	const unsigned char END_TX = 193;
	const unsigned char ESC = 219;
	const unsigned char ESC_END_MSG = 220;
	const unsigned char ESC_ESC = 221;
	const unsigned char ESC_END_TX = 222;
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
	};

	class sock_stream {
	protected:
		bool closed_;
		int fd;
		size_t count_;
		struct addrinfo *result;
	public:
		std::string data;

		auto closed (void) const { return closed_; }
		auto count (void) const { return count_; }

		sock_stream () : sock_stream(NULL, NULL) {
			#ifndef NDEBUG
			std::cerr << "Default socket constructor called." << std::endl;
			#endif
		}

		sock_stream (char *host, char *port, int ai_flags = 0, int ai_family = AF_UNSPEC, int ai_socktype = SOCK_STREAM, int ai_protocol = 0);

		void sanitize (void);
		void desanitize (void);

		size_t read (void);
		size_t read (size_t num);
		size_t read (void * ptr, const size_t &n) const;

		size_t write (void);
		size_t write (size_t num);
		size_t write (const void *ptr, const size_t &n) const;

		void send_close (void);
		void s_close (void);

		~sock_stream() {
			s_close();
		}
	}; // class sock_stream
} // namespace networking
#endif
