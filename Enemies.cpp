#include"Enemies.h"
#include"CalculateProjectileAngle.h"
#include"GetXYRatio.h"
#include"Declarations.h"
#include"FloatText.h"
#include"DoEnemyProjectiles.h"
#include<time.h>

//Number of sound effects
#define NUMBEROFDEATHS 5
#define NUMBEROFTAUNTS 10
#define NUMBEROFDAMAGE 9

int BossStage = 0;
int BosNum1 = 0;
int Direction = 3;

float XDiff;
float YDiff;

SDL_Rect One;
SDL_Rect Two;
SDL_Rect Three;
SDL_Rect Four;
SDL_Rect Five;
SDL_Rect PlayerRectangle;

SDL_Rect *RenderRect = &TeleportClips[0];

std::vector <Pickup> PickupVector;
std::vector <Enemy> EnemyVector;

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
	Moving = true;
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
					Ammo[1] += 5;
					FloatSomeText(PlayerRect.x, PlayerRect.y - 20, "Shotgun +5", White);
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
					Ammo[2] += 30;
					FloatSomeText(PlayerRect.x, PlayerRect.y - 20, "Machinegun +30", White);
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
					FloatSomeText(PlayerRect.x, PlayerRect.y - 20, "10", Green);
					PickupVector.erase(PickupVector.begin() + i);
				}
				break;
			case 4: //FlameThrower
				TempRect.x = CURRENTPICKUPX;
				TempRect.y = CURRENTPICKUPY;
				TempRect.w = MachineGun->w;
				TempRect.h = MachineGun->h;
				ApplySurface(CURRENTPICKUPX - CameraX, CURRENTPICKUPY - CameraY, Flamethrower, Screen);
				if (IsIntersecting(TempRect,PlayerRect))
				{
					Ammo[3] += 150;
					FloatSomeText(PlayerRect.x, PlayerRect.y - 20, "Flamethrower +150", White);
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
	}
	Enemies = EnemyVector.size();
}

