#ifndef __S_B_GAME_
#define __S_B_GAME_

#include <iostream>
#include <string>
#include "Socket/Socket.hpp"

class SBGame {
	static constexpr unsigned int WIDTH = 84;
	static constexpr unsigned int HEIGHT = 26;
	static constexpr int OFFSET = WIDTH * 5 + 2;
	char _mainScreen[WIDTH * HEIGHT];
	char _yourMap[10][10];
	char _opntMap[10][10]; 

	
	void clearScreen() const noexcept
	{
		#ifdef _WIN32
			std::system("cls");
		#else
			std::system ("clear");
		#endif
	}

	void hideYourMap() noexcept {
		for(int i = 0; i < 10; ++i) {
			for(int j = 0; j < 10; ++j) {
				draw(' ', i, j);
			}
		}
	}

	void draw(char c, int& i, int& j, int offset = 0) noexcept {
		_mainScreen[OFFSET + offset + (i * WIDTH * 2) + (j * 4)] = c;
	}

	void put(char c, int i, int j, bool onOpntMap) noexcept {
		if(onOpntMap) {
			_opntMap[i][j] = c;
		} else {
			_yourMap[i][j] = c;
		}
		draw(c, i, j, onOpntMap * 42);
	}

	void destroy(int& i, int& j, bool onOpntMap) noexcept {
		//char (&map)[10][10] = onOpntMap? _opntMap: _yourMap;
		char (*map)[10][10];
	   	if(onOpntMap) {
			map = &(this->_opntMap);
		} else {
			map = &(this->_yourMap);
		}

		// Up
		for(int k = i; (k > 0) && ((*map)[k][j] == '*'); --k) {
			if(j > 0) {
				put('X', k - 1, j - 1, onOpntMap);
			}
			if(j < 9) {
				put('X', k - 1, j + 1, onOpntMap);
			}
			if((*map)[k - 1][j] != '*') {
				put('X', k - 1, j, onOpntMap);
			}
		}
		
		// Down
		for(int k = i; (k < 9) && ((*map)[k][j] == '*'); ++k) {
			if(j > 0) {
				put('X', k + 1, j - 1, onOpntMap);
			}
			if(j < 9) {
				put('X', k + 1, j + 1, onOpntMap);
			}
			if((*map)[k + 1][j] != '*') {
				put('X', k + 1, j, onOpntMap);
			}
		}

		// Left
		for(int k = j; (k > 0) && ((*map)[i][k] == '*'); --k) {
			if(i > 0) {
				put('X', i - 1, k - 1, onOpntMap);
			}
			if(i < 9) {
				put('X', i + 1, k - 1, onOpntMap);
			}
			if((*map)[i][k - 1] != '*') {
				put('X', i, k - 1, onOpntMap);
			}
		}
		
		// Right
		for(int k = j; (k < 9) && ((*map)[i][k] == '*'); ++k) {
			if(i > 0) {
				put('X', i - 1, k + 1, onOpntMap);
			}
			if(i < 9) {
				put('X', i + 1, k + 1, onOpntMap);
			}
			if((*map)[i][k + 1] != '*') {
				put('X', i, k + 1, onOpntMap);
			}
		}
	}

	void hit(int& i, int& j) noexcept {
		std::cout << "Hitting coordinates: " << i << ' ' << j << std::endl;
		if(_yourMap[i][j] == '#') {
			std::cout << "Hit!" << std::endl;
			_yourMap[i][j] = '*';
			draw('#', i, j);
		} else {
			std::cout << "Miss!" << std::endl;
			_yourMap[i][j] = 'X';
			draw('X', i, j);
		}
	}

	bool isDestroyed(int& i, int& j) const noexcept {
		// Up
		for(int k = i; k > 0; --k) {
			if(_yourMap[k][j] == '#') {
				return false;
			} else if(_yourMap[k][j] != '*') {
				break;
			}
		}

		// Down
		for(int k = i; k < 10; ++k) {
			if(_yourMap[k][j] == '#') {
				return false;
			} else if(_yourMap[k][j] != '*') {
				break;
			}
		}
		
		// Left
		for(int k = j; k > 0; --k) {
			if(_yourMap[i][k] == '#') {
				return false;
			} else if(_yourMap[i][k] != '*') {
				break;
			}
		}

		// Right
		for(int k = j; k < 10; ++k) {
			if(_yourMap[i][k] == '#') {
				return false;
			} else if(_yourMap[i][k] != '*') {
				break;
			}
		}

		return true;
	}

	bool isAllFleetDestroyed() const noexcept {
		for(int i = 0; i < 10; ++i) {
			for(int j = 0; j < 10; ++j) {
				if(_yourMap[i][j] == '#') {
					return false;
				}
			}
		}

		return true;
	}

