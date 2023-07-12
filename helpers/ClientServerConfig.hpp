#ifndef __CLIENT_SERVER_CONFIG__
#define __CLIENT_SERVER_CONFIG__

#include <string>

#include "../Socket/Socket.hpp"

Socket* hostServer(int& port, Socket& s);

void connect(std::string& ip, int& port, Socket& s);

// Hande user arguments (host, or connect? ip and port)
int handle_arguments(int argc, char** argv, std::string& ip, int& port);

#endif
