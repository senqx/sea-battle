#include "IPPortConfig.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include "../network/TerminalInput.hpp"

bool isValidIp(std::string& ip) {
    unsigned parts = 0, digits = 0, value = 0;
    for (char c : ip) {
        if (c == '.') {
            if (!digits || ++parts > 3) return false;
            digits = value = 0;
        } else if (c >= '0' && c <= '9') {
            if (++digits > 3) return false;
            value = value * 10 + unsigned(c - '0');
            if (value > 255) return false;
        } else return false;
    }
    return parts == 3 && digits != 0;
}

bool isValidPort(int& port) {
    return port >= 1 && port <= 65535;
}

std::string getIP() {
    std::string ip;
    std::cout << "Please, enter IPv4 address: ";
    while (TerminalInput::next(ip)) {
        if (isValidIp(ip)) return ip;
        std::cout << "Invalid IPv4 address. Try again: ";
    }
    throw std::runtime_error("Input closed");
}

int getPort() {
    std::string text;
    std::cout << "Please, enter port [1-65535]: ";
    while (TerminalInput::next(text)) {
        try {
            std::size_t used = 0;
            int port = std::stoi(text, &used);
            if (used == text.size() && isValidPort(port)) return port;
        } catch (const std::exception&) {}
        std::cout << "Invalid port. Try again: ";
    }
    throw std::runtime_error("Input closed");
}
