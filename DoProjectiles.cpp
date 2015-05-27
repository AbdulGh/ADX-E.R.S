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
	elitcejorP.Burning = false;
	elitcejorP.Active = true;
	elitcejorP.Time = -1;

	switch (Type)
	{
	case 1:
		elitcejorP.ProjectileRect.w = 4;
		elitcejorP.ProjectileRect.h = 4;
		elitcejorP.Damage = 2000;
		break;
	case 2:
		elitcejorP.ProjectileRect.w = 3;
		elitcejorP.ProjectileRect.h = 3;
		elitcejorP.Damage = 35;
		break;

	case 3:
		elitcejorP.ProjectileRect.w = 4;
		elitcejorP.ProjectileRect.h = 4;
		elitcejorP.Damage = 3;
		elitcejorP.Burning = true;
		break;

	case 4:
		elitcejorP.ProjectileRect.w = 3;
		elitcejorP.ProjectileRect.h = 3;
		elitcejorP.Damage = 4;
		elitcejorP.Burning = true;
		break;

	case 5:
		elitcejorP.ProjectileRect.w = 7;
		elitcejorP.ProjectileRect.h = 7;
		elitcejorP.Damage = 250;
		break;

	case 6:
		elitcejorP.ProjectileRect.w = 7;
		elitcejorP.ProjectileRect.h = 7;
		elitcejorP.Damage = 50;
		elitcejorP.Time = 40;

	}
	ProjectileVector.push_back(elitcejorP);
}

bool Projectile::IsNotActive() const
{
	return !(Active);
}

void CreateExplosion(int X, int Y)
{
	Shake = true;
	Mag = 15;
	Dur = 25;

	for (int i = 0; i <= 360; i += 2)
	{
		float XR = 0;
		float YR = 0;
		GetXYRatio(&XR, &YR, i, rand() % 20);
		CreateProjectile(X, Y, XR, YR, 3);
	}
}

#define CURRENTPROJECTILE ProjectileVector.at(i)
void DoProjectiles(int CameraX, int CameraY)
{
	if (ProjectileVector.size() > 0)
	{
		for( int i = 0; i < ProjectileVector.size(); i++ )
		{	
			CURRENTPROJECTILE.WorldX += CURRENTPROJECTILE.XInc;
			CURRENTPROJECTILE.WorldY += CURRENTPROJECTILE.YInc;

			if ((CURRENTPROJECTILE.Type == 3 && CURRENTPROJECTILE.Time == -1) || CURRENTPROJECTILE.Type == 6)
			{
				CURRENTPROJECTILE.XInc /= 1.08;
				CURRENTPROJECTILE.YInc /= 1.08;

				if (CURRENTPROJECTILE.Type == 3 && abs(CURRENTPROJECTILE.XInc) < 2.5 && abs(CURRENTPROJECTILE.YInc) < 2.5) CURRENTPROJECTILE.Time = rand() % 25 + 5;
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
						
						if (CURRENTPROJECTILE.Type != 3 && CURRENTPROJECTILE.Type != 6)
						{
							CreateDebris(2,3,CURRENTPROJECTILE.WorldX,CURRENTPROJECTILE.WorldY,-CURRENTPROJECTILE.XInc / 2,-CURRENTPROJECTILE.YInc / 2,0xFFFFFF);
							CURRENTPROJECTILE.Active = false; 
						}

						else
						{
							CURRENTPROJECTILE.XInc = 0;
							CURRENTPROJECTILE.YInc = 0;
						}

						if (CURRENTPROJECTILE.Type == 5)
						{
							CreateExplosion(CURRENTPROJECTILE.WorldX, CURRENTPROJECTILE.WorldY);
						}
					}
					if (CURRENTPROJECTILE.Active == false) break;
				}
			}
			if (CURRENTPROJECTILE.Time == 0)
			{
				if (CURRENTPROJECTILE.Type == 6) CreateExplosion(CURRENTPROJECTILE.WorldX, CURRENTPROJECTILE.WorldY);
				CURRENTPROJECTILE.Active = false;
			}

			if (CURRENTPROJECTILE.Active == true)
			{
				CURRENTPROJECTILE.ProjectileRect.x = CURRENTPROJECTILE.WorldX - CameraX;
				CURRENTPROJECTILE.ProjectileRect.y = CURRENTPROJECTILE.WorldY - CameraY;
				if (CURRENTPROJECTILE.Type == 3) SDL_FillRect(Screen,&CURRENTPROJECTILE.ProjectileRect,0xFF6A00);
				else if (CURRENTPROJECTILE.Type == 4) SDL_FillRect(Screen,&CURRENTPROJECTILE.ProjectileRect,0xFF0000);
				else SDL_FillRect(Screen,&CURRENTPROJECTILE.ProjectileRect,0xFFFF00);
			}
		}
	}

	ProjectileVector.erase(
		std::remove_if(
		ProjectileVector.begin(),
		ProjectileVector.end(),
		std::mem_fun_ref((&Projectile::IsNotActive))),
		ProjectileVector.end());
}