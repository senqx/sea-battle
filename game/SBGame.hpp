#ifndef __S_B_GAME_
#define __S_B_GAME_

#include "../Socket/Socket.hpp"

class SBGame {
	static constexpr unsigned int WIDTH = 84;
	static constexpr unsigned int HEIGHT = 26;
	static constexpr int OFFSET = WIDTH * 5 + 2;
	char _mainScreen[WIDTH * HEIGHT];
	char _yourMap[10][10];
	char _opntMap[10][10];

	// Put a boat vertically or horizontaly?
	static bool getVHInput() noexcept;

	// Top-left coordinates to but the boat
	static int getCoordinates(bool& isVertical, int& len) noexcept;

	/// Helping functions
	// Clear the screen (multiplatform)
	void clearScreen() const noexcept;

	// After setting up fleet, your boats' locations are completely
	// useless for you (because they are constant)
	// So just hide them. (To reduce cheating opportunities)
	void hideYourMap() noexcept;

	// Draw on screen map (Does not change actual maps)
	void draw(char c, int& i, int& j, int offset = 0) noexcept;

	// Draws on screen and CHANGES the map value
	void put(char c, int i, int j, bool onOpntMap) noexcept;

	// Put destroyed bot in "circle" (Mark area as missed)
	// Because all Boats must have a distance of at least 1
	void destroy(int& i, int& j, bool onOpntMap) noexcept;

	// Fire at (i, j) coordinates
	// CHANGES your map value to:
	// X if missed
	// * if hit
	void hit(int& i, int& j) noexcept;

	// Check if the last shot destroyed the boat
	bool isDestroyed(int& i, int& j) const noexcept;

	// Is called after boat destruction
	// Check if that one was your last :'(
	bool isAllFleetDestroyed() const noexcept;

	// Insert the boat into the map
	// Also draws in on screen
	bool insert(int i, int j, bool isVertical, int length) noexcept;

public:
	// Default - Creates empty maps
	SBGame();

	// Print the game-screen
	void print() const noexcept;

	// Get user input to setup boats
	bool setupFleet() noexcept;

	// Start the game
	void start() noexcept;

	// Check if all players ready
	void checkReady(Socket* s) const noexcept;

	// Start sending messages (coordinates)
	// And process result
	void startCommunication(Socket* s, const bool& YOU_ARE_HOST) noexcept;

	// Don't let player to fire outside the map
	// And also into an area where you already have fired
	bool areValidCoordinates(int& i, int& j) const noexcept;

	// Make actions based on the opponent answer
	void processStatus(char& status, int& i, int& j) noexcept;

	// Accept hit on your map
	// Return the result of the shot (Miss, Hit, Destroy)
	char getHitStatus(int& i, int& j) noexcept;
};

#endif