	static bool getVHInput() noexcept {
		std::cout << "Place [H]orisontal or [V]ertical? : "; 
		std::string s;
		while(true) {
			if(std::cin.peek() == EOF) {
				std::cout << "Got EOF" << std::endl;
				exit(0);
			}
			std::cin >> s;
			if(s[0] == 'v' || s[0] == 'V') {
				return true;
			} else if(s[0] == 'h' || s[0] == 'H') {
				return false;
			} else {
				std::cout << "Please, enter v or h : ";
			}
		}
	}

	static int getCoordinates(bool& isVertical, int& len) noexcept {
		int i, j;
		while(true) {
			if(std::cin.peek() == EOF) {
				std::cout << "Got EOF" << std::endl;
				exit(0);
			}
			try {
				std::string num1;
				std::cin >> num1;
				i = std::stoi(num1);
				std::string num2;
				std::cin >> num2;
				j = std::stoi(num2);
			} catch(std::exception& e) {
				std::cout << "Please, enter valid coordinates: ";
				continue;
			}

			if(isVertical) {
				if((i >= 0) && (i <= (10 - len)) && (j >= 0) && (j < 10)) { 
					break;
				}
			} else {
				if((j >= 0) && (j <= (10 - len)) && (i >= 0) && (i < 10)) { 
					break;
				}
			}
			std::cout << "Please, enter valid coordinates: ";
		}
		return i * 10 + j;	
	}

	bool insert(int i, int j, bool isVertical, int length) noexcept {
		// h_offset = 2
		// v_offset = 5
		for(int l = 0; l < length; ++l) {
			int iCoord = i + (isVertical * l);
			int jCoord = j + (!isVertical * l);
			
			// Check nearby
			if(iCoord > 0 && _yourMap[iCoord - 1][jCoord] == '#') {
				return 1;
			}
			if(iCoord < 9 && _yourMap[iCoord + 1][jCoord] == '#') {
				return 1;
			}
			if(jCoord > 0 && _yourMap[iCoord][jCoord - 1] == '#') {
				return 1;
			}
			if(jCoord < 9 && _yourMap[iCoord][jCoord + 1] == '#') {
				return 1;
			}
			if(iCoord > 0) {
				if((jCoord > 0 && _yourMap[iCoord - 1][jCoord - 1] == '#') ||
					(jCoord < 9 && _yourMap[iCoord - 1][jCoord + 1] == '#'))
			   	{
					return 1;	
				}
			}
			if(iCoord < 9) {
				if((jCoord > 0 && _yourMap[iCoord + 1][jCoord - 1] == '#') ||
					(jCoord < 9 && _yourMap[iCoord + 1][jCoord + 1] == '#'))
			   	{
					return 1;	
				}
			}
		}

		for(int l = 0; l < length; ++l) {
			int iCoord = i + (isVertical * l);
			int jCoord = j + (!isVertical * l);
			put('#', iCoord, jCoord, false);
		}
		return 0;
	}

public:
	SBGame() 
		: _mainScreen("\
+=======================================+=+=======================================+\n\
|              Your Map                 | |            Opponent's map             |\n\
>=======================================< >=======================================<\n\
| 0   1   2   3   4   5   6   7   8   9 | | 0   1   2   3   4   5   6   7   8   9 |\n\
|---v---v---v---v---v---v---v---v---v---|-|---v---v---v---v---v---v---v---v---v---|\n\
|   |   |   |   |   |   |   |   |   |   |0|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |1|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |2|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |3|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |4|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |5|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |6|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |7|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |8|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |9|   |   |   |   |   |   |   |   |   |   |\n\
|---^---^---^---^---^---^---^---^---^---|-|---^---^---^---^---^---^---^---^---^---|\n\
+=======================================+=+=======================================+\
"),
		_yourMap{
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
		},

