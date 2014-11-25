#include"DoProjectiles.h"
#include"Debris.h"
#include"Declarations.h"
#include"GetXYRatio.h"

void CreateProjectile(float x, float y, float XRatio, float YRatio, int Type)
{
	Projectile elitcejorP;
	elitcejorP.Type = Type;
	elitcejorP.WorldX = x;
	elitcejorP.WorldY = y;
	elitcejorP.XInc = XRatio;
	elitcejorP.YInc = YRatio;
	elitcejorP.ProjectileRect.x = x;
	elitcejorP.ProjectileRect.y = y;

	switch (Type)
	{
	case 1:
		elitcejorP.ProjectileRect.w = 2;
		elitcejorP.ProjectileRect.h = 2;
		elitcejorP.Damage = 20;
		break;
	case 2:
		elitcejorP.ProjectileRect.w = 2;
		elitcejorP.ProjectileRect.h = 2;
		elitcejorP.Damage = 35;
		break;

	case 3:
		elitcejorP.ProjectileRect.w = 4;
		elitcejorP.ProjectileRect.h = 4;
		elitcejorP.Damage = 3;
		break;

	case 4:
		elitcejorP.ProjectileRect.w = 3;
		elitcejorP.ProjectileRect.h = 3;
		elitcejorP.Damage = 5;

	case 5:
		elitcejorP.ProjectileRect.w = 5;
		elitcejorP.ProjectileRect.h = 5;
		elitcejorP.Damage = 250;
	}

	elitcejorP.Time = -1;
	ProjectileVector.push_back(elitcejorP);
}

#define CURRENTPROJECTILE ProjectileVector.at(i)
void DoProjectiles(int CameraX, int CameraY)
{
	if (ProjectileVector.size() > 0)
	{
		for( int i = 0; i < ProjectileVector.size(); i++ )
		{	
			bool Erase = false;
			CURRENTPROJECTILE.WorldX += CURRENTPROJECTILE.XInc;
			CURRENTPROJECTILE.WorldY += CURRENTPROJECTILE.YInc;

			if (CURRENTPROJECTILE.Type == 3 && CURRENTPROJECTILE.Time == -1)
			{
				CURRENTPROJECTILE.XInc /= 1.1;
				CURRENTPROJECTILE.YInc /= 1.1;

				if (abs(CURRENTPROJECTILE.XInc) < 2.5 && abs(CURRENTPROJECTILE.YInc) < 2.5) CURRENTPROJECTILE.Time = rand() % 25 + 5;
			}

			else if (CURRENTPROJECTILE.Type == 5) CreateProjectile(CURRENTPROJECTILE.WorldX,CURRENTPROJECTILE.WorldY,CURRENTPROJECTILE.XInc * -1 + rand() % 6 - 3, CURRENTPROJECTILE.YInc * -1 + rand() % 6 - 3, 3);

			if (CURRENTPROJECTILE.Time > 0) CURRENTPROJECTILE.Time--; 

			CURRENTPROJECTILE.ProjectileRect.x = CURRENTPROJECTILE.WorldX;
			CURRENTPROJECTILE.ProjectileRect.y = CURRENTPROJECTILE.WorldY;
			if (LevelVector.size() > 0)
			{
				for (int wilkins = 0; wilkins < LevelVector.size(); wilkins++)
				{
					SDL_Rect TempRect;
					TempRect.x = LevelVector.at(wilkins).WorldX;
					TempRect.y = LevelVector.at(wilkins).WorldY;
					TempRect.w = LevelVector.at(wilkins).Width;
					TempRect.h = LevelVector.at(wilkins).Height;
					if (IsIntersecting(TempRect, CURRENTPROJECTILE.ProjectileRect))
					{
						
						if (CURRENTPROJECTILE.Type != 3)
						{
							CreateDebris(2,3,CURRENTPROJECTILE.WorldX,CURRENTPROJECTILE.WorldY,-CURRENTPROJECTILE.XInc / 2,-CURRENTPROJECTILE.YInc / 2,0xFFFFFF);
							Erase = true; 
						}

						else
						{
							CURRENTPROJECTILE.XInc = 0;
							CURRENTPROJECTILE.YInc = 0;
						}

						if (CURRENTPROJECTILE.Type == 5)
						{
							int RocketX = CURRENTPROJECTILE.WorldX;
							int RocketY = CURRENTPROJECTILE.WorldY;

							Shake = true;
							Mag = 20;
							Dur = 50;

							for (int i = 0; i <= 360; i++)
							{
								float XR = 0;
								float YR = 0;
								GetXYRatio(&XR,&YR,i,rand() % 20);
								CreateProjectile(RocketX,RocketY,XR,YR,3);
							}
						}
					}
					if (Erase == true) break;
				}
			}
			if (CURRENTPROJECTILE.Time == 0) Erase = true;

			if (Erase == false)
			{
				CURRENTPROJECTILE.ProjectileRect.x = CURRENTPROJECTILE.WorldX - CameraX;
				CURRENTPROJECTILE.ProjectileRect.y = CURRENTPROJECTILE.WorldY - CameraY;
				if (CURRENTPROJECTILE.Type == 3) SDL_FillRect(Screen,&CURRENTPROJECTILE.ProjectileRect,0xFF6A00);
				else if (CURRENTPROJECTILE.Type == 4) SDL_FillRect(Screen,&CURRENTPROJECTILE.ProjectileRect,0xFF0000);
				else SDL_FillRect(Screen,&CURRENTPROJECTILE.ProjectileRect,0xFFFF00);
			}

			else ProjectileVector.erase(ProjectileVector.begin() + i);
		}
	}
}