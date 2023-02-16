#include <iostream>
#include <vector>
#include "Socket/Socket.hpp"

bool isValidIp(std::string& ip) {
	int parts[] = {-1, -1, -1, -1};
	
	std::string num;
	
	int j = 0;
	for(unsigned int i = 0; i < ip.size(); ++i) {
		if(ip[i] == '.') {
			try {
				parts[j] = std::stoi(num);
			} catch (std::exception& e) {
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
	} catch (std::exception& e) {
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
		std::cerr << 
			"Wrong IP! Please, specify valid one" << std::endl;
		std::cin >> ip;
	}

	return ip;
}

int getPort() {
	int port;

	std::cout << "Please, enter port: ";
	try {
		std::cin >> port;
	} catch (std::exception& e) {
		port = -1;
	}

	while(!isValidPort(port)) {
		std::cerr << 
			"Invalid port number! Please, specify valid one" <<
			std::endl;

		try {
			std::cin >> port;
		} catch (std::exception& e) {
			port = -1;
		}
	}

	return port;
}


void hostServer(int& port) {
	std::cout << "Hosting server on port: " << port << std::endl;

	Socket s(IPV4, TCP);
	try {
		s.Bind(port);
	} catch(const SocketException& e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}
	s.Listen();
	Socket ans = s.Accept();
	std::cout << "A player connected!" << std::endl;
	while(true) {
		std::string msg = ans.Read(1024);
		if(msg == "") {
			exit(0);
		}
		std::cout << "Got message: " << msg << std::endl;
		ans.Write(msg);
	}
}

void connect(std::string& ip, int& port) {
	std::cout << "Attempting to connect to " << ip << ':' << port << std::endl;
	Socket s(IPV4, TCP);
	try {
		s.Connect(ip, port);
	} catch(const SocketException& e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}

	while(true) {
		std::cout << "Enter input: ";
		std::string msg;
		std::getline(std::cin, msg);
		s.Write(msg);
		std::cout << s.Read(1024) << std::endl;
	}
}

int main(int argc, char* argv[]) {
	std::string ip;
	int port;

	if(argc == 1) {
		std::cout << "[H]ost or [C]onnect? : ";
		std::string answer;
		std::cin >> answer;
		if(answer[0] == 'H' || answer[0] == 'h') {
			port = getPort();
			// Start server
			hostServer(port);
		} else if(answer[0] == 'C' || answer[0] == 'c') {
			ip = getIP();
			port = getPort();
			// Conncect
			connect(ip, port);
		} else {
			std::cerr << "Please, give valid answer" << std::endl;
			exit(1);
		}
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
				exit(1);
			}

			hostServer(port);
		} else if(option == "-c" || option == "--connect") {
			std::string& ip = vals[0];
			if(!isValidIp(ip)) {
				std::cerr << "Wrong IP!" << std::endl;
				exit(1);
			}
			
			try {
				port = std::stoi(std::string(vals[1]));
			} catch(std::exception& e) {
				port = -1;
			}

			if(!isValidPort(port)) {
				std::cerr << "Wrong port!" << std::endl;
				exit(1);
			}

			connect(ip, port);
		} else {
			std::cerr << "Wrong options given" << std::endl;
		}
	} else {
		std::cerr << "Wrong argument count" << std::endl;
		exit(1);
	}

	return 0;
}
