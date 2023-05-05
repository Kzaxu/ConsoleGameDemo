#include "game_of_life.hpp"


void TestGameOfLife() {
	GameOfLife game(GameOfLife::InitMethod::GosperGliderGun);
	game.ConstructConsole(200, 150, 4, 4);
	game.Start();
}


int SDL_main(int, char*[])
{
	// Use olcConsoleGameEngine derived app
	TestGameOfLife();
	return 0;
}