#define CURRENTENEMY EnemyVector.at(i)
void DoEnemies(int CameraX, int CameraY, float PlayerX, float PlayerY, SDL_Rect PlayerRect, int XVel, int YVel)
{
	if (EnemyVector.size() != 0 && rand() % 950 == 42 && Boss == false)
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
					
					if (CURRENTENEMY.Moving)
					{
					CURRENTENEMY.WorldX += ProjectileVector.at(x).Damage * (ProjectileVector.at(x).XInc / abs(ProjectileVector.at(x).XInc));
					CURRENTENEMY.WorldX += ProjectileVector.at(x).Damage * (ProjectileVector.at(x).YInc / abs(ProjectileVector.at(x).YInc)); //TODO fix this
					}

					if (rand() % 140 < 5)
					{
						SpareStream.str("");
						SpareStream << "Resources/Sounds/Damage" << (rand() % NUMBEROFDAMAGE) + 1 << ".wav";
						
						Mix_Chunk *PlayThis = Mix_LoadWAV(SpareStream.str().c_str());

						Mix_PlayChannel(-1,PlayThis,0);
					}

					CURRENTENEMY.Health -= ProjectileVector.at(x).Damage;
					SpareStream.str("");
					SpareStream << ProjectileVector.at(x).Damage;
					FloatSomeText(ProjectileVector.at(x).WorldX, ProjectileVector.at(x).WorldY, SpareStream.str().c_str(), Red, 2);
					ProjectileVector.erase(ProjectileVector.begin() + x, ProjectileVector.begin() + x + 1);

					/*switch(ProjectileVector.at(x).Type)
					{
				case 1:
					CURRENTENEMY.Health -= 30;
					FloatSomeText(ProjectileVector.at(x).WorldX, ProjectileVector.at(x).WorldY, "30", Red, 2);
					ProjectileVector.erase(ProjectileVector.begin() + x, ProjectileVector.begin() + x + 1);
					break;
				case 2:
					CURRENTENEMY.Health -= 40;
					FloatSomeText(ProjectileVector.at(x).WorldX, ProjectileVector.at(x).WorldY, "40", Red, 2);
					ProjectileVector.erase(ProjectileVector.begin() + x, ProjectileVector.begin() + x + 1);
					break;
				case 3:
					CURRENTENEMY.Health -= 3;
					FloatSomeText(ProjectileVector.at(x).WorldX, ProjectileVector.at(x).WorldY, "3", Red, 2);
					ProjectileVector.erase(ProjectileVector.begin() + x, ProjectileVector.begin() + x + 1);
					}*/
				}
			}
		}
		
		else //Teleport
		{
			if (CURRENTENEMY.Frametime < 15)
			{
			CURRENTENEMY.Frametime++;
			ApplySurface(CURRENTENEMY.WorldX - CameraX,CURRENTENEMY.WorldY - CameraY,TeleportSheet,Screen,&TeleportClips[CURRENTENEMY.Frametime % 2]);
			}

			else
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
					switch(Temp.Type)
					{
					case 1: //suicide
						Temp.Health = 80;
						Temp.Speed = rand() % 4 + 6;
						Temp.CollisionRect.w = Suicide->w;
						Temp.CollisionRect.h = Suicide->h;
					break;

					case 2: //Gunman
						Temp.Health = 100;
						Temp.ShotCounter = rand() % 60;
						Temp.Speed = rand() % 2 + 4;
						Temp.CollisionRect.w = Gunman->w;
						Temp.CollisionRect.h = Gunman->h;
					break;

					case 3: //Beheaded Serious Sam guy
						Temp.Health = 120;
						Temp.Speed = rand() % 5 + 5;
						Temp.CollisionRect.w = Serious->w;
						Temp.CollisionRect.h = Serious->h;
						Temp.ShotCounter = 60;
					break;

					case 4: //SIS Boss
						Temp.Health = 1850;
						Temp.Speed = 5;
						Temp.CollisionRect.w = 50;
						Temp.CollisionRect.h = 20;
						Boss = true;
					break;

					case 5: //SIS Invader
						Temp.Health = 100;
						Temp.Speed = 7;
						Temp.CollisionRect.w = 50;
						Temp.CollisionRect.h = 50;
					break;

					case 6: //SIS Worm
						Temp.Health = 100;
						Temp.Speed = 4;
						Temp.CollisionRect.w = 50;
						Temp.CollisionRect.h = 50;
						Temp.ShotCounter = 0;
						break;

					case 7: //DoorGuard
						Temp.Health = 2100;
						Temp.CollisionRect.w = 200;
						Temp.CollisionRect.h = 50;
						Temp.Moving = false;
						break;

					case 8: //Spawner
						Temp.Health = 150;
						Temp.Speed = 2;
						Temp.CollisionRect.w = 25;
						Temp.CollisionRect.h = 25;
						break;

					case 9: //MARS
						Temp.Health = 7000;
						Temp.Speed = 5;
						Temp.CollisionRect.w = 40;
						Temp.CollisionRect.h = 100;
						Boss = true;
						BossStage = 0;
						break;

					case 10: //Frog
						Temp.Health = 150;
						Temp.Speed = rand() % 5 + 10;
						Temp.ShotCounter = rand() % 60 - 30;
						Temp.CollisionRect.w = 25;
						Temp.CollisionRect.h = 25;
					};
					EnemyVector.erase(EnemyVector.begin() + i ,EnemyVector.begin() + i + 1);
					EnemyVector.push_back(Temp);
					continue;
				}
			}
		}

		if (Laser && CURRENTENEMY.Type != 0 && CURRENTENEMY.WorldY + CURRENTENEMY.CollisionRect.h > LaserY) 
		{
			CURRENTENEMY.Health = 0;
			//__debugbreak();
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
			if (CURRENTENEMY.ShotCounter == 60)
			{
				CURRENTENEMY.Shoot(1,PlayerX,PlayerY);
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
				EnemyProjectile PushThis(1);
				PushThis.WorldX = CURRENTENEMY.WorldX;
				PushThis.WorldY = CURRENTENEMY.WorldY;
				PushThis.CollisionRect.w = 3;
				PushThis.CollisionRect.h = 3;
				PushThis.Active = true;

				for (int May = 0; May < 360; May += 45)
				{
					CURRENTENEMY.Shoot(1,May);
				}
				CURRENTENEMY.Health = 0;
			}
		}

		else if (CURRENTENEMY.Type == 4) //SIS Boss
		{
			if (IsIntersecting(PlayerRect,CURRENTENEMY.CollisionRect))
			{
				Damaged = true;
				DamageDealt = 50;
			}
			
			CURRENTENEMY.ShotCounter++;
			if (CURRENTENEMY.ShotCounter == 90) {CURRENTENEMY.Shoot(2,0,0); CURRENTENEMY.ShotCounter = 60; }
			if (CURRENTENEMY.WorldY < PlayerY) 
			{
				if (CURRENTENEMY.YVel + 0.5 + CURRENTENEMY.WorldY > PlayerY) CURRENTENEMY.YVel = PlayerY - CURRENTENEMY.WorldY;
				else CURRENTENEMY.YVel += 0.5;
			}

			else if (CURRENTENEMY.WorldY> PlayerY)
			{
				if (CURRENTENEMY.YVel - 0.5 + CURRENTENEMY.WorldY < PlayerY) CURRENTENEMY.YVel = CURRENTENEMY.WorldY - PlayerY;
				else CURRENTENEMY.YVel -= 0.5;
			}

			if (CURRENTENEMY.WorldX < (PlayerX - 300)) 
			{
				if (CURRENTENEMY.XVel + 2.5 + CURRENTENEMY.WorldX > (PlayerX - 300)) CURRENTENEMY.XVel = 0;
				else CURRENTENEMY.XVel += 2.5;
			}

			if (CURRENTENEMY.WorldX > (PlayerX - 300)) 
			{
				if (CURRENTENEMY.XVel - 10 + CURRENTENEMY.WorldX < (PlayerX - 300)) CURRENTENEMY.XVel = 0;
				else CURRENTENEMY.XVel -= 10;
			}
			
			if (CURRENTENEMY.XVel > 5) CURRENTENEMY.XVel = 5;
			else if (CURRENTENEMY.XVel < -10) CURRENTENEMY.XVel = -10;
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
			BossHeight.x = 100;
			BossHeight.y = 10;
			BossHeight.h = 20;
			BossHeight.w = CURRENTENEMY.Health / 4;

			SDL_FillRect(Screen, &BossHeight, 0x00FF00);

			Message = TTF_RenderText_Solid(SysSmall,"SIS: ",Green);
			ApplySurface(100 - (Message->w), 10, Message, Screen);
		}

		else if (CURRENTENEMY.Type == 5) //SIS Invader
		{
			CURRENTENEMY.WorldX -= CURRENTENEMY.Speed;

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
				CURRENTENEMY.Shoot(2,0,0);
			}

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, Worm, Screen, &InvaderFrames[CURRENTENEMY.Frame]);
		}

		else if (CURRENTENEMY.Type == 7) //DoorGuard
		{
			if (IsIntersecting(CURRENTENEMY.CollisionRect,PlayerRect))
			{
				CURRENTENEMY.Health = 0;
				Invincible = false;
				Damaged = true;
				DamageDealt = 100;
			}
			
			CURRENTENEMY.ShotCounter++;

			if (CURRENTENEMY.ShotCounter % 30 == 0)
			{
				CURRENTENEMY.Shoot(1,PlayerX,PlayerY);
			}

			if (CURRENTENEMY.ShotCounter == 100)
			{
				for (int mem = 0; mem <= 180; mem += 20) 
				{
					CURRENTENEMY.Shoot(3, 90 + mem);
				}
			}

			else if (CURRENTENEMY.ShotCounter == 200)
			{
				for (int mem = 0; mem <= 180; mem += 45) CURRENTENEMY.Shoot(4,90 + mem);
			}

			else if (CURRENTENEMY.ShotCounter == 350)
			{
				for (int mem = 0; mem <= 180; mem += 30) CURRENTENEMY.Shoot(3, 90 + mem);
				for (int mem = 0; mem <= 180; mem += 60) CURRENTENEMY.Shoot(4, 90 + mem);
				for (int mem = 0; mem <= 180; mem += 15) CURRENTENEMY.Shoot(1, 90 + mem);
				CURRENTENEMY.ShotCounter = 0;
			}

			if (IsIntersecting(CURRENTENEMY.CollisionRect,PlayerRect))
			{
				Damaged = true;
				DamageDealt = 100;
			}

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, DoorGuard, Screen);
		}

		else if (CURRENTENEMY.Type == 8) //Spawner
		{
			float XDiff;
			float YDiff;
			float Angle = CalculateProjectileAngle(CURRENTENEMY.WorldX + XVel * (CURRENTENEMY.Speed / 4), CURRENTENEMY.WorldY + YVel * (CURRENTENEMY.Speed / 4), PlayerX, PlayerY);
			GetXYRatio(&XDiff, &YDiff, Angle, CURRENTENEMY.Speed);
			CURRENTENEMY.XVel = XDiff;
			CURRENTENEMY.YVel = YDiff;
			CURRENTENEMY.WorldX += XDiff;
			CURRENTENEMY.WorldY += YDiff;

			CURRENTENEMY.ShotCounter++;
			if (CURRENTENEMY.ShotCounter > 140)
			{
				CURRENTENEMY.Shoot(5,PlayerX,PlayerY);
				CURRENTENEMY.ShotCounter = 0;
			}

			if (IsIntersecting(PlayerRect,CURRENTENEMY.CollisionRect))
			{
				Damaged = true;
				DamageDealt = 20;
			}

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, Spawner, Screen);
		}

		else if (CURRENTENEMY.Type == 9) //MARS 
		{
			switch (BossStage)
			{
			case 0:
				One.w = 40;
				One.h = 100;

				Two.w = 100;
				Two.h = 40;

				Three.w = 40;
				Three.h = 100;

				Four.w = 100;
				Four.h = 40;

				Five.w = 10;
				Five.h = 10;

				CURRENTENEMY.WorldX = PlayerX;
				CURRENTENEMY.WorldY = PlayerY + 200;

				BossStage = 1;
				BosNum1 = 20;

			case 1:
				CURRENTENEMY.ShotCounter++;
				BosNum1 += Direction;

				if (CURRENTENEMY.ShotCounter == 6)
				{
					CURRENTENEMY.ShotCounter = 0;
					CURRENTENEMY.CollisionRect.h = 0;
					CURRENTENEMY.Shoot(3,270 + BosNum1);
					CURRENTENEMY.CollisionRect.h = 100;

					if (BosNum1 > 160 || BosNum1 < 20) Direction *= -1;
				}

				XDiff = abs(((PlayerX - 15) - CURRENTENEMY.WorldX) / 20);
				YDiff = abs(((PlayerY + 180) - CURRENTENEMY.WorldY) / 12);

				if (CURRENTENEMY.WorldX < (PlayerX - 15)) CURRENTENEMY.WorldX += XDiff;
				else if (CURRENTENEMY.WorldX > (PlayerX - 15)) CURRENTENEMY.WorldX -= XDiff;
				if (CURRENTENEMY.WorldY < PlayerY + 180) CURRENTENEMY.WorldY += YDiff;
				else if (CURRENTENEMY.WorldY > PlayerY + 180) CURRENTENEMY.WorldY -= YDiff;

				if (CURRENTENEMY.WorldY + 250 > LevelHeight - 20) CURRENTENEMY.WorldY  = LevelHeight - 270;
				if (CURRENTENEMY.WorldX - 105 < 20) CURRENTENEMY.WorldX = 125;
				else if (CURRENTENEMY.WorldX + 145 > LevelWidth - 20) CURRENTENEMY.WorldX = LevelWidth  - 165; //TODO finish this

				One.x = CURRENTENEMY.WorldX - Camera.x;
				One.y = CURRENTENEMY.WorldY - Camera.y;

				Five.x = One.x + 15;
				Five.y = One.y + 120;

				Two.x = Five.x + 30;
				Two.y = Five.y - 15;

				Three.x = One.x;
				Three.y = Five.y + 30;

				Four.x = Five.x - 120;
				Four.y = Two.y;

				*RenderRect = One;
				SDL_FillRect(Screen,RenderRect,0xFFFFFF);
				*RenderRect = Two;
				SDL_FillRect(Screen,RenderRect,0xC0C0C0);
				*RenderRect = Three;
				SDL_FillRect(Screen,RenderRect,0xC0C0C0);
				*RenderRect = Four;
				SDL_FillRect(Screen,RenderRect,0xC0C0C0);
				*RenderRect = Five;
				SDL_FillRect(Screen,RenderRect,0xC0C0C0);

				PlayerRectangle.x = PlayerX - Camera.x;
				PlayerRectangle.y = PlayerY - Camera.y;
				PlayerRectangle.w = 15;
				PlayerRectangle.h = 30;

				if (IsIntersecting(PlayerRectangle,One) || IsIntersecting(PlayerRectangle,Two) || IsIntersecting(PlayerRectangle,Three) || IsIntersecting(PlayerRectangle,Four))
				{
					Damaged = true;
					DamageDealt = 50;
				}

				if (CURRENTENEMY.Health <= 5500) 
				{
					BossStage = 2;
					CURRENTENEMY.ShotCounter = 0;
					BosNum1 = 0;
					CURRENTENEMY.XVel = 0;
					CURRENTENEMY.YVel = 0;

					Shake = true;
					Mag = 20;
					Dur = 40;

					CURRENTENEMY.WorldX += 50;
					CURRENTENEMY.WorldY += 105;

					CURRENTENEMY.CollisionRect.w = 100;
					CURRENTENEMY.CollisionRect.h = 40;

					CreateDebris(10,6,CURRENTENEMY.WorldX + 10, CURRENTENEMY.WorldY + 10, 0,0,0xFFFFFF);
				}
				break;

			case 2:
				if (CURRENTENEMY.XVel >= 0) CURRENTENEMY.XVel+= 0.1;
				else CURRENTENEMY.XVel-= 0.1;
				if (CURRENTENEMY.YVel >= 0) CURRENTENEMY.YVel+= 0.1;
				else CURRENTENEMY.YVel-= 0.1;

				if (CURRENTENEMY.WorldX + CURRENTENEMY.XVel + 100 > LevelWidth - 20 || CURRENTENEMY.WorldX + CURRENTENEMY.XVel - 150 < 20) CURRENTENEMY.XVel *= -0.5;
				if (CURRENTENEMY.WorldY + CURRENTENEMY.YVel + 140 > LevelHeight - 20 || CURRENTENEMY.WorldY + CURRENTENEMY.YVel - 105 < 20) CURRENTENEMY.YVel *= -0.5;

				CURRENTENEMY.WorldX += CURRENTENEMY.XVel;
				CURRENTENEMY.WorldY += CURRENTENEMY.YVel;

				Two.x = CURRENTENEMY.WorldX - CameraX;
				Two.y = CURRENTENEMY.WorldY - CameraY;

				Five.x = Two.x - 30;
				Five.y = Two.y + 15;

				Three.x = Two.x - 45;
				Three.y = Five.y + 30;

				Four.x = Five.x - 120;
				Four.y = Two.y;

				*RenderRect = Two;
				SDL_FillRect(Screen,RenderRect,0xFFFFFF);
				*RenderRect = Three;
				SDL_FillRect(Screen,RenderRect,0xFFFFFF);
				*RenderRect = Four;
				SDL_FillRect(Screen,RenderRect,0xFFFFFF);
				*RenderRect = Five;
				SDL_FillRect(Screen,RenderRect,0xFFFFFF);

				PlayerRectangle.x = PlayerX - Camera.x;
				PlayerRectangle.y = PlayerY - Camera.y;
				PlayerRectangle.w = 15;
				PlayerRectangle.h = 30;

				if (IsIntersecting(PlayerRectangle,Two) || IsIntersecting(PlayerRectangle,Three) || IsIntersecting(PlayerRectangle,Four))
				{
					Damaged = true;
					DamageDealt = 50;
				}

				CURRENTENEMY.ShotCounter++;
				if (CURRENTENEMY.ShotCounter > 600) BossStage = 3;
				break;

			case 3:
				CURRENTENEMY.ShotCounter++;
				if (CURRENTENEMY.ShotCounter > 220)
				{
					CURRENTENEMY.ShotCounter = 210;
					CURRENTENEMY.CollisionRect.w = 200;
					CURRENTENEMY.Shoot(1,90);
					CURRENTENEMY.CollisionRect.w = 100;

					BosNum1++;
					if (BosNum1 > 15)
					{
						BosNum1 = 0;
						CURRENTENEMY.ShotCounter = 0;
						CURRENTENEMY.Shoot(5,45);
						CURRENTENEMY.Shoot(5,135);
						CURRENTENEMY.Shoot(5,90);
					}
				}
				
				XDiff = abs(((PlayerX - 255) - CURRENTENEMY.WorldX) / 20);
				YDiff = abs(((PlayerY + 15) - CURRENTENEMY.WorldY) / 12);

				if (CURRENTENEMY.WorldX < (PlayerX - 255)) CURRENTENEMY.WorldX += XDiff;
				else if (CURRENTENEMY.WorldX > (PlayerX - 255)) CURRENTENEMY.WorldX -= XDiff;
				if (CURRENTENEMY.WorldY < PlayerY + 15) CURRENTENEMY.WorldY += YDiff;
				else if (CURRENTENEMY.WorldY > PlayerY + 15) CURRENTENEMY.WorldY -= YDiff;

				if (CURRENTENEMY.WorldY < 20) CURRENTENEMY.WorldY = 20;
				else if (CURRENTENEMY.WorldY + 145 > LevelHeight - 20) CURRENTENEMY.WorldY = LevelHeight - 165;
				if (CURRENTENEMY.WorldX - 150 < 20) CURRENTENEMY.WorldX = 170;

				Two.x = CURRENTENEMY.WorldX - CameraX;
				Two.y = CURRENTENEMY.WorldY - CameraY;

				Five.x = Two.x - 30;
				Five.y = Two.y + 15;

				Three.x = Two.x - 45;
				Three.y = Five.y + 30;

				Four.x = Five.x - 120;
				Four.y = Two.y;

				*RenderRect = Two;
				SDL_FillRect(Screen,RenderRect,0xFFFFFF);
				*RenderRect = Three;
				SDL_FillRect(Screen,RenderRect,0xC0C0C0);
				*RenderRect = Four;
				SDL_FillRect(Screen,RenderRect,0xC0C0C0);
				*RenderRect = Five;
				SDL_FillRect(Screen,RenderRect,0xC0C0C0);

				PlayerRectangle.x = PlayerX - Camera.x;
				PlayerRectangle.y = PlayerY - Camera.y;
				PlayerRectangle.w = 15;
				PlayerRectangle.h = 30;

				if (IsIntersecting(PlayerRectangle,Two) || IsIntersecting(PlayerRectangle,Three) || IsIntersecting(PlayerRectangle,Four))
				{
					Damaged = true;
					DamageDealt = 50;
				}

				if (CURRENTENEMY.Health <= 4000) 
				{
					BossStage = 4;
					CURRENTENEMY.ShotCounter = 0;
					BosNum1 = 0;
					CURRENTENEMY.XVel = 0;
					CURRENTENEMY.YVel = 0;

					Shake = true;
					Mag = 20;
					Dur = 40;

					CURRENTENEMY.WorldX -= 45;
					CURRENTENEMY.WorldY += 45;

					CURRENTENEMY.CollisionRect.w = 40;
					CURRENTENEMY.CollisionRect.h = 100;

					CreateDebris(10,6,CURRENTENEMY.WorldX + 10, CURRENTENEMY.WorldY + 10, 0,0,0xFFFFFF);
				}
				break;

			case 4:
				if (CURRENTENEMY.XVel >= 0) CURRENTENEMY.XVel+= 0.1;
				else CURRENTENEMY.XVel-= 0.1;
				if (CURRENTENEMY.YVel >= 0) CURRENTENEMY.YVel+= 0.1;
				else CURRENTENEMY.YVel-= 0.1;

				if (CURRENTENEMY.WorldX + CURRENTENEMY.XVel + 40 > LevelWidth - 20 || CURRENTENEMY.WorldX + CURRENTENEMY.XVel - 105 < 20) CURRENTENEMY.XVel *= -0.7;
				if (CURRENTENEMY.WorldY + CURRENTENEMY.YVel + 100 > LevelHeight - 20 || CURRENTENEMY.WorldY + CURRENTENEMY.YVel - 150 < 20) CURRENTENEMY.YVel *= -0.7;

				CURRENTENEMY.WorldX += CURRENTENEMY.XVel;
				CURRENTENEMY.WorldY += CURRENTENEMY.YVel;

				Three.x = CURRENTENEMY.WorldX - CameraX;
				Three.y = CURRENTENEMY.WorldY - CameraY;

				Five.x = Three.x + 15;
				Five.y = Three.y - 30;

				Four.x = Five.x - 120;
				Four.y = Five.y - 15;

				*RenderRect = Three;
				SDL_FillRect(Screen,RenderRect,0xFFFFFF);
				*RenderRect = Four;
				SDL_FillRect(Screen,RenderRect,0xFFFFFF);
				*RenderRect = Five;
				SDL_FillRect(Screen,RenderRect,0xFFFFFF);

				PlayerRectangle.x = PlayerX - Camera.x;
				PlayerRectangle.y = PlayerY - Camera.y;
				PlayerRectangle.w = 15;
				PlayerRectangle.h = 30;

				if (IsIntersecting(PlayerRectangle,Three) || IsIntersecting(PlayerRectangle,Four))
				{
					Damaged = true;
					DamageDealt = 50;
				}

				CURRENTENEMY.ShotCounter++;
				if (CURRENTENEMY.ShotCounter > 600) 
				{
					BossStage = 5;
					CURRENTENEMY.ShotCounter = 0;
				}

				break;

			case 5:
				CURRENTENEMY.ShotCounter++;

				SpareStream.str("");
				SpareStream << CURRENTENEMY.ShotCounter;
				Message = TTF_RenderText_Solid(SysSmall,SpareStream.str().c_str(),Green);
				ApplySurface(0,0,Message,Screen);

				if (CURRENTENEMY.ShotCounter % 30 == 0)
				{
					CURRENTENEMY.CollisionRect.h = 200;
					CURRENTENEMY.Shoot(1,PlayerX,PlayerY);
					CURRENTENEMY.CollisionRect.h = 100;
				}

				if (CURRENTENEMY.ShotCounter == 100)
				{
					CURRENTENEMY.CollisionRect.h = 200;
					for (int mem = 0; mem <= 360; mem += 30) 
					{
						CURRENTENEMY.Shoot(3, 90 + mem);
					}
					CURRENTENEMY.CollisionRect.h = 100;
				}
	
				else if (CURRENTENEMY.ShotCounter == 200)
				{
					CURRENTENEMY.CollisionRect.h = 200;
					for (int mem = 0; mem <= 360; mem += 45) CURRENTENEMY.Shoot(4,90 + mem);
					CURRENTENEMY.CollisionRect.h = 100;
				}
	
				else if (CURRENTENEMY.ShotCounter == 350)
				{
					CURRENTENEMY.CollisionRect.h = 200;
					for (int mem = 0; mem <= 360; mem += 60) CURRENTENEMY.Shoot(3, 90 + mem);
					for (int mem = 0; mem <= 360; mem += 120) CURRENTENEMY.Shoot(4, 90 + mem);
					for (int mem = 0; mem <= 360; mem += 30) CURRENTENEMY.Shoot(1, 90 + mem);
					CURRENTENEMY.ShotCounter = 0;
					CURRENTENEMY.CollisionRect.h = 100;
				}
				
				XDiff = abs(((PlayerX - 15) - CURRENTENEMY.WorldX) / 20);
				YDiff = abs(((PlayerY - 220) - CURRENTENEMY.WorldY) / 12);

				if (CURRENTENEMY.WorldX < (PlayerX - 15)) CURRENTENEMY.WorldX += XDiff;
				else if (CURRENTENEMY.WorldX > (PlayerX - 15)) CURRENTENEMY.WorldX -= XDiff;
				if (CURRENTENEMY.WorldY < PlayerY - 220) CURRENTENEMY.WorldY += YDiff;
				else if (CURRENTENEMY.WorldY > PlayerY - 220) CURRENTENEMY.WorldY -= YDiff;

				if (CURRENTENEMY.WorldX - 105 < 20) CURRENTENEMY.WorldX = 125;
				else if (CURRENTENEMY.WorldX + 40 > LevelWidth - 20) CURRENTENEMY.WorldX = LevelWidth - 60;
				if (CURRENTENEMY.WorldY - 45 < 20) CURRENTENEMY.WorldY = 65;

				Three.x = CURRENTENEMY.WorldX - CameraX;
				Three.y = CURRENTENEMY.WorldY - CameraY;

				Five.x = Three.x + 15;
				Five.y = Three.y - 30;

				Four.x = Five.x - 120;
				Four.y = Five.y - 15;

				*RenderRect = Three;
				SDL_FillRect(Screen,RenderRect,0xFFFFFF);
				*RenderRect = Four;
				SDL_FillRect(Screen,RenderRect,0xC0C0C0);
				*RenderRect = Five;
				SDL_FillRect(Screen,RenderRect,0xC0C0C0);

				PlayerRectangle.x = PlayerX - Camera.x;
				PlayerRectangle.y = PlayerY - Camera.y;
				PlayerRectangle.w = 15;
				PlayerRectangle.h = 30;

				if (IsIntersecting(PlayerRectangle,Three) || IsIntersecting(PlayerRectangle,Four))
				{
					Damaged = true;
					DamageDealt = 50;
				}


				if (CURRENTENEMY.Health <= 1500) 
				{
					BossStage = 6;
					CURRENTENEMY.ShotCounter = 0;
					BosNum1 = 0;
					CURRENTENEMY.XVel = 0;
					CURRENTENEMY.YVel = 0;

					Shake = true;
					Mag = 20;
					Dur = 40;

					CURRENTENEMY.WorldX -= 105;
					CURRENTENEMY.WorldY -= 45;

					CURRENTENEMY.CollisionRect.w = 100;
					CURRENTENEMY.CollisionRect.h = 40;

					CreateDebris(10,6,CURRENTENEMY.WorldX + 10, CURRENTENEMY.WorldY + 10, 0,0,0xFFFFFF);
				}
				break;

			case 6:
				if (CURRENTENEMY.XVel >= 0) CURRENTENEMY.XVel+= 0.1;
				else CURRENTENEMY.XVel-= 0.1;
				if (CURRENTENEMY.YVel >= 0) CURRENTENEMY.YVel+= 0.1;
				else CURRENTENEMY.YVel-= 0.1;

				if (CURRENTENEMY.WorldX + CURRENTENEMY.XVel + 130 > LevelWidth - 20 || CURRENTENEMY.WorldX + CURRENTENEMY.XVel < 20) CURRENTENEMY.XVel *= -0.9;
				if (CURRENTENEMY.WorldY + CURRENTENEMY.YVel + 40 > LevelHeight - 20 || CURRENTENEMY.WorldY + CURRENTENEMY.YVel  < 20) CURRENTENEMY.YVel *= -0.9;

				CURRENTENEMY.WorldX += CURRENTENEMY.XVel;
				CURRENTENEMY.WorldY += CURRENTENEMY.YVel;

				Four.x = CURRENTENEMY.WorldX - CameraX;
				Four.y = CURRENTENEMY.WorldY - CameraY;

				Five.x = Four.x + 120;
				Five.y = Four.y + 15;

				*RenderRect = Four;
				SDL_FillRect(Screen,RenderRect,0xFFFFFF);
				*RenderRect = Five;
				SDL_FillRect(Screen,RenderRect,0xFFFFFF);

				PlayerRectangle.x = PlayerX - Camera.x;
				PlayerRectangle.y = PlayerY - Camera.y;
				PlayerRectangle.w = 15;
				PlayerRectangle.h = 30;

				if (IsIntersecting(PlayerRectangle,Four))
				{
					Damaged = true;
					DamageDealt = 50;
				}

				CURRENTENEMY.ShotCounter++;
				if (CURRENTENEMY.ShotCounter > 600) 
				{
					BossStage = 7;
					CURRENTENEMY.ShotCounter = 0;
				}

				break;

			case 7:
				
				XDiff = abs(((PlayerX + 165) - CURRENTENEMY.WorldX) / 10);
				YDiff = abs(((PlayerY - 10) - CURRENTENEMY.WorldY) / 10);

				if (CURRENTENEMY.WorldX < (PlayerX + 165)) CURRENTENEMY.WorldX += XDiff;
				else if (CURRENTENEMY.WorldX > (PlayerX + 165)) CURRENTENEMY.WorldX -= XDiff;
				if (CURRENTENEMY.WorldY < PlayerY - 10) CURRENTENEMY.WorldY += YDiff;
				else if (CURRENTENEMY.WorldY > PlayerY - 10) CURRENTENEMY.WorldY -= YDiff;
				
				CURRENTENEMY.ShotCounter++;
				if (CURRENTENEMY.ShotCounter % 10 == 0) CURRENTENEMY.Shoot(3,rand() % 360);
				else if (CURRENTENEMY.ShotCounter == 201)
				{
					for (int x = 180; x <= 360; x += 45) CURRENTENEMY.Shoot(6,x);
					CURRENTENEMY.ShotCounter = 0;
				}
				
				if (CURRENTENEMY.WorldX + 130 > LevelWidth - 20) CURRENTENEMY.WorldX = LevelWidth - 150;
				if (CURRENTENEMY.WorldY < 20) CURRENTENEMY.WorldY = 20;
				else if (CURRENTENEMY.WorldY + 145 > LevelHeight - 20) CURRENTENEMY.WorldY = LevelHeight - 165;

				Four.x = CURRENTENEMY.WorldX - Camera.x;
				Four.y = CURRENTENEMY.WorldY - Camera.y;

				Five.x = Four.x + 120;
				Five.y = Four.y + 15;

				*RenderRect = Four;
				SDL_FillRect(Screen,RenderRect,0xFFFFFF);
				*RenderRect = Five;
				SDL_FillRect(Screen,RenderRect,0xFFFFFF);

				PlayerRectangle.x = PlayerX - Camera.x;
				PlayerRectangle.y = PlayerY - Camera.y;
				PlayerRectangle.w = 15;
				PlayerRectangle.h = 30;

				if (IsIntersecting(PlayerRectangle,Four))
				{
					Damaged = true;
					DamageDealt = 50;
				}

				if (CURRENTENEMY.Health < 500)
				{
					BossStage = 8;
					CURRENTENEMY.ShotCounter = 0;

					CURRENTENEMY.CollisionRect.w = 20;
					CURRENTENEMY.CollisionRect.h = 20;

					Five.w = 20;
					Five.h = 20;

					CURRENTENEMY.XVel = 0;
					CURRENTENEMY.YVel = 0;

					CURRENTENEMY.WorldX += 120;
					CURRENTENEMY.WorldY += 15;

					CURRENTENEMY.Moving = false;
				}
				break;

			case 8:
				if (CURRENTENEMY.WorldX + CURRENTENEMY.XVel + 10 > LevelWidth - 20 || CURRENTENEMY.WorldX + CURRENTENEMY.XVel < 20) CURRENTENEMY.XVel *= -0.3;
				if (CURRENTENEMY.WorldY + CURRENTENEMY.YVel + 10 > LevelHeight - 20 || CURRENTENEMY.WorldY + CURRENTENEMY.YVel < 20) CURRENTENEMY.YVel *= -0.3;

				CURRENTENEMY.ShotCounter++;
				
				if (CURRENTENEMY.ShotCounter % 10 == 0) CURRENTENEMY.Shoot(1,CURRENTENEMY.ShotCounter);

				if (CURRENTENEMY.ShotCounter == 200)
				{
					for (int w = 0; w < 269; w+= 90) CURRENTENEMY.Shoot(5,w);
				}

				if (CURRENTENEMY.ShotCounter == 400)
				{
					for (int w = 0; w <= 270; w+= 54) CURRENTENEMY.Shoot(3,w);
					CURRENTENEMY.ShotCounter = 0;
				}

				Five.x = CURRENTENEMY.WorldX - Camera.x;
				Five.y = CURRENTENEMY.WorldY - Camera.y;
				*RenderRect = Five;

				SDL_FillRect(Screen,RenderRect,0xFF0000);

			}

			SDL_Rect BossHeight;
			BossHeight.x = 100;
			BossHeight.y = 10;
			BossHeight.h = 20;
			BossHeight.w = CURRENTENEMY.Health / 4;

			SDL_FillRect(Screen, &BossHeight, 0x00FF00);

			Message = TTF_RenderText_Solid(SysSmall,"MARS: ",Green);
			ApplySurface(100 - (Message->w), 10, Message, Screen);

		}

		else if (CURRENTENEMY.Type == 10) //Frog
		{
			CURRENTENEMY.ShotCounter++;

			if (CURRENTENEMY.ShotCounter > 30)
			{
				int Direction = CalculateProjectileAngle(CURRENTENEMY.WorldX, CURRENTENEMY.WorldY, PlayerX, PlayerY);
				GetXYRatio(&CURRENTENEMY.XVel, &CURRENTENEMY.YVel, Direction, CURRENTENEMY.Speed);
				CURRENTENEMY.ShotCounter = -30;
			}

			else if (CURRENTENEMY.ShotCounter == 0)
			{
				CURRENTENEMY.XVel = 0;
				CURRENTENEMY.YVel = 0;
			}

			CURRENTENEMY.WorldX += CURRENTENEMY.XVel;
			CURRENTENEMY.WorldY += CURRENTENEMY.YVel;


			if (IsIntersecting(CURRENTENEMY.CollisionRect,PlayerRect))
			{
				CURRENTENEMY.Health = 0;
				Damaged = true;
				DamageDealt = 40;
			}

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, Frog, Screen);
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

				if (tni <= 7)
				{
					SpareStream.str("");
					SpareStream << "Resources/Sounds/Death" << (tni % NUMBEROFDEATHS) + 1 << ".wav";
					Mix_Chunk *PlayThis = Mix_LoadWAV(SpareStream.str().c_str());
					Mix_PlayChannel(-1,PlayThis,0);
				}

				else if (tni < 75 &&CURRENTENEMY.Type == 1)
				{
					Pickup pukciP; //pukciP = Pickup backwards
					pukciP.WorldX = CURRENTENEMY.WorldX;
					pukciP.WorldY = CURRENTENEMY.WorldY;
					pukciP.Type = 1;
					PickupVector.push_back(pukciP);
				}

				else if (tni < 50 && CURRENTENEMY.Type == 2)
				{
					Pickup pukciP; //pukciP = Pickup backwards
					pukciP.WorldX = CURRENTENEMY.WorldX;
					pukciP.WorldY = CURRENTENEMY.WorldY;
					pukciP.Type = 2;
					PickupVector.push_back(pukciP);
				}

				else if (tni < 100 && CURRENTENEMY.Type == 8)
				{
					Pickup pukciP; //pukciP = Pickup backwards
					pukciP.WorldX = CURRENTENEMY.WorldX;
					pukciP.WorldY = CURRENTENEMY.WorldY;
					pukciP.Type = 4;
					PickupVector.push_back(pukciP);
				}

				CreateDebris(5,6,CURRENTENEMY.WorldX,CURRENTENEMY.WorldY,CURRENTENEMY.XVel * 4,CURRENTENEMY.YVel * 4,0xFFFFFF);

				if (CURRENTENEMY.Type == 3) //Explosion
				{
					for (int OutOfRange = 0; OutOfRange < 360; OutOfRange += 45)
					{
						CURRENTENEMY.Shoot(1,OutOfRange);
					}
				}
			}

			if (CURRENTENEMY.Type == 4 || CURRENTENEMY.Type == 9) 
			{
				Boss = false;
				Temp1 = CURRENTENEMY.WorldX;
				Temp2 = CURRENTENEMY.WorldY;
			}

			EnemyVector.erase(EnemyVector.begin() + i, EnemyVector.begin() + i + 1);
			Enemies = EnemyVector.size();
		}
	}
}

#define CURRENTENEMYPROJECTILE EnemyProjectileVector.at(wilk)