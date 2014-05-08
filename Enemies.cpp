#include"Enemies.h"
#include"CalculateProjectileAngle.h"
#include"GetXYRatio.h"
#include"Declarations.h"

std::vector <Enemy> EnemyVector;
std::vector <EnemyProjectile> EnemyProjectileVector;

void Enemy::Bleed(int ProjectileXVel, int ProjectileYVel)
{
	CreateDebris(3,3,WorldX,WorldY,ProjectileXVel,ProjectileYVel,0x808080);
}

void Enemy::Shoot(int TargetX, int TargetY, int Type)
{
	float XInc;
	float YInc;
	GetXYRatio(&XInc,&YInc,CalculateProjectileAngle(WorldX,WorldY,TargetX,TargetY),5);

	EnemyProjectile PushThis;
	PushThis.WorldX = WorldX;
	PushThis.WorldY = WorldY;
	PushThis.XVel = XInc;
	PushThis.YVel = YInc;
	PushThis.Type = Type;
}

void SpawnEnemies(std::vector <int> Enemus) //X Y Type
{
	Enemy Teleport;
	for (int i = 0; i < Enemus.size(); i+=3)
	{
		Teleport.WorldX = Enemus.at(i);
		Teleport.WorldY = Enemus.at(i + 1);
		Teleport.Health = Enemus.at(i + 2);

		Teleport.Frame = 0;
		Teleport.Frametime = 0;
		Teleport.Type = 0;
		EnemyVector.push_back(Teleport);
	}
}

#define CURRENTENEMY EnemyVector.at(i)
void DoEnemies(int CameraX, int CameraY, SDL_Surface *Screen)
{
	for (int i = 0; i < EnemyVector.size(); i++)
	{
		if (CURRENTENEMY.Type == 0)
		{
			CURRENTENEMY.Frametime++;
			ApplySurface(CURRENTENEMY.WorldX - CameraX,CURRENTENEMY.WorldY - CameraY,TeleportSheet,Screen,&TeleportClips[Frame % 2]);
			if (CURRENTENEMY.Frametime > 15)
			{
				CURRENTENEMY.Frame++;
				CURRENTENEMY.Frametime = 0;
				if (CURRENTENEMY.Frame == 4) {EnemyVector.erase(EnemyVector.begin() + i ,EnemyVector.begin() + i + 1); continue;}
			}
		}
	}
}