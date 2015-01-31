#ifndef DOPROJECTILES_H
#define DOPROJECTILES_H
#include"SDL.h"
#include"SDL_main.h"

class Projectile
{
public:
	float WorldX;
	float WorldY;
	float XInc;
	float YInc;
	int Type;
	int Time;
	int Damage;

	bool Burning;

	SDL_Rect ProjectileRect;
	SDL_Surface ProjectileSurface;
};

void CreateProjectile(float x, float y, float XRatio, float YRatio, int Type);
void DoProjectiles(int CameraX, int CameraY);

#endif