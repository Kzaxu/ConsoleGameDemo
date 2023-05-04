#include "mazes.hpp"


int SDL_main(int, char*[])
{
	// Use olcConsoleGameEngine derived app
	MyGameMazes mazes(40, 20, 3);
	mazes.Start();
	return 0;
}