#include <iostream>
#include <string>
#include <asio.hpp>

#define MINIMAL_ARGC	4

bool tryConnect(asio::ip::tcp::socket& socket, const std::string& ip, asio::ip::port_type port) {
	asio::error_code cantConnect;
	socket.connect(asio::ip::tcp::endpoint(asio::ip::make_address(ip), port), cantConnect);
	return !cantConnect;
}

int main(int argc, char** argv) {
	if (argc < MINIMAL_ARGC) {
		std::cerr << "Not enough args! (" << argc << ")" << std::endl;
		return -1;
	}

	std::string ip(argv[1]);
	asio::ip::port_type	portLower = std::stoul(argv[2]), 
						portUpper = std::stoul(argv[3]);

	asio::io_context io;
	asio::ip::tcp::socket socket(io);

	for (asio::ip::port_type currentPort = portLower; currentPort <= portUpper; currentPort++) {
		std::cout << "Port " << currentPort;
		if (tryConnect(socket, ip, currentPort)) {
			std::cout << " active" << std::endl;
		}
		else std::cout << " inactive" << std::endl;
		socket.close();
	}

	return 0;
}