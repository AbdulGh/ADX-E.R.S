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
	void Shoot (int TargetX, int TargetY);
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

void SpawnEnemies(std::vector <int> Enemus) //X Y Type
{
	for (int i = 0; i < Enemus.size(); i+=3)
	{
		Enemy Teleport;
		Teleport.Health = Enemus.at(i+ 2);
		Teleport.WorldX = Enemus.at(i);
		Teleport.WorldY = Enemus.at(i + 1);
		Teleport.Frame = 0;
		Teleport.Frametime = 0;
	}
}