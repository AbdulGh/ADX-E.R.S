#ifndef DEBRIS_H
#define DEBRIS_H
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
	int Time;
};

extern std::vector<Debris> DebrisVector;

void DoDebris(int CameraX, int CameraY, SDL_Surface *Screen);
void CreateDebris(int Size, int Number, float x, float y, int XVel, int YVel, Uint32 Colour);
#endif