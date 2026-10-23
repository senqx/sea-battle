#ifndef SEA_BATTLE_SOCKET_HPP
#define SEA_BATTLE_SOCKET_HPP

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>

class SocketException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

enum { IPV4, TCP };

// Blocking IPv4 TCP connection. Read consumes exactly the requested byte count.
class Socket {
    int fd_;
    explicit Socket(int fd) : fd_(fd) {}
    static void fail(const char* action) {
        if (errno == ECONNRESET || errno == EPIPE)
            throw SocketException("Opponent disconnected. Game ended.");
        throw SocketException(std::string(action) + ": " + std::strerror(errno));
    }
public:
    Socket(int, int) : fd_(::socket(AF_INET, SOCK_STREAM, 0)) {
        if (fd_ < 0) fail("socket");
    }
    ~Socket() { if (fd_ >= 0) ::close(fd_); }
    int descriptor() const { return fd_; }
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    void Bind(int port) {
        int reuse = 1;
        ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);
        if (::bind(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) fail("bind");
    }
    void Listen() { if (::listen(fd_, 1) < 0) fail("listen"); }
    Socket* Accept() {
        int accepted;
        do { accepted = ::accept(fd_, nullptr, nullptr); } while (accepted < 0 && errno == EINTR);
        if (accepted < 0) fail("accept");
        return new Socket(accepted);
    }
    void Connect(const std::string& ip, int port) {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (::inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) != 1)
            throw SocketException("Invalid IPv4 address");
        if (::connect(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) fail("connect");
    }
    void Write(const std::string& data) {
        std::size_t sent = 0;
        while (sent < data.size()) {
            auto count = ::send(fd_, data.data() + sent, data.size() - sent, MSG_NOSIGNAL);
            if (count < 0 && errno == EINTR) continue;
            if (count <= 0) fail("send");
            sent += static_cast<std::size_t>(count);
        }
    }
    std::string Read(std::size_t bytes) {
        std::string data(bytes, '\0');
        std::size_t received = 0;
        while (received < bytes) {
            auto count = ::recv(fd_, &data[received], bytes - received, 0);
            if (count < 0 && errno == EINTR) continue;
            if (count < 0) fail("receive");
            if (count == 0) {
                if (received) throw SocketException("Opponent disconnected during a message");
                return {};
            }
            received += static_cast<std::size_t>(count);
        }
        return data;
    }
};
#endif
