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
	int Frame;
	int Frametime;

	void Bleed(int ProjectileXVel, int ProjectileYVel);
	void Shoot (int TargetX, int TargetY, int Type);
};

class EnemyProjectile
{
public:
	int WorldX;
	int WorldY;
	int Type;
	SDL_Rect CollisionRect;
	int XVel;
	int YVel;
};

std::vector <Enemy> EnemyVector;
std::vector <EnemyProjectile> EnemyProjectileVector;

void SpawnEnemies(std::vector <int> Enemus); //X Y Type
void DoEnemies(int CameraX, int CameraY, SDL_Surface *Screen);