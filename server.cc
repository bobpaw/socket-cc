#include "server.h"

void read_loop (networking::sock_stream &fd) {
	while (!fd.closed()) {
		fd.read();
		std::cout << fd.data << std::endl;
	}
}

void write_loop (networking::sock_stream &fd) {
	while (!fd.closed()) {
		std::getline(std::cin, fd.data);
		fd.write();
		if (std::cin.eof()) fd.send_close();
	}
}
int main (int argc, char *argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " port" << std::endl;
		return -1;
	}
	try {
		networking::sock_stream_serv a(argv[1]);
		a.s_accept();
		std::thread reader(read_loop, std::ref(a));
		std::thread writer(write_loop, std::ref(a));
		writer.join();
		reader.join();
	} catch (std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return -1;
	}
  return 0;
}
