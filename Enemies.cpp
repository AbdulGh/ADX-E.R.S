#include"Enemies.h"
#include"CalculateProjectileAngle.h"
#include"GetXYRatio.h"
#include"Declarations.h"

std::vector <Enemy> EnemyVector;
std::vector <EnemyProjectile> EnemyProjectileVector;

Enemy::Enemy(int x = 0, int y = 0, int IType = 0)
{
	WorldX = x;
	WorldY = y;
	Type = IType;
	Frame = 0;
	Frametime = 0;
	ShotCounter = 0;
	if (Type == 1) //Suicide
	{
		Health = 100;
		CollisionRect.h = Suicide->h;
		CollisionRect.w = Suicide->w;
	}
	else if (Type == 2) //Gunman
	{
		Health = 150;
	}
}
void Enemy::Bleed(int ProjectileXVel, int ProjectileYVel)
{
	CreateDebris(3,3,WorldX,WorldY,ProjectileXVel,ProjectileYVel,0x808080);
}

void Enemy::Shoot(int TargetX, int TargetY, int Type)
{
	float XInc;
	float YInc;
	GetXYRatio(&XInc,&YInc,CalculateProjectileAngle(WorldX,WorldY,TargetX,TargetY),20);

	EnemyProjectile PushThis;
	PushThis.WorldX = WorldX;
	PushThis.WorldY = WorldY;
	PushThis.XVel = XInc;
	PushThis.YVel = YInc;
	PushThis.Type = Type;
	PushThis.CollisionRect.w = 3;
	PushThis.CollisionRect.h = 3;
	EnemyProjectileVector.push_back(PushThis);
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
void DoEnemies(int CameraX, int CameraY, float PlayerX, float PlayerY, SDL_Rect PlayerRect)
{
	for (int i = 0; i < EnemyVector.size(); i++)
	{
		if (CURRENTENEMY.Type != 0)
		{
			CURRENTENEMY.CollisionRect.x = CURRENTENEMY.WorldX - CameraX;
			CURRENTENEMY.CollisionRect.y = CURRENTENEMY.WorldY - CameraY;
			PlayerRect.x = PlayerX - CameraX;
			PlayerRect.y = PlayerY - CameraY;
			if (IsIntersecting(PlayerRect,CURRENTENEMY.CollisionRect))
			{
				if (CURRENTENEMY.Type == 1)
				{
					Damaged = true;
					DamageDealt = 20;
				}
			}
			for (int x = 0; x < ProjectileVector.size(); x++)
			{
				if (IsIntersecting(CURRENTENEMY.CollisionRect,ProjectileVector.at(x).ProjectileRect))
				{
					CURRENTENEMY.Bleed(ProjectileVector.at(x).XInc, ProjectileVector.at(x).YInc);
					CURRENTENEMY.Health -= 25;

					CURRENTENEMY.WorldX += ProjectileVector.at(x).XInc / 2;
					CURRENTENEMY.WorldX += ProjectileVector.at(x).YInc / 2;
					ProjectileVector.erase(ProjectileVector.begin() + x, ProjectileVector.begin() + x + 1);
				}
			}
		}
		
		else //Teleport
		{
			CURRENTENEMY.Frametime++;
			ApplySurface(CURRENTENEMY.WorldX - CameraX,CURRENTENEMY.WorldY - CameraY,TeleportSheet,Screen,&TeleportClips[Frame % 2]);
			if (CURRENTENEMY.Frametime > 15)
			{
				CURRENTENEMY.Frame++;
				CURRENTENEMY.Frametime = 0;
				if (CURRENTENEMY.Frame == 4) 
				{
					Enemy Temp;
					Temp.WorldX = CURRENTENEMY.WorldX;
					Temp.WorldY = CURRENTENEMY.WorldY;
					Temp.Frame = 0;
					Temp.Frametime = 0;
					Temp.Type = CURRENTENEMY.Health;
					Temp.Health = 100;
					EnemyVector.erase(EnemyVector.begin() + i ,EnemyVector.begin() + i + 1);
					EnemyVector.push_back(Temp);
					continue;
				}
			}
		}
		if (CURRENTENEMY.Type == 1) //Suicide
		{
			float XDiff;
			float YDiff;
			float Angle = CalculateProjectileAngle(CURRENTENEMY.WorldX, CURRENTENEMY.WorldY, PlayerX, PlayerY);
			GetXYRatio(&XDiff, &YDiff, Angle, 8);
			CURRENTENEMY.WorldX += XDiff;
			CURRENTENEMY.WorldY += YDiff;
			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY,Suicide, Screen);
			//__debugbreak();
		}
	if (CURRENTENEMY.Health < 0) EnemyVector.erase(EnemyVector.begin() + i, EnemyVector.begin() + i + 1);
	}
}

#define CURRENTPROJECTILE EnemyProjectileVector.at(i)

void DoEnemyProjectiles(int CameraX, int CameraY)
{
	for (int i = 0; i < EnemyProjectileVector.size(); i++)
	{
		CURRENTPROJECTILE.WorldX += CURRENTPROJECTILE.XVel;
		CURRENTPROJECTILE.WorldY += CURRENTPROJECTILE.YVel;
		ApplySurface(CURRENTPROJECTILE.WorldX - CameraX, CURRENTPROJECTILE.WorldY - CameraY,PlayerNormal,Screen);
	}
}