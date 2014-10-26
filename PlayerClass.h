#ifndef PLAYERCLASS_H
#define PLAYERCLASS_H
#include"Declarations.h"

class Player
{
public:
	int Health;
	int Lives;
	int InvunFrames;
	int TopSpeed;
	float XVel;
	float YVel;
	float WorldX;
	float WorldY;
	float Damage;
	float Speed;

	bool NormalDeath;
	bool NormalMovement;
	bool NormalCollision;
	bool Reset;
	bool Render;

	SDL_Surface *CurrentSprite;
	SDL_Rect PlayerRect;
	
	Player();
	void Update();
};

#endif