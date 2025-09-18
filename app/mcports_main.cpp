#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <future>
#include <asio.hpp>

#define MINIMAL_ARGC	4
using portList = std::vector<asio::ip::port_type>;

bool tryConnect(asio::ip::tcp::socket& socket, const std::string& ip, asio::ip::port_type port) {
	asio::error_code cantConnect;
	socket.connect(asio::ip::tcp::endpoint(asio::ip::make_address(ip), port), cantConnect);
	return !cantConnect;
}

void scanInBorders(	asio::io_context& io,
					const std::string& ip,
					asio::ip::port_type starterPort,
					asio::ip::port_type endPort, 
					asio::ip::port_type portStep,
					std::promise<portList>&& listPromise) {

	asio::ip::tcp::socket socket(io);
	portList foundPorts;

	try {
		for (asio::ip::port_type currentPort = starterPort;
			currentPort <= endPort; currentPort += portStep) {
			if (tryConnect(socket, ip, currentPort)) foundPorts.push_back(currentPort);
			socket.close();
		}
	}
	catch (std::runtime_error exc) {
		std::cout << exc.what() << std::endl;
	}

	listPromise.set_value(foundPorts);
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
	std::vector<std::thread> workers;
	std::vector<std::future<portList> > portLists;

	unsigned int threadCount = std::thread::hardware_concurrency();
	if (threadCount > abs(portUpper - portLower)) {
		threadCount = abs(portUpper - portLower);
	}

	for (unsigned int i = 0; i < threadCount; i++) {
		asio::ip::port_type starterPort = portLower + i,
							endPort = portUpper,
							portStep = threadCount;

		std::promise<portList> listPromise;
		portLists.push_back(listPromise.get_future());
		workers.emplace_back(scanInBorders, std::ref(io), std::cref(ip), 
			starterPort, endPort, portStep, std::move(listPromise));
	}

	portList foundPorts;
	for (int i = 0; i < portLists.size(); i++) {
		portList foundByWorker = portLists[i].get();
		workers[i].join();

		for (asio::ip::port_type port : foundByWorker) foundPorts.push_back(port);
	}

	std::cout << "Active ports: " << std::endl;
	for (asio::ip::port_type port : foundPorts) {
		std::cout << port << std::endl;
	}

	if (foundPorts.empty()) std::cout << "No active ports!" << std::endl;

	return 0;
}