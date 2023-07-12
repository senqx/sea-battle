#include "IPPortConfig.hpp"

#include <iostream>
#include <string>

bool isValidIp(std::string& ip) {
	int parts[] = {-1, -1, -1, -1};

	std::string num;

	int j = 0;
	for(unsigned int i = 0; i < ip.size(); ++i) {
		if(ip[i] == '.') {
			try {
				parts[j] = std::stoi(num);
			} catch (const std::exception& e) {
				return false;
			}

			if(parts[j] < 0 || parts[j] > 255) {
				return false;
			}
			++j;
			num = "";
		} else if((ip[i] >= '0') && (ip[i] <= '9')) {
			num.push_back(ip[i]);
		} else {
			return false;
		}
	}

	try {
		parts[j] = std::stoi(num);
	} catch (const std::exception& e) {
		return false;
	}

	if(parts[j] < 0 || parts[j] > 255) {
		return false;
	}

	for(int i = 0; i < 4; ++i) {
		if(parts[i] == -1) {
			return false;
		}
	}

	return true;
}

bool isValidPort(int& port) {
	return (port >= 0) && (port <= 65535);
}

std::string getIP() {
	std::string ip;
	std::cout << "Please, enter IP: ";
	std::cin >> ip;

	while(!isValidIp(ip)) {
		std::cerr << "Wrong IP! Please, specify valid one" << std::endl;
		std::cin >> ip;
	}

	return ip;
}

int getPort() {
	int port;

	std::cout << "Please, enter port: ";
	try {
		std::cin >> port;
	} catch (const std::exception& e) {
		port = -1;
	}

	while(!isValidPort(port)) {
		std::cerr << "Invalid port number! Please, specify valid one" << std::endl;
		try {
			std::cin >> port;
		} catch (const std::exception& e) {
			port = -1;
		}
	}

	return port;
}
