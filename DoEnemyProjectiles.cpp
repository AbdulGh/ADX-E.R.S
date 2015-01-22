#include"DoEnemyProjectiles.h"
#include"Declarations.h"
#include"CalculateProjectileAngle.h"
#include"GetXYRatio.h"

#include<math.h>

std::vector <EnemyProjectile> EnemyProjectileVector;

void ClearProjectiles()
{
	EnemyProjectileVector.erase(EnemyProjectileVector.begin(), EnemyProjectileVector.end());
}

EnemyProjectile::EnemyProjectile(int ProjectileType)
{
	WorldX = 0;
	WorldY = 0;
	Type = ProjectileType;

	CollisionRect.x = 0;
	CollisionRect.y = 0;
	CollisionRect.w = 0;
	CollisionRect.h = 0;

	XVel = 0;
	YVel = 0;

	Frame = 0;
	Frametime = 0;
	Damage = 0;

	Spare1 = 0;
	Spare2 = 0;

	Active = true;
	Collides = true;
	RenderRect = true;
	Friction = false;
	Homing = false;
	Circle = false;
	SpawnsEnemies = false;
	Wave = false;
}

void Enemy::Shoot(int Type, int TargetX, int TargetY)
{
	EnemyProjectile PushThis(Type);
	double Bearing = 0;
	
	switch (Type)
	{
	case 1: //Normal bullet
		Bearing = CalculateProjectileAngle(WorldX,WorldY,TargetX,TargetY);
		GetXYRatio(&PushThis.XVel, &PushThis.YVel, Bearing, 10);

		PushThis.CollisionRect.w = 5;
		PushThis.CollisionRect.h = 5;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Damage = 10;
		break;
		
	case 2: //SIS bullet
		PushThis.YVel = 0;
		PushThis.XVel = 10;

		PushThis.CollisionRect.w = 30;
		PushThis.CollisionRect.h = 10;

		PushThis.WorldX = WorldX + CollisionRect.w;
		PushThis.WorldY = WorldY + 2;

		PushThis.Damage = 30;
		PushThis.RenderRect = false;
		break;

	case 3: //Large projectile
		Bearing = CalculateProjectileAngle(WorldX,WorldY,TargetX,TargetY);
		GetXYRatio(&PushThis.XVel, &PushThis.YVel, Bearing, 6);

		PushThis.CollisionRect.w = 30;
		PushThis.CollisionRect.h = 30;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Damage = 30;
		break;

	case 4://Larger projectile
		Bearing = CalculateProjectileAngle(WorldX,WorldY,TargetX,TargetY);
		GetXYRatio(&PushThis.XVel, &PushThis.YVel, Bearing, 3);

		PushThis.CollisionRect.w = 60;
		PushThis.CollisionRect.h = 60;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Damage = 100;
		break;

	case 5: //Gunman Spawner
		Bearing = CalculateProjectileAngle(WorldX,WorldY,TargetX,TargetY);
		GetXYRatio(&PushThis.XVel, &PushThis.YVel, Bearing, 20);

		PushThis.CollisionRect.w = 5;
		PushThis.CollisionRect.h = 5;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Damage = 30;
		PushThis.Friction = true;
		PushThis.SpawnsEnemies = true;
		break;

	case 6: //Homing missiles
		Bearing = CalculateProjectileAngle(WorldX,WorldY,TargetX,TargetY);
		GetXYRatio(&PushThis.XVel, &PushThis.YVel, Bearing, 2);

		PushThis.CollisionRect.w = 10;
		PushThis.CollisionRect.h = 10;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Damage = 20;
		PushThis.Homing = true;
		break;

	case 7: //Vertical lasers
		PushThis.CollisionRect.w = 10;
		PushThis.CollisionRect.h = 100;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h - 50;

		PushThis.Collides = false;

		PushThis.Damage = 20;
		PushThis.YVel = 5;
		break;

	case 8: //Fire
		Bearing = CalculateProjectileAngle(WorldX, WorldY, TargetX, TargetY);
		GetXYRatio(&PushThis.XVel, &PushThis.YVel, Bearing, 20 + (rand() % 10 - 5));

		PushThis.CollisionRect.w = 3;
		PushThis.CollisionRect.h = 3;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Damage = 10;
		PushThis.Friction = true;

		break;

	case 9: //Sine wave
		GetXYRatio(&PushThis.XVel, &PushThis.YVel, Bearing, 6);

		PushThis.CollisionRect.w = 20;
		PushThis.CollisionRect.h = 20;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Spare1 = PushThis.WorldX;
		PushThis.Spare2 = PushThis.WorldY;

		PushThis.Frame = Bearing;
		PushThis.Frametime = 0;

		PushThis.Damage = 30;
		
		PushThis.Wave = true;
		break;

	case 10: //Circle
		PushThis.CollisionRect.w = 20;
		PushThis.CollisionRect.h = 20;

		PushThis.Damage = 30;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Frame = PushThis.WorldX;
		PushThis.Frametime = rand() % 360;
		PushThis.Spare1 = PushThis.WorldY;
		PushThis.Spare2 = 1;
		break;

	case 11: //Approacher
		PushThis.CollisionRect.w = 40;
		PushThis.CollisionRect.h = 40;

		PushThis.Damage = 50;
		PushThis.Collides = false;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Spare1 = TargetX;
		PushThis.Spare2 = TargetY;
		break;
	};
	EnemyProjectileVector.push_back(PushThis);
}


