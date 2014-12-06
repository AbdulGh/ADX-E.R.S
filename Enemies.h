#ifndef ENEMIES_H
#define ENEMIES_H
#include<SDL.h>
#include<SDL_main.h>
#include"Debris.h"
#include<vector>

class Enemy
{
public:
	int WorldX;
	int WorldY;
	int Health;
	int Type;
	int Timer;
	int Frame;
	int Angle;
	int Frametime;
	int ShotCounter;
	int Speed;
	float XVel;
	float YVel;
	int MaxSpeed;

	bool Moving;

	SDL_Rect CollisionRect;

	Enemy(int,int,int);
	void Bleed(int ProjectileXVel, int ProjectileYVel);
	void Shoot (int Type, int TargetX, int TargetY);
	void Shoot (int Type, int Bearing);
	void Gib();
	void BulletPattern(int Type);
};

class Pickup
{
public: 
	int WorldX;
	int WorldY;
	int Type;
};

extern std::vector <Enemy> EnemyVector;
extern std::vector <Pickup> PickupVector;

void SpawnEnemies(std::vector <int> Enemus); //X Y Type
void DoEnemies(int CameraX, int CameraY, float PlayerX, float PlayerY, SDL_Rect PlayerRect, int XVel, int YVel);
void DoPickups(int CameraX, int CamerY, SDL_Rect PlayerRect);
#endif