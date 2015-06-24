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
	int Spare;
	int Frametime;
	int ShotCounter;
	int Speed;
	int MaxSpeed;
	int BurnTimer;
	int Damage;
	int DamageFrames;
	int XOffset;
	int YOffset;
	float XVel;
	float YVel;

	bool Moving;
	bool OnFire;

	SDL_Rect CollisionRect;

	Enemy(int,int,int);
	void Bleed(int ProjectileXVel, int ProjectileYVel, int ex, int why);
	void Shoot (int Type, int TargetX, int TargetY);
	void Shoot (int Type, int Bearing);
	void Gib();
	void StayInLevel();
	void BulletPattern(int Type);
	bool IsNotActive() const;
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

extern std::string BossName;

extern SDL_Rect BossHealthRect;

extern int BossHealth;
extern double Multiplier;

void SpawnEnemies(std::vector <int> Enemus); //X Y Type
void DoEnemies(int CameraX, int CameraY, float PlayerX, float PlayerY, SDL_Rect PlayerRect, int XVel, int YVel);
void DoPickups(int CameraX, int CamerY, SDL_Rect PlayerRect);
#endif