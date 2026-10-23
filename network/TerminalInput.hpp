#ifndef SEA_BATTLE_TERMINAL_INPUT_HPP
#define SEA_BATTLE_TERMINAL_INPUT_HPP

#include "Socket.hpp"
#include <poll.h>
#include <cctype>
#include <iostream>

// Read terminal tokens while watching for a closed connection, including during
// fleet placement or a player's turn. Do not consume pending game messages.
namespace TerminalInput {
inline int peerFd = -1;
inline std::string pending;
inline bool ended = false;
inline bool next(std::string& token) {
    std::cout << std::flush;
    for (;;) {
        pollfd descriptors[] = {{STDIN_FILENO, POLLIN, 0}, {peerFd, POLLRDHUP, 0}};
        int result;
        do { result = ::poll(descriptors, 2, 0); } while (result < 0 && errno == EINTR);
        if (result < 0) throw SocketException("Unable to read terminal input");
        if (descriptors[1].revents & (POLLRDHUP | POLLHUP | POLLERR | POLLNVAL))
            throw SocketException("Opponent disconnected. Game ended.");
        auto start = pending.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) pending.clear();
        else {
            if (start) pending.erase(0, start);
            auto end = pending.find_first_of(" \t\r\n");
            if (end != std::string::npos || ended) {
                token = pending.substr(0, end);
                pending.erase(0, end == std::string::npos ? pending.size() : end + 1);
                return true;
            }
        }
        if (ended) return false;
        do { result = ::poll(descriptors, 2, -1); } while (result < 0 && errno == EINTR);
        if (result < 0) throw SocketException("Unable to read terminal input");
        if (descriptors[1].revents & (POLLRDHUP | POLLHUP | POLLERR | POLLNVAL))
            throw SocketException("Opponent disconnected. Game ended.");
        if (descriptors[0].revents & (POLLIN | POLLHUP)) {
            char buffer[1024];
            auto count = ::read(STDIN_FILENO, buffer, sizeof(buffer));
            if (count < 0 && errno == EINTR) continue;
            if (count < 0) throw SocketException("Unable to read terminal input");
            if (count == 0) ended = true;
            else pending.append(buffer, static_cast<std::size_t>(count));
        }
    }
}
}
#endif
