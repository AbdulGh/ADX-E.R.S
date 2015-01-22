#ifndef ENEMYPROJECTILE_H
#define ENEMYPROJECTILE_H

#include<SDL.h>
#include<SDL_main.h>
#include"Debris.h"
#include<vector>
#include"Enemies.h"

class EnemyProjectile
{
public:
	float WorldX;
	float WorldY;
	float XVel;
	float YVel;
	float Spare1;
	float Spare2;

	int Type;
	int Frame;
	int Frametime;
	int Damage;

	SDL_Rect CollisionRect;

	bool Active;
	bool Collides;
	bool RenderRect;
	bool Wave;
	bool Friction;
	bool SpawnsEnemies;
	bool Homing;
	bool Circle;

	EnemyProjectile(int Type);
};

extern std::vector <EnemyProjectile> EnemyProjectileVector;

void DoEnemyProjectiles(int CameraX, int CameraY, SDL_Rect PlayerRect);
void ClearProjectiles();

#endif