#include "SBGame.hpp"

#include <exception>
#include <iostream>
#include <string>
#include "../network/TerminalInput.hpp"

namespace {
void readCoordinates(int& row, int& column) {
    std::string a, b;
    if (!TerminalInput::next(a) || !TerminalInput::next(b)) std::exit(0);
    if (a.size() != 1 || !((a[0] >= 'A' && a[0] <= 'J') || (a[0] >= 'a' && a[0] <= 'j')))
        throw std::invalid_argument("Invalid row");
    row = std::toupper(static_cast<unsigned char>(a[0])) - 'A';
    if (b.size() != 1 || b[0] < '0' || b[0] > '9')
        throw std::invalid_argument("Invalid column");
    column = b[0] - '0';
}
}


bool SBGame::getVHInput() {
	std::cout << "Place [H]orisontal or [V]ertical? : ";
	std::string s;
	while(true) {
		if (!TerminalInput::next(s)) std::exit(0);
		if(s == "v" || s == "V") {
			return true;
		} else if(s == "h" || s == "H") {
			return false;
		} else {
			std::cout << "Please, enter v or h : ";
		}
	}
}

int SBGame::getCoordinates(bool& isVertical, int& len) {
	int i, j;
	while(true) {
		try {
			readCoordinates(i, j);
		} catch(const std::invalid_argument&) {
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

void SBGame::clearScreen() const
{
    if (::isatty(STDOUT_FILENO)) std::cout << "\033[2J\033[H";
}

void SBGame::hideYourMap() {
	for(int i = 0; i < 10; ++i) {
		for(int j = 0; j < 10; ++j) {
			draw(' ', i, j);
		}
	}
}

void SBGame::draw(char c, int& i, int& j, int offset) {
	_mainScreen[OFFSET + offset + (i * WIDTH * 2) + (j * 4)] = c;
}

void SBGame::put(char c, int i, int j, bool onOpntMap) {
	if(onOpntMap) {
		_opntMap[i][j] = c;
	} else {
		_yourMap[i][j] = c;
	}
	draw(c, i, j, onOpntMap * 42);
}

void SBGame::destroy(int& i, int& j, bool onOpntMap) {
	char (&map)[10][10] = onOpntMap? _opntMap: _yourMap;

	// Up
	for(int k = i; (k > 0) && (map[k][j] == '*'); --k) {
		if(j > 0) {
			put('X', k - 1, j - 1, onOpntMap);
		}
		if(j < 9) {
			put('X', k - 1, j + 1, onOpntMap);
		}
		if(map[k - 1][j] != '*') {
			put('X', k - 1, j, onOpntMap);
		}
	}

	// Down
	for(int k = i; (k < 9) && (map[k][j] == '*'); ++k) {
		if(j > 0) {
			put('X', k + 1, j - 1, onOpntMap);
		}
		if(j < 9) {
			put('X', k + 1, j + 1, onOpntMap);
		}
		if(map[k + 1][j] != '*') {
			put('X', k + 1, j, onOpntMap);
		}
	}

	// Left
	for(int k = j; (k > 0) && (map[i][k] == '*'); --k) {
		if(i > 0) {
			put('X', i - 1, k - 1, onOpntMap);
		}
		if(i < 9) {
			put('X', i + 1, k - 1, onOpntMap);
		}
		if(map[i][k - 1] != '*') {
			put('X', i, k - 1, onOpntMap);
		}
	}

	// Right
	for(int k = j; (k < 9) && (map[i][k] == '*'); ++k) {
		if(i > 0) {
			put('X', i - 1, k + 1, onOpntMap);
		}
		if(i < 9) {
			put('X', i + 1, k + 1, onOpntMap);
		}
		if(map[i][k + 1] != '*') {
			put('X', i, k + 1, onOpntMap);
		}
	}
}

void SBGame::hit(int& i, int& j) {
	std::cout << "Hitting coordinates: " << char('A' + i) << ' ' << j << std::endl;
	if(_yourMap[i][j] == '#') {
		std::cout << "Hit!" << std::endl;
		_yourMap[i][j] = '*';
		draw('*', i, j);
	} else {
		std::cout << "Miss!" << std::endl;
		_yourMap[i][j] = 'X';
		draw('X', i, j);
	}
}

bool SBGame::isDestroyed(int& i, int& j) const {
	// Up
	for(int k = i; k >= 0; --k) {
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
	for(int k = j; k >= 0; --k) {
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

bool SBGame::isAllFleetDestroyed() const {
	for(int i = 0; i < 10; ++i) {
		for(int j = 0; j < 10; ++j) {
			if(_yourMap[i][j] == '#') {
				return false;
			}
		}
	}

	return true;
}

bool SBGame::insert(int i, int j, bool isVertical, int length) {
	// h_offset = 2
	// v_offset = 5
	for(int l = 0; l < length; ++l) {
		int iCoord = i + (isVertical * l);
		int jCoord = j + (!isVertical * l);

		// Check the cell itself as well as neighboring cells.
		if (_yourMap[iCoord][jCoord] == '#') return true;

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
SBGame::SBGame()
	: _mainScreen("\
+=======================================+=+=======================================+\n\
|              Your Map                 | |            Opponent's map             |\n\
>=======================================< >=======================================<\n\
| 0   1   2   3   4   5   6   7   8   9 | | 0   1   2   3   4   5   6   7   8   9 |\n\
|---v---v---v---v---v---v---v---v---v---|-|---v---v---v---v---v---v---v---v---v---|\n\
|   |   |   |   |   |   |   |   |   |   |A|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |B|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |C|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |D|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |E|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |F|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |G|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |H|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |I|   |   |   |   |   |   |   |   |   |   |\n\
|---+---+---+---+---+---+---+---+---+---|-|---+---+---+---+---+---+---+---+---+---|\n\
|   |   |   |   |   |   |   |   |   |   |J|   |   |   |   |   |   |   |   |   |   |\n\
|---^---^---^---^---^---^---^---^---^---|-|---^---^---^---^---^---^---^---^---^---|\n\
+=======================================+=+=======================================+\
")
  , _yourMap{
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
  , _opntMap{
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

void SBGame::print() const {
	clearScreen();
	std::cout << _mainScreen << "\nX miss / cleared water   # hit   D sunk (opponent)   * hit (your fleet)\n"
              << "Coordinates: row A-J, column 0-9 (example: A 3). A hit keeps your turn.\n" << std::endl;
}

bool SBGame::setupFleet() {
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
				std::cout << "Please, input coordinates [A-J] [0-9]: ";
				int coordinates = getCoordinates(isVertical, len);
				if(insert(
					coordinates / 10,
					coordinates % 10,
					isVertical,
					len
				))
				{
					std::cout << "Boats should have distance of at least 1!" << std::endl;
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
	if (!TerminalInput::next(ans)) std::exit(0);
	if(ans.empty() || ans[0] == 'y' || ans[0] == 'Y') {
		hideYourMap();
		return 0;
	} else {
		return 1;
	}
}

void SBGame::start() {
	print();
	while(setupFleet()) {
        for (auto& row : _yourMap) for (char& cell : row) cell = ' ';
        hideYourMap();
        print();
    }
	std::cout << "Status: Ready\nWaiting for Opponent..." << std::endl;
}


void SBGame::checkReady(Socket* s) const {
	try {
		s->Write("Ready");
	} catch(std::exception& e) {
		std::cerr << e.what() << std::endl;
		exit(1);
	}

	std::string msg;
	try {
		msg = s->Read(5);
	} catch(std::exception& e) {
		std::cerr<< e.what() << std::endl;
		exit(1);
	}
	if(msg == "") {
		std::cout << "Opponent disconnected. Game ended." << std::endl;
		exit(0);
	}

	if (msg != "Ready") {
        std::cerr << "Invalid readiness message" << std::endl;
        std::exit(1);
    }
	if(msg == "Ready") {
		print();
		std::cout << "And the game starts!" << std::endl;
	}
}

void SBGame::startCommunication(Socket* s, const bool& YOU_ARE_HOST) {
	bool isMovingHost = true;
	while(true) {
		if(isMovingHost == YOU_ARE_HOST) {
			std::cout << "Your turn!" << std::endl;
			int i = -1, j = -1;

			while(!areValidCoordinates(i, j)) {
				std::cout << "Please, enter valid firing coordinates: ";
				try {
					readCoordinates(i, j);
				} catch(const std::invalid_argument&) {
					i = j = -1;
                    std::cerr << "Please enter a row A-J and column 0-9, for example A 3." << std::endl;
				}
			}

			std::cout << "Sending coordinates: " <<
				i << ' ' << j << std::endl;
			try {
				std::string msg = {char(i + '0'), char(j + '0')};
				s->Write(msg);
			} catch(std::exception& e) {
				std::cerr << e.what() << std::endl;
                std::exit(1);
			}

			std::string hitStatus;
			try {
				hitStatus = s->Read(1);
			} catch(std::exception& e) {
				std::cerr << e.what() << std::endl;
                std::exit(1);
			}

			if(hitStatus == "") {
				std::cout << "Opponent disconnected. Game ended." << std::endl;
				exit(0);
			}

			if (hitStatus.find_first_not_of("X#DW") != std::string::npos) {
                std::cerr << "Invalid shot response" << std::endl;
                std::exit(1);
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
				coordinates = s->Read(2);
			} catch(std::exception& e) {
				std::cerr << e.what() << std::endl;
                std::exit(1);
			}

			if(coordinates == "") {
				std::cout << "Opponent disconnected. Game ended." << std::endl;
				exit(0);
			}

			std::cout << "Got coordinates: (Raw: " << coordinates <<
				"), Parsed: " << coordinates[0] << ' ' <<
				coordinates[1] << std::endl;
			int i = coordinates[0] - '0';
			int j = coordinates[1] - '0';

			if (i < 0 || i > 9 || j < 0 || j > 9 ||
                (_yourMap[i][j] != ' ' && _yourMap[i][j] != '#')) {
                std::cerr << "Invalid or repeated opponent coordinates" << std::endl;
                std::exit(1);
            }
            std::string hitStatus;
			hitStatus.push_back(getHitStatus(i, j));
			print();

			try {
				s->Write(hitStatus);
			} catch(std::exception& e) {
				std::cerr << e.what() << std::endl;
                std::exit(1);
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


bool SBGame::areValidCoordinates(int& i, int& j) const {
	if((i < 0) || (i > 9) || (j < 0) || (j > 9)) {
		return false;
	}

	if(_opntMap[i][j] != ' ') {
		return false;
	}

	return true;
}

void SBGame::processStatus(char& status, int& i, int& j) {
	if(status == 'W') {
		_opntMap[i][j] = '*';
        destroy(i, j, true);
        draw('D', i, j, 42);
        print();
        std::cout << "You won!" << std::endl;
		exit(0);
	}

	if(status == '#') {
		_opntMap[i][j] = '*';
	} else if(status == 'D') {
		_opntMap[i][j] = '*';
		destroy(i, j, true);
	} else {
		_opntMap[i][j] = status;
	}
	draw(status, i, j, 42);
}

char SBGame::getHitStatus(int& i, int& j) {
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

