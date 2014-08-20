#include"Enemies.h"
#include"CalculateProjectileAngle.h"
#include"GetXYRatio.h"
#include"Declarations.h"
#include"FloatText.h"
#include<time.h>

//Number of sound effects
#define NUMBEROFDEATHS 5
#define NUMBEROFTAUNTS 10
#define NUMBEROFDAMAGE 9


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
	Timer = 0;
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
	EnemyProjectile PushThis;
	PushThis.Type = Type;
	PushThis.Active = true;
	PushThis.Frame = 0;
	PushThis.Frametime = 0;
	if (Type == 1)
	{
		float XInc = 0;
		float YInc = 0;
		TargetX += XVel * Speed * 3;
		TargetY += YVel * Speed * 3;
		GetXYRatio(&XInc,&YInc,CalculateProjectileAngle(WorldX,WorldY,TargetX,TargetY),10);

		PushThis.WorldX = WorldX;
		PushThis.WorldY = WorldY;
		PushThis.XVel = XInc;
		PushThis.YVel = YInc;
		PushThis.CollisionRect.w = 3;
		PushThis.CollisionRect.h = 3;
	}
	else if (Type == 2)
	{
		PushThis.WorldX = WorldX;
		PushThis.WorldY = WorldY;
		PushThis.CollisionRect.w = 30;
		PushThis.CollisionRect.h = 10;
	}
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
				TempRect.x = CURRENTPICKUPX;
				TempRect.y = CURRENTPICKUPY;
				TempRect.w = Shotgun->w;
				TempRect.h = Shotgun->h;
				ApplySurface(CURRENTPICKUPX - CameraX, CURRENTPICKUPY - CameraY, Shotgun, Screen);
				if (IsIntersecting(TempRect,PlayerRect))
				{
					Ammo[1] += 10;
					FloatSomeText(PlayerRect.x, PlayerRect.y - 20, "Shotgun +10", White);
					PickupVector.erase(PickupVector.begin() + i);
				}
				break;
			case 2: //MachineGun
				TempRect.x = CURRENTPICKUPX;
				TempRect.y = CURRENTPICKUPY;
				TempRect.w = MachineGun->w;
				TempRect.h = MachineGun->h;
				ApplySurface(CURRENTPICKUPX - CameraX, CURRENTPICKUPY - CameraY, MachineGun, Screen);
				if (IsIntersecting(TempRect,PlayerRect))
				{
					Ammo[2] += 50;
					FloatSomeText(PlayerRect.x, PlayerRect.y, "Machinegun +50", White);
					PickupVector.erase(PickupVector.begin() + i);
				}
				break;
			case 3: //Health
				TempRect.x = CURRENTPICKUPX;
				TempRect.y = CURRENTPICKUPY;
				TempRect.w = Health->w;
				TempRect.h = Health->h;
				ApplySurface(CURRENTPICKUPX - CameraX, CURRENTPICKUPY - CameraY, Health, Screen);
				if (IsIntersecting(TempRect,PlayerRect))
				{
					Damaged = true;
					DamageDealt = -10;
					FloatSomeText(PlayerRect.x, PlayerRect.y, "10", Green);
					PickupVector.erase(PickupVector.begin() + i);
				}
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
	if (EnemyVector.size() != 0 && rand() % 400 == 42 && Boss == false)
	{
		SpareStream.str("");
		SpareStream << "Resources/Sounds/Taunt" << (rand() % NUMBEROFTAUNTS) + 1 << ".wav";
		Mix_Chunk *PlayThis = Mix_LoadWAV(SpareStream.str().c_str());
		Mix_PlayChannel(-1,PlayThis,0);
	}

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
					CURRENTENEMY.WorldX += ProjectileVector.at(x).XInc / 2;
					CURRENTENEMY.WorldX += ProjectileVector.at(x).YInc / 2;

					if (rand() % 100 < 8)
					{
						SpareStream.str("");
						SpareStream << "Resources/Sounds/Damage" << (rand() % NUMBEROFDAMAGE) + 1 << ".wav";
						
						Mix_Chunk *PlayThis = Mix_LoadWAV(SpareStream.str().c_str());

						Mix_PlayChannel(-1,PlayThis,0);
					}

					switch(ProjectileVector.at(x).Type)
					{
				case 1:
					CURRENTENEMY.Health -= 30;
					FloatSomeText(CURRENTENEMY.WorldX + 3, CURRENTENEMY.WorldY - 5, "30", Red, 2);
					ProjectileVector.erase(ProjectileVector.begin() + x, ProjectileVector.begin() + x + 1);
					break;
				case 2:
					CURRENTENEMY.Health -= 40;
					FloatSomeText(CURRENTENEMY.WorldX + 3, CURRENTENEMY.WorldY - 5, "40", Red, 2);
					ProjectileVector.erase(ProjectileVector.begin() + x, ProjectileVector.begin() + x + 1);
					}
				}
			}
		}
		
		else //Teleport
		{
			CURRENTENEMY.Frametime++;
			ApplySurface(CURRENTENEMY.WorldX - CameraX,CURRENTENEMY.WorldY - CameraY,TeleportSheet,Screen,&TeleportClips[CURRENTENEMY.Frame % 2]);
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
						Temp.Health = 80;
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
						Temp.Speed = rand() % 4 + 4;
						Temp.CollisionRect.w = Serious->w;
						Temp.CollisionRect.h = Serious->h;
						Temp.ShotCounter = 60;
					}
					else if (Temp.Type == 4) //SIS Boss
					{
						Temp.Health = 1000;
						Temp.Speed = 5;
						Temp.CollisionRect.w = 50;
						Temp.CollisionRect.h = 20;
						Boss = true;
					}
					else if (Temp.Type == 5) //SIS Invader
					{
						Temp.Health = 100;
						Temp.Speed = 7;
						Temp.CollisionRect.w = 50;
						Temp.CollisionRect.h = 50;
					}
					else if (Temp.Type == 6) //SIS Worm
					{
						Temp.Health = 100;
						Temp.Speed = 4;
						Temp.CollisionRect.w = 50;
						Temp.CollisionRect.h = 50;
						Temp.ShotCounter = 0;
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
			GetXYRatio(&XDiff, &YDiff, Angle, CURRENTENEMY.Speed / 2);
			CURRENTENEMY.XVel += XDiff / 2;
			CURRENTENEMY.YVel += YDiff / 2;

			if (CURRENTENEMY.XVel > CURRENTENEMY.Speed) CURRENTENEMY.XVel = CURRENTENEMY.Speed;
			else if (CURRENTENEMY.XVel < CURRENTENEMY.Speed * -1) CURRENTENEMY.XVel = CURRENTENEMY.Speed * -1;
			if (CURRENTENEMY.YVel > CURRENTENEMY.Speed) CURRENTENEMY.YVel = CURRENTENEMY.Speed;
			else if (CURRENTENEMY.YVel < CURRENTENEMY.Speed * -1) CURRENTENEMY.YVel = CURRENTENEMY.Speed * -1;
			CURRENTENEMY.WorldX += CURRENTENEMY.XVel;
			CURRENTENEMY.WorldY += CURRENTENEMY.YVel;
			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY,Suicide, Screen);
		}
		else if (CURRENTENEMY.Type == 2) //Gunman
		{
			if (CURRENTENEMY.Timer > 0) CURRENTENEMY.Timer--;
			float XDiff = 0;
			float YDiff = 0;
			int TempX = 0;
			int TempY = 0;
			int Distance = sqrt((CURRENTENEMY.WorldX - PlayerX) * (CURRENTENEMY.WorldX - PlayerX) + (CURRENTENEMY.WorldY - PlayerY) * (CURRENTENEMY.WorldY - PlayerY));
			if ((Distance > 500 || Distance < 250) && CURRENTENEMY.Timer == 0)
			{
				CURRENTENEMY.Angle = 0;
				float Angle = CalculateProjectileAngle(CURRENTENEMY.WorldX, CURRENTENEMY.WorldY, PlayerX, PlayerY);
				GetXYRatio(&XDiff, &YDiff, Angle, CURRENTENEMY.Speed);
				if (Distance < 200)
				{
					XDiff *= -2;
					YDiff *= -2;
				}
			}
			else
			{
				if (CURRENTENEMY.Angle == 0 && CURRENTENEMY.Timer == 0) 
				{
					CURRENTENEMY.Angle = rand () % 359 + 1;
					CURRENTENEMY.Timer = 20;
				}
				GetXYRatio(&XDiff, &YDiff, CURRENTENEMY.Angle, CURRENTENEMY.Speed);
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
				if (IsIntersecting(Fu,ck)) //lol
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

		else if (CURRENTENEMY.Type == 4) //SIS Boss
		{
			CURRENTENEMY.ShotCounter++;
			if (CURRENTENEMY.ShotCounter == 90) {CURRENTENEMY.Shoot(0,0,2,0,0); CURRENTENEMY.ShotCounter = 60; }
			if (CURRENTENEMY.WorldY < PlayerY) 
			{
				if (CURRENTENEMY.YVel + 0.3 + CURRENTENEMY.WorldY > PlayerY) CURRENTENEMY.YVel = PlayerY - CURRENTENEMY.WorldY;
				else CURRENTENEMY.YVel += 0.3;
			}

			else if (CURRENTENEMY.WorldY> PlayerY)
			{
				if (CURRENTENEMY.YVel - 0.3 + CURRENTENEMY.WorldY < PlayerY) CURRENTENEMY.YVel = CURRENTENEMY.WorldY - PlayerY;
				else CURRENTENEMY.YVel -= 0.3;
			}

			if (CURRENTENEMY.WorldX < (PlayerX - 300)) 
			{
				if (CURRENTENEMY.XVel + 1 + CURRENTENEMY.WorldX > (PlayerX - 300)) CURRENTENEMY.XVel = 0;
				else CURRENTENEMY.XVel += 1;
			}

			if (CURRENTENEMY.WorldX > (PlayerX - 300)) 
			{
				if (CURRENTENEMY.XVel - 1 + CURRENTENEMY.WorldX < (PlayerX - 300)) CURRENTENEMY.XVel = 0;
				else CURRENTENEMY.XVel -= 1;
			}
			
			if (CURRENTENEMY.XVel > 5) CURRENTENEMY.XVel = 5;
			else if (CURRENTENEMY.XVel < -5) CURRENTENEMY.XVel = -5;
			if (CURRENTENEMY.YVel > 5) CURRENTENEMY.YVel = 5;
			else if (CURRENTENEMY.YVel < -5) CURRENTENEMY.YVel = -5;

			CURRENTENEMY.WorldX += CURRENTENEMY.XVel;
			if (CURRENTENEMY.WorldX < 20) CURRENTENEMY.WorldX = 20;
			CURRENTENEMY.WorldY += CURRENTENEMY.YVel;
			CURRENTENEMY.Frametime++;
			
			if (CURRENTENEMY.Frametime > 10)
			{
				CURRENTENEMY.Frame++;
				CURRENTENEMY.Frametime = 0;
				if (CURRENTENEMY.Frame > 2) CURRENTENEMY.Frame = 0;
			}
			SDL_Surface *ApplyThis = Ship;
			ApplyThis->w = 50;
			ApplyThis->h = 20;
			ApplySurface(0, 0, Ship, ApplyThis, &ShipFrames[CURRENTENEMY.Frame]);
			ApplyThis = rotozoomSurface(ApplyThis,CURRENTENEMY.YVel * 2,1,0);
			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, ApplyThis, Screen);
			SDL_Rect BossHeight;
			BossHeight.x = 200;
			BossHeight.y = 10;
			BossHeight.h = 20;
			BossHeight.w = CURRENTENEMY.Health / 4;

			SDL_FillRect(Screen, &BossHeight, 0x00FF00);

			Message = TTF_RenderText_Solid(SysSmall,"SIS: ",Green);
			ApplySurface(200 - (Message->w), 10, Message, Screen);
		}

		else if (CURRENTENEMY.Type == 5) //SIS Invader
		{
			CURRENTENEMY.WorldX -= CURRENTENEMY.Speed;
			CURRENTENEMY.CollisionRect.x = CURRENTENEMY.WorldX;

			if (IsIntersecting(CURRENTENEMY.CollisionRect,PlayerRect) && Invincible == false)
			{
				CURRENTENEMY.Health = 0;
				Damaged = true;
				DamageDealt = 40;
			}

			else if (CURRENTENEMY.WorldX < 0) CURRENTENEMY.Health = 0;

			CURRENTENEMY.Frametime++;
			
			if (CURRENTENEMY.Frametime > 30)
			{
				CURRENTENEMY.Frame = 1 - CURRENTENEMY.Frame;
				CURRENTENEMY.Frametime = 0;
			}

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, Invader, Screen, &InvaderFrames[CURRENTENEMY.Frame]);
		}

		else if (CURRENTENEMY.Type == 6) //SIS Worm
		{
			CURRENTENEMY.WorldX += CURRENTENEMY.Speed;
			CURRENTENEMY.CollisionRect.x = CURRENTENEMY.WorldX;

			if (IsIntersecting(CURRENTENEMY.CollisionRect,PlayerRect) && Invincible == false)
			{
				CURRENTENEMY.Health = 0;
				Damaged = true;
				DamageDealt = 30;
			}

			else if (CURRENTENEMY.WorldX > LevelWidth) CURRENTENEMY.Health = 0;

			CURRENTENEMY.Frametime++;
			
			if (CURRENTENEMY.Frametime > 30)
			{
				CURRENTENEMY.Frame = 1 - CURRENTENEMY.Frame;
				CURRENTENEMY.Frametime = 0;
			}

			CURRENTENEMY.ShotCounter++;
			if (CURRENTENEMY.ShotCounter > 60)
			{
				CURRENTENEMY.ShotCounter = 0;
				CURRENTENEMY.Shoot(0,0,2,0,0);
			}

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, Worm, Screen, &InvaderFrames[CURRENTENEMY.Frame]);
		}

		if (CURRENTENEMY.Health <= 0) 
		{
			if (CURRENTENEMY.Type != 5 && CURRENTENEMY.Type != 6)
			{

				int tni = rand() % 100 + 1; //tni = int backwards

				if (tni > 95)
				{
					Pickup pukciP;
					pukciP.WorldX = CURRENTENEMY.WorldX;
					pukciP.WorldY = CURRENTENEMY.WorldY;
					pukciP.Type = 3;
					PickupVector.push_back(pukciP);
				}

				if (tni <= 20)
				{
					SpareStream.str("");
					SpareStream << "Resources/Sounds/Death" << (tni % NUMBEROFDEATHS) + 1 << ".wav";
					Mix_Chunk *PlayThis = Mix_LoadWAV(SpareStream.str().c_str());
					Mix_PlayChannel(-1,PlayThis,0);
				}

				else if (tni < 45 &&CURRENTENEMY.Type == 1)
				{
					Pickup pukciP; //pukciP = Pickup backwards
					pukciP.WorldX = CURRENTENEMY.WorldX;
					pukciP.WorldY = CURRENTENEMY.WorldY;
					pukciP.Type = 1;
					PickupVector.push_back(pukciP);
				}

				else if (tni < 25 && CURRENTENEMY.Type == 2)
				{
					Pickup pukciP; //pukciP = Pickup backwards
					pukciP.WorldX = CURRENTENEMY.WorldX;
					pukciP.WorldY = CURRENTENEMY.WorldY;
					pukciP.Type = 2;
					PickupVector.push_back(pukciP);
				}
				CreateDebris(5,6,CURRENTENEMY.WorldX,CURRENTENEMY.WorldY,CURRENTENEMY.XVel * 4,CURRENTENEMY.YVel * 4,0xFFFFFF);

				if (CURRENTENEMY.Type == 3) //Explosion
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
				}
			}

			if (CURRENTENEMY.Type == 4) 
			{
				Boss = false;
				Temp1 = CURRENTENEMY.WorldX;
				Temp2 = CURRENTENEMY.WorldY;
			}

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
		CURRENTENEMYPROJECTILE.CollisionRect.x = CURRENTENEMYPROJECTILE.WorldX;
		CURRENTENEMYPROJECTILE.CollisionRect.y = CURRENTENEMYPROJECTILE.WorldY;
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

		if (CURRENTENEMYPROJECTILE.Active == true)
		{
			if (CURRENTENEMYPROJECTILE.Type == 1)
			{
				CURRENTENEMYPROJECTILE.WorldX += CURRENTENEMYPROJECTILE.XVel;
				CURRENTENEMYPROJECTILE.WorldY += CURRENTENEMYPROJECTILE.YVel;

				if (IsIntersecting(PlayerRect, CURRENTENEMYPROJECTILE.CollisionRect) && Damaged == false)
				{
					Damaged = true;
					DamageDealt = 10;
					CURRENTENEMYPROJECTILE.Active = false;
				}

				if (CURRENTENEMYPROJECTILE.Active)
				{
					CURRENTENEMYPROJECTILE.CollisionRect.x -= CameraX;
					CURRENTENEMYPROJECTILE.CollisionRect.y -= CameraY;
					CURRENTENEMYPROJECTILE.CollisionRect.w = 3;
					CURRENTENEMYPROJECTILE.CollisionRect.h = 3;
					SDL_FillRect(Screen,&CURRENTENEMYPROJECTILE.CollisionRect,0xFFFFFF);
				}
			}
			else if (CURRENTENEMYPROJECTILE.Type == 2)
			{
				if (IsIntersecting(PlayerRect, CURRENTENEMYPROJECTILE.CollisionRect) && Damaged == false)
				{
					Damaged = true;
					DamageDealt = 30;
					CURRENTENEMYPROJECTILE.Active = false;
				}

				CURRENTENEMYPROJECTILE.WorldX += 10;
				CURRENTENEMYPROJECTILE.Frametime++;

				ApplySurface(CURRENTENEMYPROJECTILE.WorldX - CameraX, CURRENTENEMYPROJECTILE.WorldY - CameraY, ShipProjectile, Screen);
			}
		}
		if (CURRENTENEMYPROJECTILE.Active == false) EnemyProjectileVector.erase(EnemyProjectileVector.begin() + wilk, EnemyProjectileVector.begin() + wilk + 1);
	}
}