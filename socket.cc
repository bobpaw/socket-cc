#include "socket.h"

namespace networking {
std::mutex read_write;

  sock_stream::sock_stream (char *host, char *port, int ai_flags, int ai_family, int ai_socktype, int ai_protocol) {
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

  void sock_stream::sanitize (void) {
    for (auto i = std::string::npos ^ std::string::npos; i < data.length(); ++i) {
      if (data[i] == END_MSG) {
	data[i] = ESC;
	++i;
	data.insert(i, 1, ESC_END_MSG);
      } else if (data[i] == ESC) {
	++i;
	data.insert(i, 1, ESC_ESC);
      } else if (data[i] == END_TX) {
	++i;
	data.insert(i, 1, ESC_END_TX);
      }
    }
  }

  void sock_stream::desanitize (void) {
    for (auto i = std::string::npos ^ std::string::npos; i < data.length(); ++i) {
      if (data[i] == ESC) {
	if (data[i + 1] == ESC_ESC) {
	  data[i] = ESC;
	  data.erase(i + 1, 1);
	} else if (data[i + 1] == ESC_END_MSG) {
	  data[i] = END_MSG;
	  data.erase(i + 1, 1);
	} else if (data[i + 1] == ESC_END_TX) {
	  data[i] = END_TX;
	  data.erase(i + 1, 1);
	}
      }
    }
  }

  size_t sock_stream::read (void) {
    read_write.lock();
    auto filled = std::string::npos ^ std::string::npos;
    data.reserve(32);
    char one = '\0';
    while (one != END_MSG) {
      if (recv(fd, &one, 1, 0) < 0) throw sock_exception("recv");
      if (one == END_MSG) break;
      if (one == END_TX) {
	s_close();
	break;
      }
      data.push_back(one);
      ++filled;
      if (filled == data.length()) data.reserve(filled << 1);
    }
    desanitize();
    count_ = data.length();
    read_write.unlock();
    return filled;
  }

  size_t sock_stream::write (void) {
    read_write.lock();
    sanitize();
    ssize_t ret = send(fd, data.c_str(), data.length(), 0);
    if (ret < 0 || send(fd, &END_MSG, 1, 0) != 1) {
      throw sock_exception("send");
    }
    read_write.unlock();
    return (size_t) ret;
  }

  // Returns total number of read characters (including END), which will not equal count!
  size_t sock_stream::read (size_t num) {
    read_write.lock();
    char *buffer = new char[num];
    auto line = recv(fd, buffer, num, 0);
    if (line < 0) {
      throw sock_exception("recv");
    }
    data.assign(buffer, num);
    if (data[data.length() - 1] == END_MSG) data.pop_back();
    desanitize();
    count_ = (size_t) data.length();
    read_write.unlock();
    return line;
  }

  size_t sock_stream::write (size_t num) {
    read_write.lock();
    sanitize();
    ssize_t ret = send(fd, data.c_str(), num, 0);
    if (ret < 0 || send(fd, &END_MSG, 1, 0) != 1) {
      throw sock_exception("send");
    }
    read_write.unlock();
    return (size_t) ret;
  }

  size_t sock_stream::read (void * ptr, const size_t &n) const {
    ssize_t ret = recv(fd, ptr, n, 0);
    if (ret < 0) {
      throw sock_exception("recv");
    }
    return (size_t) ret;
  }

  size_t sock_stream::write (const void *ptr, const size_t &n) const {
    ssize_t ret = send(fd, ptr, n, 0);
    if (ret < 0) {
      throw sock_exception("send");
    }
    return (size_t) ret;
  }

  void sock_stream::send_close (void) {
    if (!closed_) {
      write(&END_TX, 1);
    }
  }

  void sock_stream::s_close (void) {
    if (!closed_) {
      if (result != NULL) {
	freeaddrinfo(result);
	result = NULL;
      }
      if (fd != -1) {
	close(fd);
	fd = -1;
      }
      closed_ = true;
    }
  }
}
