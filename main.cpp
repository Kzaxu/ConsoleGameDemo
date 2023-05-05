#include "game_of_life.hpp"


int SDL_main(int, char*[])
{
	// Use olcConsoleGameEngine derived app
	GameOfLife game(GameOfLife::InitMethod::GosperGliderGun);
	game.ConstructConsole(200, 150, 4, 4);
	game.Start();
	return 0;
}