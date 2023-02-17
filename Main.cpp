#include <exception>
#include <iostream>
#include <vector>
#include "SBGame.hpp"
#include "Socket/Socket.hpp"

bool YOU_ARE_HOST = false;

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

void startGame() {
	SBGame game;
	game.print();
	game.setUpFleet();
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

int main(int argc, char* argv[]) {
	std::string ip;
	int port;
	
	Socket s(IPV4, TCP);
	
	int res = handle_arguments(argc, argv, ip, port);
	
	if(res == 1) {
		// You are Hosting
		Socket* ans = hostServer(port, s);
		YOU_ARE_HOST = true;
		while(true) {
			std::string msg; 
			try {
				msg = ans->Read(1024);
			} catch(std::exception& e) {
				std::cerr<< e.what() << std::endl;
				delete ans;
				exit(1);
			}
			if(msg == "") {
				std::cout << "Client closed the connection" << std::endl;
				delete ans;
				break;
			}
			std::cout << "Got message: " << msg << std::endl;
			ans->Write(msg);
		}
	} else if(res == 0) {
		// You are Client
		connect(ip, port, s);
		YOU_ARE_HOST = false;
		while(true) {
			std::cout << "Enter input: ";
			std::string msg;
			std::getline(std::cin, msg);
			try {
				s.Write(msg);
			} catch(std::exception& e) {
				std::cerr << e.what() << std::endl;
				std::cout << "Server closed" << std::endl;
				break;
			}
			std::string ans;
			try {
				ans = s.Read(1024);
			} catch(std::exception& e) {
				std::cerr << e.what() << std::endl;
			}
			if(ans == "") {
				std::cout << "Server closed!" << std::endl;
				break;
			}
			std::cout << "Got response: " << ans << std::endl;
		}
	} else {
		exit(1);
	}
	return 0;
}
