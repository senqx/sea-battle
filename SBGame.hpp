#ifndef __S_B_GAME_
#define __S_B_GAME_

#include <iostream>
#include <string>

class SBGame {
	static constexpr unsigned int WIDTH = 84;
	static constexpr unsigned int HEIGHT = 26;
	char _mainScreen[WIDTH * HEIGHT];
	char _yourMap[10][10];
	char _opntMap[10][10]; 

	
	void clearScreen() const noexcept
	{
		#ifdef _WIN32
			std::system("cls");
		#else
			// Assume POSIX
			std::system ("clear");
		#endif
	}
	
	static bool getVHInput() noexcept {
		std::cout << "Place [H]orisontal or [V]ertical? : "; 
		std::string s;
		while(true) {
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

	static int getCoordinates(bool& isVertical) noexcept {
		int i, j;
		while(true) {
			try {
				std::cin >> i >> j;
			} catch(std::exception& e) {
				std::cout << "Please, enter valid coordinates: ";
				continue;
			}

			if(isVertical) {
				if(j >= 0 && j <= 6 && i >= 0 && i < 10) { 
					break;
				}
			} else {
				if(i >= 0 && i <= 6 && j >= 0 && j < 10) { 
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
		constexpr int offset = WIDTH * 5 + 2;
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
			_yourMap[iCoord][jCoord] = '#';
			_mainScreen[offset + (iCoord * WIDTH * 2) + (jCoord * 4)] = '#';
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

	void setUpFleet() noexcept {
		std::cout << "Please, setup your fleet" << std::endl;
		int boats[4] = {1, 2, 3, 4};

		for(int i = 4; i > 0; --i) {
			while(boats[4 - i]) {
				std::cout << "Remaining boats: " <<
					"Boat4: " << boats[0] << ", Boat3: " << boats[1] <<
					", Boat2: " << boats[2] << ", Boat1: " << boats[3] <<
				   	std::endl;
				std::cout << "Placing Boat" << i << ':' << std::endl;
				bool isVertical = getVHInput();
				while(true) {
					std::cout << "Please, input coorinates [0-9] [0-9]: ";
					int coordinates = getCoordinates(isVertical);
					if(insert(
						coordinates / 10,
						coordinates % 10,
				   		isVertical,
				   		i
					)) 
					{
						std::cout << 
							"Boats should have distance of at least 1!" <<
						   	std::endl;	
						continue;
					}
					break;
				}
				--boats[4 - i];
				print();
			}
		}
	}
};

#endif
