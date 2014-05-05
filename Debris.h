#include<SDL.h>
#include<SDL_main.h>
#include<vector>

class Debris
{
public:
	SDL_Rect Rect;
	float XVel;
	float YVel;
	Uint32 Colour;
};

void DoDebris(int CameraX, int CameraY, SDL_Surface *Screen);
void CreateDebris(int Size, int Number, float x, float y, int XVel, int YVel, Uint32 Colour);