#include "helpers/ClientServerConfig.hpp"
#include "game/SBGame.hpp"

bool YOU_ARE_HOST = false;

int main(int argc, char* argv[]) {
	std::string ip;
	int port;
	
	Socket s(IPV4, TCP);
	SBGame game;
	
	int res = handle_arguments(argc, argv, ip, port);
	
	if(res == 1) {
		// You are Hosting
		Socket* ans = hostServer(port, s);
		YOU_ARE_HOST = true;
		game.start();
		game.checkReady(ans);
		game.startCommunication(ans, YOU_ARE_HOST);	
	} else if(res == 0) {
		// You are Client
		connect(ip, port, s);
		YOU_ARE_HOST = false;
		game.start();
		game.checkReady(&s);
		game.startCommunication(&s, YOU_ARE_HOST);	
	} else {
		exit(1);
	}
	return 0;
}