void Enemy::Shoot(int Type, int Bearing)
{
	EnemyProjectile PushThis(Type);
	
	switch (Type)
	{
	case 1: //Normal bullet
		GetXYRatio(&PushThis.XVel, &PushThis.YVel, Bearing, 10);

		PushThis.CollisionRect.w = 5;
		PushThis.CollisionRect.h = 5;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Damage = 10;
		break;
		
	case 2: //SIS bullet
		PushThis.YVel = 0;
		PushThis.XVel = 10;

		PushThis.CollisionRect.w = 30;
		PushThis.CollisionRect.h = 10;

		PushThis.WorldX = WorldX + CollisionRect.w;
		PushThis.WorldY = WorldY + 2;

		PushThis.Damage = 30;
		PushThis.RenderRect = false;
		break;

	case 3: //Large projectile
		GetXYRatio(&PushThis.XVel, &PushThis.YVel, Bearing, 6);

		PushThis.CollisionRect.w = 30;
		PushThis.CollisionRect.h = 30;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Damage = 30;
		break;

	case 4://Larger projectile
		GetXYRatio(&PushThis.XVel, &PushThis.YVel, Bearing, 3);

		PushThis.CollisionRect.w = 60;
		PushThis.CollisionRect.h = 60;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Damage = 100;
		break;

	case 5: //Gunman Spawner
		GetXYRatio(&PushThis.XVel, &PushThis.YVel, Bearing, 20);

		PushThis.CollisionRect.w = 5;
		PushThis.CollisionRect.h = 5;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Damage = 30;
		PushThis.Friction = true;
		PushThis.SpawnsEnemies = true;

		break;

	case 6: //Homing missiles
		GetXYRatio(&PushThis.XVel, &PushThis.YVel, Bearing, 3);

		PushThis.CollisionRect.w = 10;
		PushThis.CollisionRect.h = 10;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Damage = 20;
		PushThis.Homing = true;
		break;

	case 7: //Boilerplate code is better than default parameters 8^)
		break;

	case 8: //Fire
		GetXYRatio(&PushThis.XVel, &PushThis.YVel, Bearing, 20 + (rand() % 10 - 5));

		PushThis.CollisionRect.w = 3;
		PushThis.CollisionRect.h = 3;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Damage = 10;
		PushThis.Friction = true;
		break;

	case 9: //Sine wave
		GetXYRatio(&PushThis.XVel, &PushThis.YVel, Bearing, 6);

		PushThis.CollisionRect.w = 20;
		PushThis.CollisionRect.h = 20;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Spare1 = PushThis.WorldX;
		PushThis.Spare2 = PushThis.WorldY;

		PushThis.Frame = Bearing;
		PushThis.Frametime = 0;

		PushThis.Damage = 30;

		PushThis.Wave = true;
		break;

	case 10: //Circle
		PushThis.CollisionRect.w = 20;
		PushThis.CollisionRect.h = 20;

		PushThis.Damage = 30;

		PushThis.WorldX = WorldX + CollisionRect.w / 2;
		PushThis.WorldY = WorldY + CollisionRect.h / 2;

		PushThis.Frame = PushThis.WorldX;
		PushThis.Frametime = rand() % 360;
		PushThis.Spare1 = PushThis.WorldY;
		PushThis.Spare2 = 1;
		PushThis.Circle = true;
		break;
	};

	EnemyProjectileVector.push_back(PushThis);
}


#define CURRENTENEMYPROJECTILE EnemyProjectileVector.at(i)

