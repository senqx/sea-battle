#include "ClientServerConfig.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "../Socket/Socket.hpp"
#include "IPPortConfig.hpp"

Socket* hostServer(int& port, Socket& s) {
	std::cout << "Hosting server on port: " << port << std::endl;

	try {
		s.Bind(port);
	} catch(const SocketException& e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}
	s.Listen();
	Socket* ans = s.Accept();
	std::cout << "A player connected!" << std::endl;
	return ans;
}

void connect(std::string& ip, int& port, Socket& s) {
	std::cout << "Attempting to connect to " << ip << ':' << port << std::endl;

	try {
		s.Connect(ip, port);
	} catch(const SocketException& e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}
}

int handle_arguments(int argc, char** argv, std::string& ip, int& port) {
	if(argc == 1) {
		std::cout << "[H]ost or [C]onnect? : ";

		std::string answer;
		std::cin >> answer;

		if(answer[0] == 'H' || answer[0] == 'h') {
			port = getPort();
			return 1;
		} else if(answer[0] == 'C' || answer[0] == 'c') {
			ip = getIP();
			port = getPort();
			return 0;
		}

		std::cerr << "Please, give valid answer" << std::endl;
		return -1;
	} else if(argc == 3 || argc == 4) {
		std::string option;
		std::vector<std::string> vals;

		for(int i = 1; i < argc; ++i) {
			std::string a = argv[i];
			if(a[0] == '-') {
				option = a;
			} else {
				vals.push_back(a);
			}
		}

		if(option == "-h" || option == "--host") {
			try {
				port = std::stoi(std::string(vals[0]));
			} catch(std::exception& e) {
				port = -1;
			}

			if(!isValidPort(port)) {
				std::cerr << "Wrong port!" << std::endl;
				return -1;
			}
			return 1;
		} else if(option == "-c" || option == "--connect") {
			ip = vals[0];
			if(!isValidIp(ip)) {
				std::cerr << "Wrong IP!" << std::endl;
				return -1;
			}

			try {
				port = std::stoi(std::string(vals[1]));
			} catch(std::exception& e) {
				port = -1;
			}

			if(!isValidPort(port)) {
				std::cerr << "Wrong port!" << std::endl;
				return -1;
			}

			return 0;
		}

		std::cerr << "Wrong options given" << std::endl;
		return -1;
	}

	std::cerr << "Wrong argument count" << std::endl;
	return -1;
}

