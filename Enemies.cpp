#include"Enemies.h"
#include"CalculateProjectileAngle.h"
#include"GetXYRatio.h"
#include"Declarations.h"
#include<time.h>

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
	XVel = 0;
	YVel = 0;
}
void Enemy::Bleed(int ProjectileXVel, int ProjectileYVel)
{
	srand(time(NULL));
	for(int i = 0; i < 4; i++)
	{
		int Rand1 = rand() % 8 - 16;
		int Rand2 = rand() % 8 - 16;
		CreateDebris(3,1,WorldX + 15,WorldY + 15,ProjectileXVel + Rand1,ProjectileYVel + Rand2,0x808080);
	}
}

void Enemy::Shoot(int TargetX, int TargetY, int Type, int XVel, int YVel)
{
	float XInc = 0;
	float YInc = 0;
	TargetX += XVel * Speed;
	TargetY += YVel * Speed;
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
		Teleport.Speed = 0;
		EnemyVector.push_back(Teleport);
		Enemies++;
	}
}

#define CURRENTENEMY EnemyVector.at(i)
void DoEnemies(int CameraX, int CameraY, float PlayerX, float PlayerY, SDL_Rect PlayerRect, int XVel, int YVel)
{
	for (int i = 0; i < EnemyVector.size(); i++)
	{
		CURRENTENEMY.CollisionRect.x = CURRENTENEMY.WorldX - CameraX;
		CURRENTENEMY.CollisionRect.y = CURRENTENEMY.WorldY - CameraY;
		if (CURRENTENEMY.Type != 0)
		{
			if (IsIntersecting(PlayerRect,CURRENTENEMY.CollisionRect))
			{
				if (CURRENTENEMY.Type == 1 && Invincible == false)
				{
					Damaged = true;
					DamageDealt = 30;
					CreateDebris(5,5,CURRENTENEMY.WorldX,CURRENTENEMY.WorldY,CURRENTENEMY.XVel * 2,CURRENTENEMY.YVel * 2,0xFFFFFF);
					EnemyVector.erase(EnemyVector.begin() + i, EnemyVector.begin() + i + 1);
					Enemies--;
					continue;
				}
			}
			for (int x = 0; x < ProjectileVector.size(); x++)
			{
				ProjectileVector.at(x).ProjectileRect.x = ProjectileVector.at(x).WorldX - CameraX;
				ProjectileVector.at(x).ProjectileRect.y = ProjectileVector.at(x).WorldY - CameraY;
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
					if (Temp.Type == 1) //suicide
					{
						Temp.Health = 100;
						Temp.Speed = rand() % 4 + 6;
						Temp.CollisionRect.w = Suicide->w;
						Temp.CollisionRect.h = Suicide->h;
					}
					else if (Temp.Type == 2) //Gunman
					{
						Temp.Health = 150;
						Temp.ShotCounter = rand() % 90;
						Temp.Speed = rand() % 2 + 4;
						Temp.CollisionRect.w = Gunman->w;
						Temp.CollisionRect.h = Gunman->h;
					}
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
			float Angle = CalculateProjectileAngle(CURRENTENEMY.WorldX + XVel * (CURRENTENEMY.Speed / 4), CURRENTENEMY.WorldY + YVel * (CURRENTENEMY.Speed / 4), PlayerX, PlayerY);
			//if (CURRENTENEMY.Speed > 9) __debugbreak();
			GetXYRatio(&XDiff, &YDiff, Angle, CURRENTENEMY.Speed);
			CURRENTENEMY.XVel = XDiff;
			CURRENTENEMY.YVel = YDiff;
			CURRENTENEMY.WorldX += XDiff;
			CURRENTENEMY.WorldY += YDiff;
			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY,Suicide, Screen);
		}
		else if (CURRENTENEMY.Type == 2) //Gunman
		{
			float XDiff = 0;
			float YDiff = 0;
			int TempX = 0;
			int TempY = 0;
			int Distance = sqrt((CURRENTENEMY.WorldX - PlayerX) * (CURRENTENEMY.WorldX - PlayerX) + (CURRENTENEMY.WorldY - PlayerY) * (CURRENTENEMY.WorldY - PlayerY));
			if (Distance > 700 || Distance < 400)
			{
				float Angle = CalculateProjectileAngle(CURRENTENEMY.WorldX, CURRENTENEMY.WorldY, PlayerX, PlayerY);
				GetXYRatio(&XDiff, &YDiff, Angle, CURRENTENEMY.Speed);
				if (Distance < 600)
				{
					XDiff *= -2;
					YDiff *= -2;
				}
			}
			
			TempX = CURRENTENEMY.WorldX + XDiff;
			TempY = CURRENTENEMY.WorldY + YDiff;

			CURRENTENEMY.CollisionRect.x = TempX - CameraX;
			CURRENTENEMY.CollisionRect.y = TempY - CameraY;
			for (int x = 0; x < RectVector.size(); x++)
			{
				if (CURRENTENEMY.CollisionRect.x < 0) continue;
				if (CURRENTENEMY.CollisionRect.y < 0) continue;

				SDL_Rect Fu = CURRENTENEMY.CollisionRect;
				SDL_Rect ck = RectVector.at(x);
				if (IsIntersecting(Fu,ck))
				{
					XDiff = 0;
					YDiff = 0;
					break;
				}
			}
			CURRENTENEMY.WorldX += XDiff;
			CURRENTENEMY.WorldY += YDiff;
			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY,Gunman, Screen);

			CURRENTENEMY.ShotCounter++;
			if (CURRENTENEMY.ShotCounter == 90)
			{
				CURRENTENEMY.Shoot(PlayerX,PlayerY,1,XVel,YVel);
				CURRENTENEMY.ShotCounter = 0;
			}
		}
		if (CURRENTENEMY.Health < 0) 
		{
			EnemyVector.erase(EnemyVector.begin() + i, EnemyVector.begin() + i + 1);
			Enemies--;
		}
	}
}

