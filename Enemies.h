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

	SDL_Rect CollisionRect;

	Enemy(int,int,int);
	void Bleed(int ProjectileXVel, int ProjectileYVel);
	void Shoot (int TargetX, int TargetY, int Type, int XVel, int YVel);
};

class EnemyProjectile
{
public:
	float WorldX;
	float WorldY;
	int Type;
	SDL_Rect CollisionRect;
	int XVel;
	int YVel;
	int Frame;
	int Frametime;
	bool Active;
};

class Pickup
{
public: 
	int WorldX;
	int WorldY;
	int Type;
};

extern std::vector <Enemy> EnemyVector;
extern std::vector <EnemyProjectile> EnemyProjectileVector;
extern std::vector <Pickup> PickupVector;

void SpawnEnemies(std::vector <int> Enemus); //X Y Type
void DoEnemies(int CameraX, int CameraY, float PlayerX, float PlayerY, SDL_Rect PlayerRect, int XVel, int YVel);
void DoEnemyProjectiles(int CameraX, int CameraY,SDL_Rect CharacterRect);
void DoPickups(int CameraX, int CamerY, SDL_Rect PlayerRect);
#endif