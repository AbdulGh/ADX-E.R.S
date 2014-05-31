#include"DoProjectiles.h"
#include"Debris.h"
#include"Declarations.h"

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
	elitcejorP.ProjectileRect.w = 2;
	elitcejorP.ProjectileRect.h = 2;
	ProjectileVector.push_back(elitcejorP);
}

#define CURRENTPROJECTILE ProjectileVector.at(i)
void DoProjectiles(int CameraX, int CameraY)
{
	int What = ProjectileVector.size();
	if (What > 0)
	{
		for( int i = 0; i < ProjectileVector.size(); i++ )
		{	
			bool IWillNOTUseAGoto = false;
			CURRENTPROJECTILE.WorldX += CURRENTPROJECTILE.XInc;
			CURRENTPROJECTILE.WorldY += CURRENTPROJECTILE.YInc;
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
						
						if (CURRENTPROJECTILE.Type == 1)
						{
							CreateDebris(2,3,CURRENTPROJECTILE.WorldX,CURRENTPROJECTILE.WorldY,-CURRENTPROJECTILE.XInc / 2,-CURRENTPROJECTILE.YInc / 2,0xFFFFFF);
							ProjectileVector.erase(ProjectileVector.begin() + i, ProjectileVector.begin() + i + 1);
							IWillNOTUseAGoto = true;
							break;
						}
					}
					if (IWillNOTUseAGoto == true) break;
				}
			}

			if (IWillNOTUseAGoto == false)
			{
				CURRENTPROJECTILE.ProjectileRect.x = CURRENTPROJECTILE.WorldX - CameraX;
				CURRENTPROJECTILE.ProjectileRect.y = CURRENTPROJECTILE.WorldY - CameraY;
				SDL_FillRect(Screen,&CURRENTPROJECTILE.ProjectileRect,0xFFFF00);
			}
		}
	}
}