#define CURRENTPROJECTILE EnemyProjectileVector.at(i)

void DoEnemyProjectiles(int CameraX, int CameraY, SDL_Rect PlayerRect)
{
	PlayerRect.x += CameraX;
	PlayerRect.y += CameraY;

	for (int i = 0; i < EnemyProjectileVector.size(); i++)
	{
		CURRENTPROJECTILE.WorldX += CURRENTPROJECTILE.XVel;
		CURRENTPROJECTILE.WorldY += CURRENTPROJECTILE.YVel;
		CURRENTPROJECTILE.CollisionRect.x = CURRENTPROJECTILE.WorldX;
		CURRENTPROJECTILE.CollisionRect.y = CURRENTPROJECTILE.WorldY;

		SDL_FillRect(Screen,&CURRENTPROJECTILE.CollisionRect,0xFFFF00);

		if (IsIntersecting(PlayerRect, CURRENTPROJECTILE.CollisionRect) && Damaged == false)
		{
			SDL_FillRect(Screen,&PlayerRect,0x0000FF);
			SDL_FillRect(Screen,&CURRENTPROJECTILE.CollisionRect,0xFF0000);
			SDL_Flip(Screen);
			SDL_Delay(2000);
			//__debugbreak();
			Damaged = true;
			DamageDealt = 10;
			EnemyProjectileVector.erase(EnemyProjectileVector.begin() + i, EnemyProjectileVector.begin() + i + 1);
			continue;
		}

		for (int y = 0; y < LevelVector.size(); y++)
		{
			SDL_Rect TileRect;
			TileRect.x = LevelVector.at(y).WorldX;
			TileRect.y = LevelVector.at(y).WorldY;
			TileRect.w = LevelVector.at(y).Width;
			TileRect.h = LevelVector.at(y).Height;
			if (IsIntersecting(CURRENTPROJECTILE.CollisionRect,TileRect))
			{
				CreateDebris(3,3,CURRENTPROJECTILE.WorldX,CURRENTPROJECTILE.WorldY,CURRENTPROJECTILE.XVel *-0.5,CURRENTPROJECTILE.YVel * -0.5, 0xFFFFFF);
				EnemyProjectileVector.erase(EnemyProjectileVector.begin() + i, EnemyProjectileVector.begin() + i + 1);
			}
		}
	}
}