		_opntMap{
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
			{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
		}
   	{}
	
	void print() const noexcept {
		clearScreen();
		std::cout << _mainScreen << std::endl;	
	}

	bool setupFleet() noexcept {
		std::cout << "Please, setup your fleet" << std::endl;
		int boats[4] = {1, 2, 3, 4};

		for(int len = 4; len > 0; --len) {
			while(boats[4 - len]) {
				std::cout << "Remaining boats: " <<
					"Boat4: " << boats[0] << ", Boat3: " << boats[1] <<
					", Boat2: " << boats[2] << ", Boat1: " << boats[3] <<
				   	std::endl;
				std::cout << "Placing Boat" << len << ':' << std::endl;
				bool isVertical = true;
			   	if(len != 1) {
					isVertical = getVHInput();
				}
				while(true) {
					std::cout << "Please, input coorinates [0-9] [0-9]: ";
					int coordinates = getCoordinates(isVertical, len);
					if(insert(
						coordinates / 10,
						coordinates % 10,
				   		isVertical,
				   		len
					)) 
					{
						std::cout << 
							"Boats should have distance of at least 1!" <<
						   	std::endl;	
						continue;
					}
					break;
				}
				--boats[4 - len];
				print();
			}
		}

		std::string ans;
		std::cout << "Fleet setup completed! Proceed? [y/n]: ";
		std::cin >> ans;
		if(ans.empty() || ans[0] == 'y' || ans[0] == 'Y') {
			hideYourMap();
			return 0;
		} else {
			return 1;
		}
	}

	void start() noexcept {
		print();
		while(setupFleet()) {}
		std::cout << "Status: Ready\nWaiting for Opponent..." << std::endl;
	}


	void checkReady(Socket* s) const noexcept {
		try {
			s->Write("Ready");
		} catch(std::exception& e) {
			std::cerr << e.what() << std::endl;
			exit(1);
		}

		std::string msg; 
		try {
			msg = s->Read(1024);
		} catch(std::exception& e) {
			std::cerr<< e.what() << std::endl;
			exit(1);
		}
		if(msg == "") {
			std::cout << "Client closed the connection" << std::endl;
			exit(0);
		}

		if(msg == "Ready") {
			print();
			std::cout << "And the game starts!" << std::endl;
		}
	}

	void startCommunication(Socket* s, const bool& YOU_ARE_HOST) noexcept {
		bool isMovingHost = true;
		while(true) {
			if(isMovingHost == YOU_ARE_HOST) {
				std::cout << "Your turn!" << std::endl;
				int i = -1, j = -1;

				while(!areValidCoordinates(i, j)) {
					std::cout << "Please, enter valid firing coordinates: ";
					if(std::cin.peek() == EOF) {
						std::cout << "Got EOF" << std::endl;
						exit(0);
					}
					try {
						std::string num1;
						std::cin >> num1;
						i = std::stoi(num1);
						std::string num2;
						std::cin >> num2;
						j = std::stoi(num2);
					} catch(std::exception& e) {
						std::cerr << e.what() << std::endl;
					}
				}

				std::cout << "Sending coordinates: " <<
					i << ' ' << j << std::endl;
				try {
					std::string msg = {char(i + '0'), char(j + '0')};
					s->Write(msg);
				} catch(std::exception& e) {
					std::cerr << e.what() << std::endl;
				}

				std::string hitStatus;
				try {
					hitStatus = s->Read(1024);
				} catch(std::exception& e) {
					std::cerr << e.what() << std::endl;
				}

				if(hitStatus == "") {
					std::cout << "Opponent closed the connection" << std::endl;
					exit(0);		
				}

				processStatus(hitStatus[0], i, j);
				print();
				std::cout << "Hitstatus: " << hitStatus << std::endl;
				if(hitStatus[0] == 'X') {
					isMovingHost ^= 1;
				}
			} else {
				std::cout << "Waiting for Opponent's message..." << std::endl;
				
				std::string coordinates;
				try {
					coordinates = s->Read(1024);
				} catch(std::exception& e) {
					std::cerr << e.what() << std::endl;
				}

				if(coordinates == "") {
					std::cout << "Opponent closed the connection" << std::endl;
					exit(0);		
				}

				std::cout << "Got coordinates: (Raw: " << coordinates <<
				   	"), Parsed: " << coordinates[0] << ' ' <<
				   	coordinates[1] << std::endl;
				int i = coordinates[0] - '0';
				int j = coordinates[1] - '0';

				std::string hitStatus;
				hitStatus.push_back(getHitStatus(i, j));
				print();

				try {
					s->Write(hitStatus);
				} catch(std::exception& e) {
					std::cerr << e.what() << std::endl;
				}

				if(hitStatus[0] == 'W') {
					std::cout << "You lost!" << std::endl;
					exit(0);
				} else if(hitStatus[0] == 'X') {
					isMovingHost ^= 1;
				}
			}
		}
	}


	bool areValidCoordinates(int& i, int& j) const noexcept {
		if((i < 0) || (i > 9) || (j < 0) || (j > 9)) {
			return false;
		}
		
		if(_opntMap[i][j] != ' ') {
			return false;
		}

		return true;
	}

	void processStatus(char& status, int& i, int& j) noexcept {
		if(status == 'W') {
			std::cout << "You won!" << std::endl;
			exit(0);
		}
		
		_opntMap[i][j] = '*';
		draw('#', i, j, 42);	
		if(status == 'D') {
			destroy(i, j, true);
		}
	}

	char getHitStatus(int& i, int& j) noexcept {
		hit(i, j);
		
		if(_yourMap[i][j] == 'X') {
			return 'X';
		}
		
		if(isDestroyed(i, j)) {
			destroy(i, j, false);
			if(isAllFleetDestroyed()) {
				return 'W';
			}
			return 'D';
		}

		return '#';
	}
};

#endif