void DoEnemyProjectiles(int CameraX, int CameraY, SDL_Rect PlayerRect)
{
	PlayerRect.x += CameraX;
	PlayerRect.y += CameraY;
	
	for (int i = 0; i < EnemyProjectileVector.size(); i++)
	{
		CURRENTENEMYPROJECTILE.Frametime++;
		
		if (CURRENTENEMYPROJECTILE.Homing)
		{
			if (CURRENTENEMYPROJECTILE.WorldX < PlayerRect.x && CURRENTENEMYPROJECTILE.XVel < 7) CURRENTENEMYPROJECTILE.XVel += 0.3;
			else if (CURRENTENEMYPROJECTILE.WorldX > PlayerRect.x && CURRENTENEMYPROJECTILE.XVel > -7) CURRENTENEMYPROJECTILE.XVel -= 0.3;

			if (CURRENTENEMYPROJECTILE.WorldY < PlayerRect.y && CURRENTENEMYPROJECTILE.YVel < 7) CURRENTENEMYPROJECTILE.YVel += 0.3;
			else if (CURRENTENEMYPROJECTILE.WorldY > PlayerRect.y && CURRENTENEMYPROJECTILE.YVel > -7) CURRENTENEMYPROJECTILE.YVel -= 0.3;
		}

		else if (CURRENTENEMYPROJECTILE.Wave)
		{
			CURRENTENEMYPROJECTILE.Spare1 += CURRENTENEMYPROJECTILE.XVel;
			CURRENTENEMYPROJECTILE.Spare2 += CURRENTENEMYPROJECTILE.YVel;

			CURRENTENEMYPROJECTILE.WorldX = CURRENTENEMYPROJECTILE.Spare1 + 200 * sin(CURRENTENEMYPROJECTILE.Frametime * 0.0525) * cos(CURRENTENEMYPROJECTILE.Frame * 0.0175); //~pi/180
			CURRENTENEMYPROJECTILE.WorldY = CURRENTENEMYPROJECTILE.Spare2 + 200 * sin(CURRENTENEMYPROJECTILE.Frametime * 0.0525) * sin(CURRENTENEMYPROJECTILE.Frame * 0.0175);
		}

		else if (CURRENTENEMYPROJECTILE.Circle)
		{
			CURRENTENEMYPROJECTILE.Spare2 += 2;

			GetXYRatio(&CURRENTENEMYPROJECTILE.XVel, &CURRENTENEMYPROJECTILE.YVel, CURRENTENEMYPROJECTILE.Frametime, CURRENTENEMYPROJECTILE.Spare2);

			CURRENTENEMYPROJECTILE.WorldX = CURRENTENEMYPROJECTILE.Frame + CURRENTENEMYPROJECTILE.XVel;
			CURRENTENEMYPROJECTILE.WorldY = CURRENTENEMYPROJECTILE.Spare1 + CURRENTENEMYPROJECTILE.YVel;
		}

		else if (CURRENTENEMYPROJECTILE.Type == 11)
		{
			int XDiff = CURRENTENEMYPROJECTILE.Spare1 - CURRENTENEMYPROJECTILE.WorldX;
			int YDiff = CURRENTENEMYPROJECTILE.Spare2 - CURRENTENEMYPROJECTILE.WorldY;

			CURRENTENEMYPROJECTILE.WorldX += XDiff / 8;
			CURRENTENEMYPROJECTILE.WorldY += YDiff / 8;

			if (CURRENTENEMYPROJECTILE.Frametime > 120)
			{
				if (SpawnFlowers)
				{
					EnemyProjectile Flower(12);
					Flower.WorldX = CURRENTENEMYPROJECTILE.Spare1;
					Flower.WorldY = CURRENTENEMYPROJECTILE.Spare2;

					Flower.CollisionRect.w = 40;
					Flower.CollisionRect.h = 40;
					Flower.Damage = 50;
					Flower.Collides = false;

					Flower.WorldX += 20;
					Flower.WorldY += 20;
					Flower.Spare1 = sqrt((Flower.WorldX - OrbitX) * (Flower.WorldX - OrbitX) + (Flower.WorldY - OrbitY) * (Flower.WorldY - OrbitY));
					Flower.Spare2 = CalculateProjectileAngle(OrbitX, OrbitY, Flower.WorldX, Flower.WorldY);
					Flower.WorldX -= 20;
					Flower.WorldY -= 20;
					Temp2 = 0;

					EnemyProjectileVector.push_back(Flower);
				}
				CURRENTENEMYPROJECTILE.Active = false;
			}
		}

		else if (CURRENTENEMYPROJECTILE.Type == 12)
		{
			CURRENTENEMYPROJECTILE.WorldX = static_cast<float>(OrbitX + CURRENTENEMYPROJECTILE.Spare1 * cos((CURRENTENEMYPROJECTILE.Spare2 + AngleOffset) * (3.141 / 180)));
			CURRENTENEMYPROJECTILE.WorldY = static_cast<float>(OrbitY + CURRENTENEMYPROJECTILE.Spare1 * sin((CURRENTENEMYPROJECTILE.Spare2 + AngleOffset) * (3.141 / 180)));
		}

		else
		{
			CURRENTENEMYPROJECTILE.WorldX += CURRENTENEMYPROJECTILE.XVel;
			CURRENTENEMYPROJECTILE.WorldY += CURRENTENEMYPROJECTILE.YVel;
		}
		
		CURRENTENEMYPROJECTILE.CollisionRect.x = CURRENTENEMYPROJECTILE.WorldX;
		CURRENTENEMYPROJECTILE.CollisionRect.y = CURRENTENEMYPROJECTILE.WorldY;

		if (CURRENTENEMYPROJECTILE.Friction)
		{
			CURRENTENEMYPROJECTILE.XVel /= 1.05;
			CURRENTENEMYPROJECTILE.YVel /= 1.05;

			if(abs(CURRENTENEMYPROJECTILE.XVel < 0.5) && abs(CURRENTENEMYPROJECTILE.YVel < 0.5))
			{
				if (CURRENTENEMYPROJECTILE.SpawnsEnemies)
				{
					SpawnVector.erase(SpawnVector.begin(),SpawnVector.end());
					SpawnVector.push_back(CURRENTENEMYPROJECTILE.WorldX);
					SpawnVector.push_back(CURRENTENEMYPROJECTILE.WorldY);
					SpawnVector.push_back(2);
					SpawnEnemies(SpawnVector);
				}
				CURRENTENEMYPROJECTILE.Active = false;
			}
		}
		
		if (CURRENTENEMYPROJECTILE.Collides)
		{
			SDL_Rect CurrentTile;
			for (int eye = 0; eye < LevelVector.size(); eye++)
			{
				CurrentTile.x = LevelVector.at(eye).WorldX;
				CurrentTile.y = LevelVector.at(eye).WorldY;
				CurrentTile.w = LevelVector.at(eye).Width;
				CurrentTile.h = LevelVector.at(eye).Height;
				
				if (IsIntersecting(CurrentTile,CURRENTENEMYPROJECTILE.CollisionRect))
				{
					CURRENTENEMYPROJECTILE.Active = false;
					if (CURRENTENEMYPROJECTILE.Type != 10) CreateDebris(CURRENTENEMYPROJECTILE.CollisionRect.h / 3, 6, CURRENTENEMYPROJECTILE.WorldX, CURRENTENEMYPROJECTILE.WorldY, CURRENTENEMYPROJECTILE.XVel * -1, CURRENTENEMYPROJECTILE.YVel * -1, 0xFFFFFF);
				}
			}
		}
		
		if (IsIntersecting(PlayerRect, CURRENTENEMYPROJECTILE.CollisionRect) && !Invincible)
		{
			Damaged = true;
			DamageDealt = CURRENTENEMYPROJECTILE.Damage;
			if (CURRENTENEMYPROJECTILE.Collides) CURRENTENEMYPROJECTILE.Active = false;
		}

		if (CURRENTENEMYPROJECTILE.Active)
		{
			SDL_Rect RenderRect;
			if (CURRENTENEMYPROJECTILE.RenderRect)
			{
				RenderRect = CURRENTENEMYPROJECTILE.CollisionRect;
				RenderRect.x -= CameraX;
				RenderRect.y -= CameraY;
				if (CURRENTENEMYPROJECTILE.Type != 7 && CURRENTENEMYPROJECTILE.Type != 6 && CURRENTENEMYPROJECTILE.Type != 8) SDL_FillRect(Screen, &RenderRect, 0xFFFFFF);
				else if (CURRENTENEMYPROJECTILE.Type == 7 || CURRENTENEMYPROJECTILE.Type == 6) SDL_FillRect(Screen, &RenderRect, 0xFF0000);
				else SDL_FillRect(Screen, &RenderRect, 0xFF6A00);
			}

			else
			{
				switch (CURRENTENEMYPROJECTILE.Type)
				{
				case 2:
					ApplySurface(CURRENTENEMYPROJECTILE.WorldX - CameraX, CURRENTENEMYPROJECTILE.WorldY - CameraY, ShipProjectile, Screen);
					break;
				};
			}
		}

		if (CURRENTENEMYPROJECTILE.WorldY > LevelHeight) CURRENTENEMYPROJECTILE.Active = false;

		if (!CURRENTENEMYPROJECTILE.Active)
		{
			EnemyProjectileVector.erase(EnemyProjectileVector.begin() + i);
		}
	}
}