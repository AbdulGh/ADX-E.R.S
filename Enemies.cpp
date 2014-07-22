#include"Enemies.h"
#include"CalculateProjectileAngle.h"
#include"GetXYRatio.h"
#include"Declarations.h"
#include<time.h>

std::vector <Pickup> PickupVector;
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
	TargetX += XVel * Speed * 3;
	TargetY += YVel * Speed * 3;
	if (Type == 1) GetXYRatio(&XInc,&YInc,CalculateProjectileAngle(WorldX,WorldY,TargetX,TargetY),10);

	EnemyProjectile PushThis;
	PushThis.WorldX = WorldX;
	PushThis.WorldY = WorldY;
	PushThis.XVel = XInc;
	PushThis.YVel = YInc;
	PushThis.Type = Type;
	PushThis.CollisionRect.w = 3;
	PushThis.CollisionRect.h = 3;
	PushThis.Active = true;
	EnemyProjectileVector.push_back(PushThis);
}

#define CURRENTPICKUPTYPE PickupVector.at(i).Type
#define CURRENTPICKUPX PickupVector.at(i).WorldX
#define CURRENTPICKUPY PickupVector.at(i).WorldY
void DoPickups(int CameraX, int CameraY, SDL_Rect PlayerRect)
{
	if (PickupVector.size() != 0)
	{
		SDL_Rect TempRect;
		PlayerRect.x += CameraX;
		PlayerRect.y += CameraY;
		for (int i = 0; i < PickupVector.size(); i++)
		{
			switch (CURRENTPICKUPTYPE)
			{
			case 1: //Shotgun
				ApplySurface(CURRENTPICKUPX - CameraX, CURRENTPICKUPY - CameraY, Shotgun, Screen);
				TempRect.x = CURRENTPICKUPX;
				TempRect.y = CURRENTPICKUPY;
				TempRect.w = 13;
				TempRect.h = 13;
				if (IsIntersecting(TempRect,PlayerRect))
				{
					ShotgunAmmo += 10;
					PickupVector.erase(PickupVector.begin() + i);
				}
				break;
			}
		}
	}
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
			for (int x = 0; x < ProjectileVector.size(); x++)
			{
				ProjectileVector.at(x).ProjectileRect.x = ProjectileVector.at(x).WorldX - CameraX;
				ProjectileVector.at(x).ProjectileRect.y = ProjectileVector.at(x).WorldY - CameraY;
				if (IsIntersecting(CURRENTENEMY.CollisionRect,ProjectileVector.at(x).ProjectileRect))
				{
					CURRENTENEMY.Bleed(ProjectileVector.at(x).XInc, ProjectileVector.at(x).YInc);
					CURRENTENEMY.Health -= 30;

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
						Temp.Health = 140;
						Temp.Speed = rand() % 4 + 6;
						Temp.CollisionRect.w = Suicide->w;
						Temp.CollisionRect.h = Suicide->h;
					}
					else if (Temp.Type == 2) //Gunman
					{
						Temp.Health = 100;
						Temp.ShotCounter = rand() % 90;
						Temp.Speed = rand() % 2 + 4;
						Temp.CollisionRect.w = Gunman->w;
						Temp.CollisionRect.h = Gunman->h;
					}
					else if (Temp.Type == 3) //Beheaded Serious Sam guy
					{
						Temp.Health = 120;
						Temp.Speed = rand() % 4 + 10;
						Temp.CollisionRect.w = Serious->w;
						Temp.CollisionRect.h = Serious->h;
					}
					EnemyVector.erase(EnemyVector.begin() + i ,EnemyVector.begin() + i + 1);
					EnemyVector.push_back(Temp);
					continue;
				}
			}
		}
		if (CURRENTENEMY.Type == 1) //Suicide
		{
			if (IsIntersecting(PlayerRect,CURRENTENEMY.CollisionRect))
			{
				if (Invincible == false)
				{
					Damaged = true;
					DamageDealt = 30;
					CreateDebris(5,5,CURRENTENEMY.WorldX,CURRENTENEMY.WorldY,CURRENTENEMY.XVel * 2,CURRENTENEMY.YVel * 2,0xFFFFFF);
					EnemyVector.erase(EnemyVector.begin() + i, EnemyVector.begin() + i + 1);
					Enemies--;
					continue;
				}
			}
			
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
			if (Distance > 400 || Distance < 300)
			{
				float Angle = CalculateProjectileAngle(CURRENTENEMY.WorldX, CURRENTENEMY.WorldY, PlayerX, PlayerY);
				GetXYRatio(&XDiff, &YDiff, Angle, CURRENTENEMY.Speed);
				if (Distance < 300)
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
		else if (CURRENTENEMY.Type == 3) //Serious sam beheaded kamakazie
		{
			float XDiff;
			float YDiff;
			float Angle = CalculateProjectileAngle(CURRENTENEMY.WorldX + XVel * (CURRENTENEMY.Speed / 4), CURRENTENEMY.WorldY + YVel * (CURRENTENEMY.Speed / 4), PlayerX, PlayerY);
			GetXYRatio(&XDiff, &YDiff, Angle, CURRENTENEMY.Speed);
			CURRENTENEMY.XVel = XDiff;
			CURRENTENEMY.YVel = YDiff;
			CURRENTENEMY.WorldX += XDiff;
			CURRENTENEMY.WorldY += YDiff;
			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY,Serious, Screen);

			int Distance = sqrt((CURRENTENEMY.WorldX - PlayerX) * (CURRENTENEMY.WorldX - PlayerX) + (CURRENTENEMY.WorldY - PlayerY) * (CURRENTENEMY.WorldY - PlayerY));
			if (Distance < 100)
			{
				EnemyProjectile PushThis;
				PushThis.WorldX = CURRENTENEMY.WorldX;
				PushThis.WorldY = CURRENTENEMY.WorldY;
				PushThis.Type = 1;
				PushThis.CollisionRect.w = 3;
				PushThis.CollisionRect.h = 3;
				PushThis.Active = true;

				for (int i = 0; i < 360; i += 45)
				{
					float ex = 0;
					float why = 0;
					GetXYRatio(&ex,&why,i,5);
					
					PushThis.XVel = ex;
					PushThis.YVel = why;
					EnemyProjectileVector.push_back(PushThis);
				}
				CURRENTENEMY.Health = 0;
			}
		}

		if (CURRENTENEMY.Health <= 0) 
		{
			int tni = rand() % 100 + 1;
			if (tni < 20)
			{
				Pickup pukciP;
				pukciP.WorldX = CURRENTENEMY.WorldX;
				pukciP.WorldY = CURRENTENEMY.WorldY;
				pukciP.Type = 1;
				PickupVector.push_back(pukciP);
			}
			CreateDebris(5,6,CURRENTENEMY.WorldX,CURRENTENEMY.WorldY,CURRENTENEMY.XVel * 4,CURRENTENEMY.YVel * 4,0xFFFFFF);
			EnemyVector.erase(EnemyVector.begin() + i, EnemyVector.begin() + i + 1);
			Enemies--;
		}
	}
}

