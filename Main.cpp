#include "helpers/ClientServerConfig.hpp"
#include "game/SBGame.hpp"
#include <iostream>
#include <memory>
#include "network/TerminalInput.hpp"

int main(int argc, char* argv[]) {
    try {
        std::string ip;
        int port = 0;
        int mode = handle_arguments(argc, argv, ip, port);
        if (mode < 0) return 1;
        if (mode == 2) return 0;
        Socket socket(IPV4, TCP);
        std::unique_ptr<Socket> peer;
        Socket* connection = &socket;
        if (mode == 1) {
            peer.reset(hostServer(port, socket));
            connection = peer.get();
        } else {
            connect(ip, port, socket);
        }
        TerminalInput::peerFd = connection->descriptor();
        SBGame game;
        game.start();
        game.checkReady(connection);
        game.startCommunication(connection, mode == 1);
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
