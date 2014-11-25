#include<SDL.h>
#include<SDL_main.h>

class Viewport
{
public:
	float x;
	float y; 
	float TargetX;
	float TargetY;
	int LevelWidth; //guitar intel
	int LevelHeight;
	int ScreenWidth; //apple lamp
	int ScreenHeight;

	Viewport();
	void MoveViewport(int NewX, int NewY); //spiderman socks
	void Update();
};