#define CURRENTENEMYPROJECTILE EnemyProjectileVector.at(wilk)

void DoEnemyProjectiles(int CameraX, int CameraY, SDL_Rect PlayerRect)
{
	PlayerRect.x += CameraX;
	PlayerRect.y += CameraY;

	for (int wilk = 0; wilk < EnemyProjectileVector.size(); wilk++)
	{
		CURRENTENEMYPROJECTILE.WorldX += CURRENTENEMYPROJECTILE.XVel;
		CURRENTENEMYPROJECTILE.WorldY += CURRENTENEMYPROJECTILE.YVel;
		CURRENTENEMYPROJECTILE.CollisionRect.x = CURRENTENEMYPROJECTILE.WorldX;
		CURRENTENEMYPROJECTILE.CollisionRect.y = CURRENTENEMYPROJECTILE.WorldY;

		if (IsIntersecting(PlayerRect, CURRENTENEMYPROJECTILE.CollisionRect) && Damaged == false)
		{
			Damaged = true;
			DamageDealt = 10;
			CURRENTENEMYPROJECTILE.Active = false;
		}

		for (int may = 0; may < LevelVector.size() && CURRENTENEMYPROJECTILE.Active == true; may++)
		{
			SDL_Rect TileRect;
			TileRect.x = LevelVector.at(may).WorldX;
			TileRect.y = LevelVector.at(may).WorldY;
			TileRect.w = LevelVector.at(may).Width;
			TileRect.h = LevelVector.at(may).Height;
			if (IsIntersecting(CURRENTENEMYPROJECTILE.CollisionRect,TileRect))
			{
				CreateDebris(3,3,CURRENTENEMYPROJECTILE.WorldX,CURRENTENEMYPROJECTILE.WorldY,CURRENTENEMYPROJECTILE.XVel * -1 ,CURRENTENEMYPROJECTILE.YVel * -1 , 0xFFFFFF);
				CURRENTENEMYPROJECTILE.Active = false;
			}
		}
		if (CURRENTENEMYPROJECTILE.Active == false) EnemyProjectileVector.erase(EnemyProjectileVector.begin() + wilk, EnemyProjectileVector.begin() + wilk + 1);
		else
		{
			CURRENTENEMYPROJECTILE.CollisionRect.x -= CameraX;
			CURRENTENEMYPROJECTILE.CollisionRect.y -= CameraY;
			CURRENTENEMYPROJECTILE.CollisionRect.w = 3;
			CURRENTENEMYPROJECTILE.CollisionRect.h = 3;
			SDL_FillRect(Screen,&CURRENTENEMYPROJECTILE.CollisionRect,0xFF0000);
		}
	}
}