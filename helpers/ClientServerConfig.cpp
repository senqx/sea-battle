#include "ClientServerConfig.hpp"
#include "IPPortConfig.hpp"
#include <iostream>
#include <string>
#include "../network/TerminalInput.hpp"

Socket* hostServer(int& port, Socket& socket) {
    std::cout << "Hosting server on port: " << port << std::endl;
    socket.Bind(port);
    socket.Listen();
    Socket* peer = socket.Accept();
    std::cout << "A player connected!" << std::endl;
    return peer;
}

void connect(std::string& ip, int& port, Socket& socket) {
    std::cout << "Connecting to " << ip << ':' << port << std::endl;
    socket.Connect(ip, port);
}

int handle_arguments(int argc, char** argv, std::string& ip, int& port) {
    auto usage = [] {
        std::cerr << "Usage: SeaBattle --host PORT\n"
                     "       SeaBattle --connect IPV4 PORT\n"
                     "       SeaBattle (interactive setup)\n";
        return -1;
    };
    if (argc == 1) {
        std::cout << "\n========== SEA BATTLE ==========\n"
                     "1. Host a game\n2. Join a game\n3. Quit\n";
        std::string answer;
        for (;;) {
            std::cout << "Choose [1-3]: " << std::flush;
            if (!TerminalInput::next(answer)) return -1;
            if (answer == "3" || answer == "q" || answer == "Q") return 2;
            if (answer == "1" || answer == "h" || answer == "H") { port = getPort(); return 1; }
            if (answer == "2" || answer == "c" || answer == "C") { ip = getIP(); port = getPort(); return 0; }
            std::cout << "Please choose 1, 2, or 3.\n";
        }
    }
    const std::string option = argv[1];
    const bool host = option == "-h" || option == "--host";
    const bool client = option == "-c" || option == "--connect";
    if ((!host && !client) || argc != (host ? 3 : 4)) return usage();
    if (client) {
        ip = argv[2];
        if (!isValidIp(ip)) return usage();
    }
    try {
        std::string text = argv[host ? 2 : 3];
        std::size_t used;
        port = std::stoi(text, &used);
        if (used != text.size() || !isValidPort(port)) return usage();
    } catch (const std::exception&) { return usage(); }
    return host ? 1 : 0;
}
