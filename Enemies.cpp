#include"Enemies.h"
#include"CalculateProjectileAngle.h"
#include"GetXYRatio.h"
#include"Declarations.h"
#include"FloatText.h"
#include"DoEnemyProjectiles.h"
#include"MiscObject.h"
#include<time.h>
#include<math.h>

#define PI 3.1415

int BossStage = 0;
int BosNum1 = 0;
int BossHealth = 0;
double Multiplier = 0;
int Direction = 4;
int SFXTimer = 0;

float XDiff;
float YDiff;
SDL_Rect One;
SDL_Rect Two;
SDL_Rect Three;
SDL_Rect Four;
SDL_Rect Five;
SDL_Rect PlayerRectangle;
SDL_Rect BossHealthRect;

SDL_Rect *RenderRect = &TeleportClips[0];

std::vector <Pickup> PickupVector;
std::vector <Enemy> EnemyVector;

std::string BossName;

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
	Damage = 0;
	DamageFrames = 0;
	XOffset = 0;
	YOffset = 0;
	Moving = true;
	OnFire = false;
}

void Enemy::Gib()
{
	SDL_Surface* Sprite;

	switch (Type)
	{
	case 1:
		Sprite = Suicide;
		break;
	case 2:
		Sprite = Gunman;
		break;
	case 3:
		Sprite = Serious;
		break;
	case 7:
		Sprite = DoorGuard;
		break;
	case 8:
		Sprite = Spawner;
		break;
	case 10:
		Sprite = Frog;
		break;
	case 11:
		Sprite = VertLaser;
		break;
	case 12:
		Sprite = Biggie;
		break;
	case 13:
		Sprite = Warden;
		break;
	case 14:
		Sprite = Warkid;
		break;
	case 15:
		Sprite = Turret;
		break;
	case 16:
		Sprite = Warden;
		break;
	default:
		return;
	}

	SDL_Rect Part;
	Part.x = 0;
	Part.y = 0;
	Part.w = Sprite->w / 2 - 1;
	Part.h = Sprite->h / 3;
	AddObject(WorldX, WorldY, Sprite, -rand() % 12, -rand() % 12, 100, Part);

	Part.x = Part.w;
	AddObject(WorldX, WorldY, Sprite, rand() % 12, -rand() % 12, 130, Part);

	Part.x = 0;
	Part.y = Part.h;
	Part.h = 2 * Sprite->h / 3 - 1;
	AddObject(WorldX, WorldY, Sprite, -rand() % 12, rand() % 12, 120, Part);

	Part.x = Part.w;
	AddObject(WorldX, WorldY, Sprite, rand() % 12, rand() % 12, 140, Part);
}

void Enemy::StayInLevel()
{
	if (WorldX < 20) WorldX = 20;
	else if (WorldX + CollisionRect.w > Camera.LevelWidth - 20) WorldX = Camera.LevelWidth - (CollisionRect.w + 20);
	if (WorldY < 20) WorldY = 20;
	else if (WorldY + CollisionRect.h > Camera.LevelHeight - 20) WorldY = Camera.LevelHeight - (CollisionRect.h + 20);
}

bool Enemy::IsNotActive() const
{
	return (Health <= 0);
}

void Enemy::Bleed(int ProjectileXVel, int ProjectileYVel, int ex, int why)
{
	for (int i = 0; i < 3; i++)
	{
		int Rand1 = rand() % 16 - 8;
		int Rand2 = rand() % 16 - 8;
		CreateDebris(3, 1, ex + 15, why + 15, ProjectileXVel + Rand1, ProjectileYVel + Rand2, 0x262626);
	}
}

void Enemy::BulletPattern(int Type)
{
	EnemyProjectile Shot(3);
	switch (Type)
	{
	case 1: //Spiral
		Shot.Type = 3;
		Shot.WorldX = WorldX;
		Shot.WorldY = WorldY;
		Shot.Damage = 50;
		Shot.CollisionRect.w = 30;
		Shot.CollisionRect.h = 30;

		for (int i = 10; i <= 360; i += 20)
		{
			GetXYRatio(&Shot.XVel, &Shot.YVel, i, 3 + i / 26);
			EnemyProjectileVector.push_back(Shot);
		}
		break;

	case 2: //8 shots diagonal
		for (int i = 45; i < 360; i += 90) Shoot(3, i);
		for (int i = 0; i < 360; i += 90) Shoot(4, i);
		break;

	case 3: //8 shot
		for (int i = 0; i < 360; i += 45) Shoot(3, i);
		break;

	case 4: //Plain 4 directional shot
		for (int a = 0; a <= 270; a += 90) Shoot(3, a);
		break;

	case 5: //Cool sine wave rotating thing
		for (int i = 0; i < 360; i += 30) Shoot(9, i);
		break;

	case 6: //Gunmen and missiles
		for (int i = 0; i < 360; i += 180) Shoot(5, i);
		for (int i = 0; i < 360; i += 90) Shoot(6, i);
		break;
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
		int Distance, XDiff, YDiff;
		for (int i = 0; i < PickupVector.size(); i++)
		{
			XDiff = CURRENTPICKUPX - PlayerRect.x;
			YDiff = CURRENTPICKUPY - PlayerRect.y;
			Distance = sqrt(XDiff * XDiff + YDiff * YDiff);

			bool Collected = (Distance < 36);

			TempRect.x = CURRENTPICKUPX;
			TempRect.y = CURRENTPICKUPY;

			if (Distance < 150 && Distance != 0)
			{
				if (XDiff > 0) CURRENTPICKUPX -= 230 / Distance;
				else CURRENTPICKUPX += 230 / Distance;

				if (YDiff > 0) CURRENTPICKUPY -= 230 / Distance;
				else CURRENTPICKUPY += 230 / Distance;
			}

			switch (CURRENTPICKUPTYPE)
			{
			case 1: //Shotgun
				TempRect.w = Shotgun->w;
				TempRect.h = Shotgun->h;
				ApplySurface(CURRENTPICKUPX - CameraX, CURRENTPICKUPY - CameraY, Shotgun, Screen);
				if (Collected)
				{
					Ammo[1] += 8;
					FloatSomeText(PlayerRect.x, PlayerRect.y - 20, "Shotgun +8", White);
					PickupVector.erase(PickupVector.begin() + i);
				}
				break;

			case 2: //MachineGun
				TempRect.w = MachineGun->w;
				TempRect.h = MachineGun->h;
				ApplySurface(CURRENTPICKUPX - CameraX, CURRENTPICKUPY - CameraY, MachineGun, Screen);
				if (Collected)
				{
					Ammo[2] += 40;
					FloatSomeText(PlayerRect.x, PlayerRect.y - 20, "Machinegun +40", White);
					PickupVector.erase(PickupVector.begin() + i);
				}
				break;
			case 3: //Health
				TempRect.w = Health->w;
				TempRect.h = Health->h;
				ApplySurface(CURRENTPICKUPX - CameraX, CURRENTPICKUPY - CameraY, Health, Screen);
				if (Collected)
				{
					Damaged = true;
					DamageDealt = -10;
					FloatSomeText(PlayerRect.x, PlayerRect.y - 20, "10", Green);
					PickupVector.erase(PickupVector.begin() + i);
				}
				break;

			case 4: //FlameThrower
				TempRect.w = MachineGun->w;
				TempRect.h = MachineGun->h;
				ApplySurface(CURRENTPICKUPX - CameraX, CURRENTPICKUPY - CameraY, Flamethrower, Screen);
				if (Collected)
				{
					Ammo[3] += 220;
					FloatSomeText(PlayerRect.x, PlayerRect.y - 20, "Flamethrower +220", White);
					PickupVector.erase(PickupVector.begin() + i);
				}
				break;

			case 5: //Laser SMG
				TempRect.w = LaserPickup->w;
				TempRect.h = LaserPickup->h;
				ApplySurface(CURRENTPICKUPX - CameraX, CURRENTPICKUPY - CameraY, LaserPickup, Screen);
				if (Collected)
				{
					Ammo[4] += 80;
					FloatSomeText(PlayerRect.x, PlayerRect.y - 20, "Laser SMG +80", White);
					PickupVector.erase(PickupVector.begin() + i);
				}
				break;

			case 6: //RPG
				TempRect.w = Rocket->w;
				TempRect.h = Rocket->h;
				ApplySurface(CURRENTPICKUPX - CameraX, CURRENTPICKUPY - CameraY, Rocket, Screen);
				if (Collected)
				{
					Ammo[5] += 2;
					FloatSomeText(PlayerRect.x, PlayerRect.y - 20, "RPG +2", White);
					PickupVector.erase(PickupVector.begin() + i);
				}
				break;

			case 7: //Laser Shotgun
				TempRect.w = LaserShotgun->w;
				TempRect.h = LaserShotgun->h;
				ApplySurface(CURRENTPICKUPX - CameraX, CURRENTPICKUPY - CameraY, LaserShotgun, Screen);
				if (Collected)
				{
					Ammo[6] += 20;
					FloatSomeText(PlayerRect.x, PlayerRect.y - 20, "Automatic Laser Shotgun +20", White);
					PickupVector.erase(PickupVector.begin() + i);
				}
				break;

			case 8: //Grenade machinegun
				TempRect.w = Grenade->w;
				TempRect.h = Grenade->h;
				ApplySurface(CURRENTPICKUPX - CameraX, CURRENTPICKUPY - CameraY, Grenade, Screen);
				if (Collected)
				{
					Ammo[7] += 10;
					FloatSomeText(PlayerRect.x, PlayerRect.y - 20, "Automatic Grenade Machinegun +10", White);
					PickupVector.erase(PickupVector.begin() + i);
				}
				break;

			case 9: //Minigun
				TempRect.w = Grenade->w;
				TempRect.h = Grenade->h;
				ApplySurface(CURRENTPICKUPX - CameraX, CURRENTPICKUPY - CameraY, Minigun, Screen);
				if (Collected)
				{
					Ammo[8] += 500;
					FloatSomeText(PlayerRect.x, PlayerRect.y - 20, "Minigun +500", White);
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
	for (int i = 0; i < Enemus.size(); i += 3)
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
	SFXTimer++;

	for (int i = 0; i < EnemyVector.size(); i++)
	{
		if (CURRENTENEMY.OnFire)
		{
			CURRENTENEMY.BurnTimer--;
			if (CURRENTENEMY.BurnTimer % 10 == 0)
			{
				CURRENTENEMY.Health -= 3;
				CURRENTENEMY.Damage += 3;
				CURRENTENEMY.XOffset = rand() % CURRENTENEMY.CollisionRect.w;
				CURRENTENEMY.YOffset = rand() % CURRENTENEMY.CollisionRect.h;
				for (int w = 0; w <= 4; w++) CreateDebris(4, 1, CURRENTENEMY.WorldX + rand() % CURRENTENEMY.CollisionRect.w, CURRENTENEMY.WorldY + rand() % CURRENTENEMY.CollisionRect.h, rand() % 5 - 10, rand() % 5 - 10, 0xFF6A00);
			}

			if (CURRENTENEMY.BurnTimer == 0) CURRENTENEMY.OnFire = false;
		}

		CURRENTENEMY.CollisionRect.x = CURRENTENEMY.WorldX - CameraX;
		CURRENTENEMY.CollisionRect.y = CURRENTENEMY.WorldY - CameraY;
		if (CURRENTENEMY.Type != 0)
		{
			for (int x = 0; x < ProjectileVector.size() && ProjectileVector.at(x).Active && CURRENTENEMY.Health > 0; x++)
			{
				ProjectileVector.at(x).ProjectileRect.x = ProjectileVector.at(x).WorldX - CameraX;
				ProjectileVector.at(x).ProjectileRect.y = ProjectileVector.at(x).WorldY - CameraY;

				if (IsIntersecting(CURRENTENEMY.CollisionRect, ProjectileVector.at(x).ProjectileRect))
				{
					CURRENTENEMY.Bleed(ProjectileVector.at(x).XInc, ProjectileVector.at(x).YInc, ProjectileVector.at(x).WorldX, ProjectileVector.at(x).WorldY);
					DebugWindow(std::to_string(CURRENTENEMY.Health));

					if (ProjectileVector.at(x).Burning && rand() % 4 == 1)
					{
						CURRENTENEMY.OnFire = true;
						CURRENTENEMY.BurnTimer = 230;
					}

					if (ProjectileVector.at(x).Type != 3 && ProjectileVector.at(x).Type != 4)
					{
						Mix_PlayChannel(-1, MetalSounds.at(rand() % 23), 0);
					}

					if (ProjectileVector.at(x).Type == 5 || ProjectileVector.at(x).Type == 6)
					{
						CreateExplosion(ProjectileVector.at(x).WorldX - 2 * ProjectileVector.at(x).XInc, ProjectileVector.at(x).WorldY - 2 *ProjectileVector.at(x).YInc);
					}

					if (CURRENTENEMY.Moving & ProjectileVector.at(x).Damage <= 100)
					{
						CURRENTENEMY.WorldX += (ProjectileVector.at(x).Damage / 8) * ProjectileVector.at(x).XInc / 3;
						CURRENTENEMY.WorldY += (ProjectileVector.at(x).Damage / 8) * ProjectileVector.at(x).YInc / 3;
					}

					CURRENTENEMY.Health -= ProjectileVector.at(x).Damage;
					CURRENTENEMY.Damage += ProjectileVector.at(x).Damage;
					CURRENTENEMY.XOffset = ProjectileVector.at(x).WorldX - CURRENTENEMY.WorldX - 5;
					CURRENTENEMY.YOffset = ProjectileVector.at(x).WorldY - CURRENTENEMY.WorldY - 5;
					ProjectileVector.at(x).Active = false;
					CURRENTENEMY.DamageFrames = 100;
				}
			}
		}

		else //Teleport
		{
			if (CURRENTENEMY.Frametime < 15)
			{
				CURRENTENEMY.Frametime++;
				ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, TeleportSheet, Screen, &TeleportClips[CURRENTENEMY.Frametime % 2]);
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
					switch (Temp.Type)
					{
					case 1: //suicide
						Temp.Health = 50;
						Temp.Speed = rand() % 6 + 5;
						Temp.CollisionRect.w = Suicide->w;
						Temp.CollisionRect.h = Suicide->h;
						break;

					case 2: //Gunman
						Temp.Health = 60;
						Temp.ShotCounter = rand() % 30 - 80;
						Temp.Speed = rand() % 2 + 4;
						Temp.CollisionRect.w = Gunman->w;
						Temp.CollisionRect.h = Gunman->h;

						Temp.Frame = Temp.WorldX;
						Temp.Frametime = Temp.WorldY;

						Temp.Timer = rand() % 70;
						break;

					case 3: //Beheaded Serious Sam guy
						Temp.Health = 50;
						Temp.Speed = rand() % 5 + 5;
						Temp.CollisionRect.w = Serious->w;
						Temp.CollisionRect.h = Serious->h;
						Temp.ShotCounter = 60;
						break;

					case 4: //SIS Boss
						Temp.Health = 1500;
						Temp.Speed = 5;
						Temp.CollisionRect.w = 50;
						Temp.CollisionRect.h = 20;
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
						Temp.Health = 500;
						Temp.CollisionRect.w = 200;
						Temp.CollisionRect.h = 50;
						Temp.Moving = false;
						break;

					case 8: //Spawner
						Temp.Health = 100;
						Temp.Speed = 2;
						Temp.CollisionRect.w = 25;
						Temp.CollisionRect.h = 25;
						break;

					case 9: //MARS
						Temp.Health = 14000;
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
						break;

					case 11: //Vertical laser 
						Temp.Health = 30;
						Temp.Moving = false;
						Temp.CollisionRect.w = 50;
						Temp.CollisionRect.h = 20;
						Temp.ShotCounter = rand() % 50;
						break;

					case 12: //Biggie;
						Temp.Health = 2000;
						Temp.CollisionRect.w = 100;
						Temp.CollisionRect.h = 130;

						Temp.Frame = Temp.WorldX;
						Temp.Frametime = Temp.WorldY;

						Temp.Moving = false;
						break;

					case 13: //Warden;
						Temp.Health = 5500;
						Temp.CollisionRect.w = 300;
						Temp.CollisionRect.h = 300;

						Temp.Frametime = 1;

						Boss = true;
						BossName = "W.A.R DEN:";
						Multiplier = (float)(ScreenWidth - 50) / Temp.Health;
						Temp.Moving = false;
						break;

					case 14: //Warkid;
						Temp.Health = 5000;
						Temp.CollisionRect.w = 100;
						Temp.CollisionRect.h = 100;

						Temp.Frametime = 1;

						Temp.Moving = false;
						break;

					case 15: //Turret
						Temp.Health = 250;
						Temp.CollisionRect.w = 30;
						Temp.CollisionRect.h = 30;
						Temp.Moving = false;
						break;

					case 16: //Rush warden
						Temp.Health = 3000;
						Temp.CollisionRect.w = 300;
						Temp.CollisionRect.h = 300;

						Temp.Frametime = 1;
						Temp.Moving = false;
						break;

					case 17: //Rush SIS
						Temp.Health = 800;
						Temp.CollisionRect.w = 50;
						Temp.CollisionRect.h = 20;

						Temp.Angle = Temp.WorldX;
						Temp.Timer = Temp.WorldY;

						Boss = true;
						BossName = "SIS:";
						Multiplier = (float)(ScreenWidth - 50) / Temp.Health;
						break;

					case 18: //Rush MARS
						Temp.CollisionRect.w = 40;
						Temp.CollisionRect.h = 100;
						Temp.Health = 5000;
						Boss = true;
						BossStage = 0;
						BossName = "M.A.R.S:";
						Multiplier = (float)(ScreenWidth - 50) / Temp.Health;
						break;

					case 19: //ADXERS left arm
						Temp.CollisionRect.w = 90;
						Temp.CollisionRect.h = 90;
						Temp.ShotCounter = 0;
						Temp.Frame = PlayerX;
						Temp.Frametime = PlayerY;
						Temp.Speed = 12;
						Temp.Health = 3000;
						Boss = true;
						BossStage = 0;
						BossName = "ADXERS:";
						Multiplier = (float)(ScreenWidth - 50) / 6000;
						break;

					case 20: //ADXERS right arm
						Temp.CollisionRect.w = 90;
						Temp.CollisionRect.h = 90;
						Temp.ShotCounter = 0;
						Temp.Angle = 226;
						Temp.Timer = -1;
						Temp.Frame = PlayerX + 500;
						Temp.Frametime = PlayerY;
						Temp.Speed = 10;
						Temp.Health = 3000;
						break;

					case 21: //ADXERS
						BossStage = 0;
						Temp.Health = 20000;
						Temp.ShotCounter = 0;
						Temp.Angle = 0;
						Temp.Timer = 0;
						Temp.Frame = 2;
						Temp.Frametime = 2;
						Temp.CollisionRect.w = 60;
						Temp.CollisionRect.h = 100;
						Temp.Moving = false;
						ADXERSFlag = 1;
						break;

					};
					EnemyVector.erase(EnemyVector.begin() + i, EnemyVector.begin() + i + 1);
					EnemyVector.push_back(Temp);
					continue;
				}
			}
		}

		if (Laser && CURRENTENEMY.Type != 0 && CURRENTENEMY.WorldY + CURRENTENEMY.CollisionRect.h > LaserY)
		{
			CURRENTENEMY.Health = 0;
		}

		float XDiff;
		float YDiff;
		float Angle;
		SDL_Surface *ApplyThis;
		int Distance;

		switch (CURRENTENEMY.Type)
		{
		case 1: //Suicide
			if (IsIntersecting(PlayerRect, CURRENTENEMY.CollisionRect))
			{
				if (Invincible == false)
				{
					Damaged = true;
					DamageDealt = 30;
					CreateDebris(5, 5, CURRENTENEMY.WorldX, CURRENTENEMY.WorldY, CURRENTENEMY.XVel * 2, CURRENTENEMY.YVel * 2, 0xFFFFFF);
					EnemyVector.erase(EnemyVector.begin() + i, EnemyVector.begin() + i + 1);
					Enemies--;
					continue;
				}
			}

			Angle = CalculateProjectileAngle(CURRENTENEMY.WorldX + XVel * (CURRENTENEMY.Speed / 4), CURRENTENEMY.WorldY + YVel * (CURRENTENEMY.Speed / 4), PlayerX, PlayerY);
			GetXYRatio(&XDiff, &YDiff, Angle, CURRENTENEMY.Speed / 2);
			CURRENTENEMY.XVel += XDiff / 2;
			CURRENTENEMY.YVel += YDiff / 2;

			if (CURRENTENEMY.XVel > CURRENTENEMY.Speed) CURRENTENEMY.XVel = CURRENTENEMY.Speed;
			else if (CURRENTENEMY.XVel < CURRENTENEMY.Speed * -1) CURRENTENEMY.XVel = CURRENTENEMY.Speed * -1;
			if (CURRENTENEMY.YVel > CURRENTENEMY.Speed) CURRENTENEMY.YVel = CURRENTENEMY.Speed;
			else if (CURRENTENEMY.YVel < CURRENTENEMY.Speed * -1) CURRENTENEMY.YVel = CURRENTENEMY.Speed * -1;
			CURRENTENEMY.WorldX += CURRENTENEMY.XVel;
			CURRENTENEMY.WorldY += CURRENTENEMY.YVel;

			CURRENTENEMY.StayInLevel();

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, Suicide, Screen);
			break;

		case 2: //Gunman
			CURRENTENEMY.Timer++;
			CURRENTENEMY.ShotCounter++;

			if (IsIntersecting(PlayerRect, CURRENTENEMY.CollisionRect))
			{
				Damaged = true;
				DamageDealt = 20;
				continue;
			}

			if (CURRENTENEMY.ShotCounter > 0 && CURRENTENEMY.ShotCounter % 20 == 0)
			{
				CURRENTENEMY.Shoot(3, PlayerX, PlayerY);

				if (CURRENTENEMY.ShotCounter > 40) CURRENTENEMY.ShotCounter = -80;
			}

			if (CURRENTENEMY.Timer > 70)
			{
				CURRENTENEMY.Timer = 0;

				CURRENTENEMY.Frame = PlayerX + rand() % 900 - 450;
				CURRENTENEMY.Frametime = PlayerY + rand() % 900 - 450;
			}

			CURRENTENEMY.Frame += XVel;
			CURRENTENEMY.Frametime += YVel;

			CURRENTENEMY.XVel = (CURRENTENEMY.Frame - CURRENTENEMY.WorldX) / 35;
			CURRENTENEMY.YVel = (CURRENTENEMY.Frametime - CURRENTENEMY.WorldY) / 35;

			if (CURRENTENEMY.Frametime > CURRENTENEMY.WorldY) CURRENTENEMY.YVel += 3;
			else CURRENTENEMY.YVel -= 3;

			CURRENTENEMY.WorldX += CURRENTENEMY.XVel;
			CURRENTENEMY.WorldY += CURRENTENEMY.YVel;

			CURRENTENEMY.StayInLevel();

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, Gunman, Screen);
			break;

		case 3: //Serious sam beheaded kamakazie
			Angle = CalculateProjectileAngle(CURRENTENEMY.WorldX + XVel * (CURRENTENEMY.Speed / 4), CURRENTENEMY.WorldY + YVel * (CURRENTENEMY.Speed / 4), PlayerX, PlayerY);
			GetXYRatio(&XDiff, &YDiff, Angle, CURRENTENEMY.Speed);
			CURRENTENEMY.XVel = XDiff;
			CURRENTENEMY.YVel = YDiff;
			CURRENTENEMY.WorldX += XDiff;
			CURRENTENEMY.WorldY += YDiff;
			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, Serious, Screen);

			Distance = sqrt((CURRENTENEMY.WorldX - PlayerX) * (CURRENTENEMY.WorldX - PlayerX) + (CURRENTENEMY.WorldY - PlayerY) * (CURRENTENEMY.WorldY - PlayerY));
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
					CURRENTENEMY.Shoot(1, May);
				}
				CURRENTENEMY.Health = 0;
			}
			break;

		case 4: //SIS Boss
			BossHealth = CURRENTENEMY.Health;

			if (IsIntersecting(PlayerRect, CURRENTENEMY.CollisionRect))
			{
				Damaged = true;
				DamageDealt = 50;
			}

			CURRENTENEMY.ShotCounter++;
			if (CURRENTENEMY.ShotCounter == 90) { CURRENTENEMY.Shoot(2, 0, 0); CURRENTENEMY.ShotCounter = 60; }

			float YAcceleration;
			int YMaxSpeed;

			if (InBetween(CURRENTENEMY.WorldX - 20, PlayerX, CURRENTENEMY.WorldX + CURRENTENEMY.CollisionRect.w + 20))
			{
				YAcceleration = 2;
				YMaxSpeed = 12;
			}

			else
			{
				YAcceleration = 0.5;
				YMaxSpeed = 5;
			}

			if (CURRENTENEMY.WorldY < PlayerY)
			{
				if (CURRENTENEMY.YVel + YAcceleration + CURRENTENEMY.WorldY > PlayerY) CURRENTENEMY.YVel = PlayerY - CURRENTENEMY.WorldY;
				else CURRENTENEMY.YVel += YAcceleration;
			}

			else if (CURRENTENEMY.WorldY > PlayerY)
			{
				if (CURRENTENEMY.YVel - YAcceleration + CURRENTENEMY.WorldY < PlayerY) CURRENTENEMY.YVel = CURRENTENEMY.WorldY - PlayerY;
				else CURRENTENEMY.YVel -= YAcceleration;
			}

			if (CURRENTENEMY.WorldX < (PlayerX - 300))
			{
				if (CURRENTENEMY.XVel + 2.5 + CURRENTENEMY.WorldX > (PlayerX - 300)) CURRENTENEMY.XVel = 0;
				else CURRENTENEMY.XVel += 2.5;
			}

			if (CURRENTENEMY.WorldX >(PlayerX - 300))
			{
				if (CURRENTENEMY.XVel - 10 + CURRENTENEMY.WorldX < (PlayerX - 300)) CURRENTENEMY.XVel = 0;
				else CURRENTENEMY.XVel -= 10;
			}

			if (CURRENTENEMY.XVel > 5) CURRENTENEMY.XVel = 5;
			else if (CURRENTENEMY.XVel < -10) CURRENTENEMY.XVel = -10;
			if (CURRENTENEMY.YVel > YMaxSpeed) CURRENTENEMY.YVel = YMaxSpeed;
			else if (CURRENTENEMY.YVel < -YMaxSpeed) CURRENTENEMY.YVel = -YMaxSpeed;

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

			ApplyThis = Ship;
			ApplyThis->w = 50;
			ApplyThis->h = 20;
			ApplySurface(0, 0, Ship, ApplyThis, &ShipFrames[CURRENTENEMY.Frame]);
			ApplyThis = rotozoomSurface(ApplyThis, CURRENTENEMY.YVel * 2, 1, 0);
			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, ApplyThis, Screen);
			break;

		case 5: //SIS Invader
			CURRENTENEMY.WorldX -= CURRENTENEMY.Speed;

			if (IsIntersecting(CURRENTENEMY.CollisionRect, PlayerRect) && Invincible == false)
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
			break;

		case 6: //SIS Worm
			CURRENTENEMY.WorldX += CURRENTENEMY.Speed;
			CURRENTENEMY.CollisionRect.x = CURRENTENEMY.WorldX - Camera.x;

			if (IsIntersecting(CURRENTENEMY.CollisionRect, PlayerRect) && Invincible == false)
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
				CURRENTENEMY.Shoot(2, 0, 0);
			}

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, Worm, Screen, &InvaderFrames[CURRENTENEMY.Frame]);
			break;

		case 7: //DoorGuard
			if (IsIntersecting(CURRENTENEMY.CollisionRect, PlayerRect))
			{
				CURRENTENEMY.Health = 0;
				Invincible = false;
				Damaged = true;
				DamageDealt = 100;
			}

			CURRENTENEMY.ShotCounter++;

			if (CURRENTENEMY.ShotCounter % 30 == 0)
			{
				CURRENTENEMY.Shoot(1, PlayerX, PlayerY);
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
				for (int mem = 0; mem <= 180; mem += 45) CURRENTENEMY.Shoot(4, 90 + mem);
			}

			else if (CURRENTENEMY.ShotCounter == 350)
			{
				for (int mem = 0; mem <= 180; mem += 30) CURRENTENEMY.Shoot(3, 90 + mem);
				for (int mem = 0; mem <= 180; mem += 60) CURRENTENEMY.Shoot(4, 90 + mem);
				for (int mem = 0; mem <= 180; mem += 15) CURRENTENEMY.Shoot(1, 90 + mem);
				CURRENTENEMY.ShotCounter = 0;
			}

			if (IsIntersecting(CURRENTENEMY.CollisionRect, PlayerRect))
			{
				Damaged = true;
				DamageDealt = 100;
			}

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, DoorGuard, Screen);
			break;

		case 8: //Spawner
			Angle = CalculateProjectileAngle(CURRENTENEMY.WorldX + XVel * (CURRENTENEMY.Speed / 4), CURRENTENEMY.WorldY + YVel * (CURRENTENEMY.Speed / 4), PlayerX, PlayerY);
			GetXYRatio(&XDiff, &YDiff, Angle, CURRENTENEMY.Speed);
			CURRENTENEMY.XVel = XDiff;
			CURRENTENEMY.YVel = YDiff;
			CURRENTENEMY.WorldX += XDiff;
			CURRENTENEMY.WorldY += YDiff;

			CURRENTENEMY.ShotCounter++;
			if (CURRENTENEMY.ShotCounter > 300)
			{
				CURRENTENEMY.Shoot(5, PlayerX, PlayerY);
				CURRENTENEMY.ShotCounter = 0;
			}

			if (IsIntersecting(PlayerRect, CURRENTENEMY.CollisionRect))
			{
				Damaged = true;
				DamageDealt = 20;
			}

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, Spawner, Screen);
			break;

		case 9: //MARS 
			BossHealth = CURRENTENEMY.Health;

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

				if (CURRENTENEMY.ShotCounter == 9)
				{
					CURRENTENEMY.ShotCounter = 0;
					CURRENTENEMY.CollisionRect.h = 0;
					CURRENTENEMY.Shoot(3, 270 + BosNum1);
					CURRENTENEMY.CollisionRect.h = 100;

					if (BosNum1 > 160 || BosNum1 < 20) Direction *= -1;
				}

				XDiff = abs(((PlayerX - 15) - CURRENTENEMY.WorldX) / 20);
				YDiff = abs(((PlayerY + 210) - CURRENTENEMY.WorldY) / 12);

				if (CURRENTENEMY.WorldX < (PlayerX - 15)) CURRENTENEMY.WorldX += XDiff;
				else if (CURRENTENEMY.WorldX > (PlayerX - 15)) CURRENTENEMY.WorldX -= XDiff;
				if (CURRENTENEMY.WorldY < PlayerY + 210) CURRENTENEMY.WorldY += YDiff;
				else if (CURRENTENEMY.WorldY > PlayerY + 210) CURRENTENEMY.WorldY -= YDiff;

				if (CURRENTENEMY.WorldY + 250 > LevelHeight - 20) CURRENTENEMY.WorldY = LevelHeight - 270;
				if (CURRENTENEMY.WorldX - 105 < 20) CURRENTENEMY.WorldX = 125;
				else if (CURRENTENEMY.WorldX + 145 > LevelWidth - 20) CURRENTENEMY.WorldX = LevelWidth - 165;

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
				SDL_FillRect(Screen, RenderRect, 0xFFFFFF);
				*RenderRect = Two;
				SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
				*RenderRect = Three;
				SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
				*RenderRect = Four;
				SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
				*RenderRect = Five;
				SDL_FillRect(Screen, RenderRect, 0xC0C0C0);

				PlayerRectangle.x = PlayerX - Camera.x;
				PlayerRectangle.y = PlayerY - Camera.y;
				PlayerRectangle.w = 15;
				PlayerRectangle.h = 30;

				if (IsIntersecting(PlayerRectangle, One) || IsIntersecting(PlayerRectangle, Two) || IsIntersecting(PlayerRectangle, Three) || IsIntersecting(PlayerRectangle, Four))
				{
					Damaged = true;
					DamageDealt = 50;
				}

				if (CURRENTENEMY.Health <= 10000)
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

					CreateDebris(10, 6, CURRENTENEMY.WorldX + 10, CURRENTENEMY.WorldY + 10, 0, 0, 0xFFFFFF);
				}
				break;

			case 2:
				if (CURRENTENEMY.XVel >= 0) CURRENTENEMY.XVel += 0.1;
				else CURRENTENEMY.XVel -= 0.1;
				if (CURRENTENEMY.YVel >= 0) CURRENTENEMY.YVel += 0.1;
				else CURRENTENEMY.YVel -= 0.1;

				if (CURRENTENEMY.WorldX + CURRENTENEMY.XVel + 100 > LevelWidth - 20 || CURRENTENEMY.WorldX + CURRENTENEMY.XVel - 150 < 20)
				{
					Shake = true;
					Mag = 20;
					Dur = 30;
					CURRENTENEMY.XVel *= -0.5;
				}

				if (CURRENTENEMY.WorldY + CURRENTENEMY.YVel + 140 > LevelHeight - 20 || CURRENTENEMY.WorldY + CURRENTENEMY.YVel - 105 < 20)
				{
					Shake = true;
					Mag = 20;
					Dur = 30;
					CURRENTENEMY.YVel *= -0.5;
				}

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
				SDL_FillRect(Screen, RenderRect, 0xFFFFFF);
				*RenderRect = Three;
				SDL_FillRect(Screen, RenderRect, 0xFFFFFF);
				*RenderRect = Four;
				SDL_FillRect(Screen, RenderRect, 0xFFFFFF);
				*RenderRect = Five;
				SDL_FillRect(Screen, RenderRect, 0xFFFFFF);

				PlayerRectangle.x = PlayerX - Camera.x;
				PlayerRectangle.y = PlayerY - Camera.y;
				PlayerRectangle.w = 15;
				PlayerRectangle.h = 30;

				if (IsIntersecting(PlayerRectangle, Two) || IsIntersecting(PlayerRectangle, Three) || IsIntersecting(PlayerRectangle, Four))
				{
					Damaged = true;
					DamageDealt = 50;
				}

				CURRENTENEMY.ShotCounter++;
				if (CURRENTENEMY.ShotCounter > 600) 
				{
					BossStage = 3;
					BossTheme = Mix_LoadMUS("Resources/Sounds/Music/Two.ogg");
					Mix_PlayMusic(BossTheme,-1);
				}
				break;

			case 3:
				CURRENTENEMY.ShotCounter++;
				if (CURRENTENEMY.ShotCounter % 15 == 0) CURRENTENEMY.Shoot(1, 90);
				if (CURRENTENEMY.ShotCounter > 250)
				{
					CURRENTENEMY.ShotCounter = 210;
					CURRENTENEMY.CollisionRect.w = 200;
					CURRENTENEMY.Shoot(3, 45);
					CURRENTENEMY.Shoot(3, 135);
					CURRENTENEMY.Shoot(3, 90);
					CURRENTENEMY.CollisionRect.w = 100;

					BosNum1++;
					if (BosNum1 > 5)
					{
						BosNum1 = 0;
						CURRENTENEMY.BulletPattern(1);
						CURRENTENEMY.ShotCounter = 0;
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
				SDL_FillRect(Screen, RenderRect, 0xFFFFFF);
				*RenderRect = Three;
				SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
				*RenderRect = Four;
				SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
				*RenderRect = Five;
				SDL_FillRect(Screen, RenderRect, 0xC0C0C0);

				PlayerRectangle.x = PlayerX - Camera.x;
				PlayerRectangle.y = PlayerY - Camera.y;
				PlayerRectangle.w = 15;
				PlayerRectangle.h = 30;

				if (IsIntersecting(PlayerRectangle, Two) || IsIntersecting(PlayerRectangle, Three) || IsIntersecting(PlayerRectangle, Four))
				{
					Damaged = true;
					DamageDealt = 50;
				}

				if (CURRENTENEMY.Health <= 7000)
				{
					BossStage = 4;

					CURRENTENEMY.Shoot(5, 45);
					CURRENTENEMY.Shoot(5, 135);
					CURRENTENEMY.Shoot(5, 90);

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

					CreateDebris(10, 6, CURRENTENEMY.WorldX + 10, CURRENTENEMY.WorldY + 10, 0, 0, 0xFFFFFF);
				}
				break;

			case 4:
				if (CURRENTENEMY.XVel >= 0) CURRENTENEMY.XVel += 0.1;
				else CURRENTENEMY.XVel -= 0.1;
				if (CURRENTENEMY.YVel >= 0) CURRENTENEMY.YVel += 0.1;
				else CURRENTENEMY.YVel -= 0.1;

				if (CURRENTENEMY.WorldX + CURRENTENEMY.XVel + 40 > LevelWidth - 20 || CURRENTENEMY.WorldX + CURRENTENEMY.XVel - 105 < 20)
				{
					Shake = true;
					Mag = 20;
					Dur = 30;
					CURRENTENEMY.XVel *= -0.7;
				}

				if (CURRENTENEMY.WorldY + CURRENTENEMY.YVel + 100 > LevelHeight - 20 || CURRENTENEMY.WorldY + CURRENTENEMY.YVel - 150 < 20)
				{
					Shake = true;
					Mag = 20;
					Dur = 30;
					CURRENTENEMY.YVel *= -0.7;
				}

				CURRENTENEMY.WorldX += CURRENTENEMY.XVel;
				CURRENTENEMY.WorldY += CURRENTENEMY.YVel;

				Three.x = CURRENTENEMY.WorldX - CameraX;
				Three.y = CURRENTENEMY.WorldY - CameraY;

				Five.x = Three.x + 15;
				Five.y = Three.y - 30;

				Four.x = Five.x - 120;
				Four.y = Five.y - 15;

				*RenderRect = Three;
				SDL_FillRect(Screen, RenderRect, 0xFFFFFF);
				*RenderRect = Four;
				SDL_FillRect(Screen, RenderRect, 0xFFFFFF);
				*RenderRect = Five;
				SDL_FillRect(Screen, RenderRect, 0xFFFFFF);

				PlayerRectangle.x = PlayerX - Camera.x;
				PlayerRectangle.y = PlayerY - Camera.y;
				PlayerRectangle.w = 15;
				PlayerRectangle.h = 30;

				if (IsIntersecting(PlayerRectangle, Three) || IsIntersecting(PlayerRectangle, Four))
				{
					Damaged = true;
					DamageDealt = 50;
				}

				CURRENTENEMY.ShotCounter++;
				if (CURRENTENEMY.ShotCounter > 600)
				{
					BossStage = 5;
					CURRENTENEMY.ShotCounter = 0;
					BossTheme = Mix_LoadMUS("Resources/Sounds/Music/Three.ogg");
					Mix_PlayMusic(BossTheme, -1);
				}

				break;

			case 5:
				CURRENTENEMY.ShotCounter++;

				if (CURRENTENEMY.ShotCounter % 30 == 0)
				{
					CURRENTENEMY.CollisionRect.h = 200;
					CURRENTENEMY.Shoot(1, PlayerX, PlayerY);
					CURRENTENEMY.CollisionRect.h = 100;
				}

				if (CURRENTENEMY.ShotCounter == 50)
				{
					CURRENTENEMY.CollisionRect.h = 200;
					for (int mem = 0; mem <= 360; mem += 30)
					{
						CURRENTENEMY.Shoot(3, 90 + mem);
					}
					CURRENTENEMY.CollisionRect.h = 100;
				}

				else if (CURRENTENEMY.ShotCounter == 100)
				{
					CURRENTENEMY.CollisionRect.h = 200;
					for (int mem = 0; mem <= 360; mem += 45) CURRENTENEMY.Shoot(4, 90 + mem);
					CURRENTENEMY.CollisionRect.h = 100;
				}

				else if (CURRENTENEMY.ShotCounter == 250)
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
				SDL_FillRect(Screen, RenderRect, 0xFFFFFF);
				*RenderRect = Four;
				SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
				*RenderRect = Five;
				SDL_FillRect(Screen, RenderRect, 0xC0C0C0);

				PlayerRectangle.x = PlayerX - Camera.x;
				PlayerRectangle.y = PlayerY - Camera.y;
				PlayerRectangle.w = 15;
				PlayerRectangle.h = 30;

				if (IsIntersecting(PlayerRectangle, Three) || IsIntersecting(PlayerRectangle, Four))
				{
					Damaged = true;
					DamageDealt = 50;
				}


				if (CURRENTENEMY.Health <= 3000)
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

					CreateDebris(10, 6, CURRENTENEMY.WorldX + 10, CURRENTENEMY.WorldY + 10, 0, 0, 0xFFFFFF);
				}
				break;

			case 6:
				if (CURRENTENEMY.XVel >= 0) CURRENTENEMY.XVel += 0.1;
				else CURRENTENEMY.XVel -= 0.1;
				if (CURRENTENEMY.YVel >= 0) CURRENTENEMY.YVel += 0.1;
				else CURRENTENEMY.YVel -= 0.1;

				if (CURRENTENEMY.WorldX + CURRENTENEMY.XVel + 130 > LevelWidth - 20 || CURRENTENEMY.WorldX + CURRENTENEMY.XVel < 20)
				{
					Shake = true;
					Mag = 20;
					Dur = 30;
					CURRENTENEMY.XVel *= -0.9;
				}

				if (CURRENTENEMY.WorldY + CURRENTENEMY.YVel + 40 > LevelHeight - 20 || CURRENTENEMY.WorldY + CURRENTENEMY.YVel < 20)
				{
					Shake = true;
					Mag = 20;
					Dur = 30;
					CURRENTENEMY.YVel *= -0.9;
				}
				CURRENTENEMY.WorldX += CURRENTENEMY.XVel;
				CURRENTENEMY.WorldY += CURRENTENEMY.YVel;

				Four.x = CURRENTENEMY.WorldX - CameraX;
				Four.y = CURRENTENEMY.WorldY - CameraY;

				Five.x = Four.x + 120;
				Five.y = Four.y + 15;

				*RenderRect = Four;
				SDL_FillRect(Screen, RenderRect, 0xFFFFFF);
				*RenderRect = Five;
				SDL_FillRect(Screen, RenderRect, 0xFFFFFF);

				PlayerRectangle.x = PlayerX - Camera.x;
				PlayerRectangle.y = PlayerY - Camera.y;
				PlayerRectangle.w = 15;
				PlayerRectangle.h = 30;

				if (IsIntersecting(PlayerRectangle, Four))
				{
					Damaged = true;
					DamageDealt = 50;
				}

				CURRENTENEMY.ShotCounter++;
				if (CURRENTENEMY.ShotCounter > 600)
				{
					BossStage = 7;
					CURRENTENEMY.ShotCounter = 0;
					BossTheme = Mix_LoadMUS("Resources/Sounds/Music/Four.ogg");
					Mix_PlayMusic(BossTheme, -1);
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
				if (CURRENTENEMY.ShotCounter % 15 == 0)
				{
					CURRENTENEMY.Shoot(3, rand() % 360);
					CURRENTENEMY.Shoot(3, PlayerX, PlayerY);
				}

				else if (CURRENTENEMY.ShotCounter == 60)
				{
					for (int x = 0; x < 360; x += 20) CURRENTENEMY.Shoot(3, x);
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
				SDL_FillRect(Screen, RenderRect, 0xFFFFFF);
				*RenderRect = Five;
				SDL_FillRect(Screen, RenderRect, 0xFFFFFF);

				PlayerRectangle.x = PlayerX - Camera.x;
				PlayerRectangle.y = PlayerY - Camera.y;
				PlayerRectangle.w = 15;
				PlayerRectangle.h = 30;

				if (IsIntersecting(PlayerRectangle, Four))
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
				/*if (CURRENTENEMY.WorldX + CURRENTENEMY.XVel + 10 > LevelWidth - 20 || CURRENTENEMY.WorldX + CURRENTENEMY.XVel < 20)
				{
					Shake = true;
					Mag = 20;
					Dur = 30;
					CURRENTENEMY.XVel *= -0.3;
				}

				if (CURRENTENEMY.WorldY + CURRENTENEMY.YVel + 10 > LevelHeight - 20 || CURRENTENEMY.WorldY + CURRENTENEMY.YVel < 20)
				{
					Shake = true;
					Mag = 20;
					Dur = 30;
					CURRENTENEMY.YVel *= -0.3;
				}*/

				CURRENTENEMY.ShotCounter++;

				if (CURRENTENEMY.ShotCounter % 10 == 0) CURRENTENEMY.Shoot(3, CURRENTENEMY.ShotCounter);

				if (CURRENTENEMY.ShotCounter == 100)
				{
					for (int w = 0; w < 360; w += 20) CURRENTENEMY.Shoot(3, PlayerX, PlayerY);
				}

				if (CURRENTENEMY.ShotCounter == 200)
				{
					for (int w = 0; w <= 270; w += 54) CURRENTENEMY.Shoot(3, w);
					CURRENTENEMY.ShotCounter = 0;
				}

				Five.x = CURRENTENEMY.WorldX - Camera.x;
				Five.y = CURRENTENEMY.WorldY - Camera.y;
				*RenderRect = Five;

				SDL_FillRect(Screen, RenderRect, 0xFF0000);

			}
			break;

		case 10: //Frog
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

			CURRENTENEMY.StayInLevel();

			if (IsIntersecting(CURRENTENEMY.CollisionRect, PlayerRect))
			{
				CURRENTENEMY.Health = 0;
				Damaged = true;
				DamageDealt = 40;
			}

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, Frog, Screen);
			break;

		case 11: //Lasers
			CURRENTENEMY.ShotCounter++;
			if (CURRENTENEMY.ShotCounter == 52)
			{
				CURRENTENEMY.Shoot(7, 0, 0);
				CURRENTENEMY.ShotCounter = -rand() % 20;
			}

			if (IsIntersecting(CURRENTENEMY.CollisionRect, PlayerRect))
			{
				CURRENTENEMY.Health = 0;
				Damaged = true;
				DamageDealt = 40;
			}

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, VertLaser, Screen);
			break;

		case 12: //Biggie;
			CURRENTENEMY.ShotCounter++;

			if (IsIntersecting(CURRENTENEMY.CollisionRect, PlayerRect))
			{
				Damaged = true;
				DamageDealt = 40;
			}

			if (CURRENTENEMY.ShotCounter > 210)
			{
				CURRENTENEMY.ShotCounter = 0;
				CURRENTENEMY.BulletPattern(rand() % 4 + 1);
			}

			else if (CURRENTENEMY.ShotCounter % 23 == 0)
			{
				int Angle = CalculateProjectileAngle(CURRENTENEMY.WorldX, CURRENTENEMY.WorldY, PlayerX, PlayerY);
				CURRENTENEMY.Shoot(3, Angle - 25);
				CURRENTENEMY.Shoot(3, Angle);
				CURRENTENEMY.Shoot(3, Angle + 25);
			}

			if (CURRENTENEMY.Health < 500 && CURRENTENEMY.ShotCounter % 4 == 0) CURRENTENEMY.Shoot(3, rand() % 360);

			if (abs(CURRENTENEMY.WorldX - CURRENTENEMY.Frame) > 300) CURRENTENEMY.XVel *= -1;
			if (abs(CURRENTENEMY.WorldY - CURRENTENEMY.Frametime) > 300) CURRENTENEMY.YVel *= -1;

			if (CURRENTENEMY.Timer > 0)
			{
				CURRENTENEMY.WorldX += CURRENTENEMY.XVel;
				CURRENTENEMY.WorldY += CURRENTENEMY.YVel;
				if (CURRENTENEMY.Timer > 90) CURRENTENEMY.Timer = -90;
			}

			else if (CURRENTENEMY.Timer == 0)
			{
				CURRENTENEMY.XVel = rand() % 6 - 3;
				CURRENTENEMY.YVel = rand() % 6 - 3;
			}

			CURRENTENEMY.Timer++;

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, Biggie, Screen);
			break;

		case 13: //WARDEN
			BossHealth = CURRENTENEMY.Health;
			CURRENTENEMY.ShotCounter++;

			if (CURRENTENEMY.ShotCounter == 1)
			{
				for (float o = 0; o < 6.28; o += 1.26)
				{
					for (int p = 100; p <= 1400; p += 70)
					{
						CURRENTENEMY.Shoot(11, OrbitX - 20 + p * cos(o), OrbitY - 20 + p * sin(o));
					}
				}
			}

			if (CURRENTENEMY.Health == 5000)
			{
				Mix_PlayMusic(BossTheme, -1);
				BossTheme = Mix_LoadMUS("Resources/Sounds/Music/Beat3.ogg");

				Shake = true;
				Mag = 10;
				Dur = 50;

				CURRENTENEMY.Health -= 20;
			}

			else if (CURRENTENEMY.Health == 3700)
			{
				Mix_PlayMusic(BossTheme, -1);
				BossTheme = Mix_LoadMUS("Resources/Sounds/Music/Beat4.ogg");

				Shake = true;
				Mag = 20;
				Dur = 30;

				CURRENTENEMY.Health -= 20;
			}

			else if (CURRENTENEMY.Health == 2500)
			{
				Mix_PlayMusic(BossTheme, -1);

				Shake = true;
				Mag = 20;
				Dur = 30;

				CURRENTENEMY.Frametime *= 2;
				CURRENTENEMY.Health -= 20;
			}

			else if (CURRENTENEMY.Health == 700)
			{
				Shake = true;
				Mag = 20;
				Dur = 30;

				CURRENTENEMY.Health -= 20;
			}

			if (CURRENTENEMY.Health <= 20)
			{
				CURRENTENEMY.Gib();
				CURRENTENEMY.Health = -1;
				Boss = false;
			}

			if (CURRENTENEMY.ShotCounter % 2 == 0)
			{
				if (rand() % 110 <= 1) CURRENTENEMY.Frametime *= -1;
				if (CURRENTENEMY.ShotCounter > 120 && CURRENTENEMY.ShotCounter % 2 == 0) AngleOffset += 0.5 * CURRENTENEMY.Frametime;

				if (CURRENTENEMY.Health < 5000 && CURRENTENEMY.ShotCounter % 50 == 0) CURRENTENEMY.Shoot(10, 1);

				if (CURRENTENEMY.Health < 3700 && CURRENTENEMY.ShotCounter % 60 == 0)
				{
					for (int s = 0; s < 3; s++) CURRENTENEMY.Shoot(9, rand() % 360);
				}

				if (CURRENTENEMY.Health < 2500 && CURRENTENEMY.ShotCounter % 15 == 0) CURRENTENEMY.Shoot(3, rand() % 360);

				if (CURRENTENEMY.Health < 700 && CURRENTENEMY.ShotCounter % 50 == 0)
				{
					int Angle = CalculateProjectileAngle(CURRENTENEMY.WorldX + CURRENTENEMY.CollisionRect.w / 2, CURRENTENEMY.WorldY + CURRENTENEMY.CollisionRect.h / 2, PlayerX, PlayerY);
					for (int u = -45; u <= 45; u += 45)
					{
						CURRENTENEMY.Shoot(3, Angle + u);
						for (int p = 0; p <= 10; p++) CURRENTENEMY.Shoot(8, rand() % 360);
					}
				}
			}

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, Warden, Screen);
			break;

		case 14: //Warkid
			CURRENTENEMY.ShotCounter++;

			if (IsIntersecting(CURRENTENEMY.CollisionRect, PlayerRect))
			{
				Damaged = true;
				DamageDealt = 50;
			}

			if (CURRENTENEMY.ShotCounter % 30 == 0) CURRENTENEMY.Shoot(3, PlayerX, PlayerY);
			if (CURRENTENEMY.ShotCounter % 60 == 0) CURRENTENEMY.Shoot(3, rand() % 360);
			if (CURRENTENEMY.ShotCounter % 120 == 0) CURRENTENEMY.BulletPattern(3);
			if (CURRENTENEMY.ShotCounter == 180)
			{
				CURRENTENEMY.BulletPattern(5);
				CURRENTENEMY.ShotCounter = 0;
			}

			if (IsIntersecting(CURRENTENEMY.CollisionRect, PlayerRect))
			{
				Damaged = true;
				DamageDealt = 40;
			}

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, Warkid, Screen);
			break;

		case 15: //Turret
			CURRENTENEMY.ShotCounter++;

			if (CURRENTENEMY.ShotCounter % 90 == 0)
			{
				for (int u = 0; u < 360; u += 90)
				{
					CURRENTENEMY.Shoot(3, u);
				}

				if (CURRENTENEMY.ShotCounter == 270)
					for (int p = 0; p <= 10; p++) CURRENTENEMY.Shoot(8, rand() % 360);

				else if (CURRENTENEMY.ShotCounter == 900)
				{
					CURRENTENEMY.BulletPattern(5);
					CURRENTENEMY.ShotCounter = 0;
				}
			}

			if (IsIntersecting(CURRENTENEMY.CollisionRect, PlayerRect))
			{
				Damaged = true;
				DamageDealt = 20;
			}

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, Turret, Screen);
			break;

		case 16: //Rush warden
			BossHealth = CURRENTENEMY.Health;

			CURRENTENEMY.ShotCounter++;

			if (CURRENTENEMY.ShotCounter >= 120)
			{
				AngleOffset += 0.4;

				if (CURRENTENEMY.ShotCounter == 220)
				{
					int f = rand() % 360;
					for (int d = 0; d < 360; d += 120) CURRENTENEMY.Shoot(10, f + d);
				}

				else if (CURRENTENEMY.ShotCounter == 320)
				{
					for (int d = 0; d <= 5; d++) CURRENTENEMY.Shoot(9, rand() % 360);
				}

				else if (CURRENTENEMY.ShotCounter == 420)
				{
					CURRENTENEMY.ShotCounter = 120;
					EnemyProjectile Big(4);
					Big.CollisionRect.w = 60;
					Big.CollisionRect.h = 60;
					Big.XVel = 0;
					Big.YVel = 4;
					Big.WorldY = 21;
					Big.Damage = 100;
						
					for	(int f = 31; f <= LevelWidth - 81; f += 200)
					{	
						Big.WorldX = f;
						EnemyProjectileVector.push_back(Big);
					}
				}

				if (CURRENTENEMY.Health <= 10)
				{
					CURRENTENEMY.Gib();
					Boss = false;
				}
			}

			else if (CURRENTENEMY.ShotCounter == 1)
			{
				WardenFlag = 1;
				for (float o = 0; o < 6.28; o += 1.26)
				{
					for (int p = 100; p <= 1400; p += 70)
					{
						CURRENTENEMY.Shoot(11, OrbitX - 20 + p * cos(o), OrbitY - 20 + p * sin(o));
					}
				}
			}
			
			else if (CURRENTENEMY.ShotCounter == 119)
			{
				BossTheme = Mix_LoadMUS("Resources/Sounds/Music/Beat4.ogg");
				Mix_PlayMusic(BossTheme, -1);
			}

			if (IsIntersecting(CURRENTENEMY.CollisionRect, PlayerRect))
			{
				Damaged = true;
				DamageDealt = 100;
			}

			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, Warden, Screen);
			break;

		case 17: //Rush SIS 
			BossHealth = CURRENTENEMY.Health;
			CURRENTENEMY.ShotCounter++;
			
			if (CURRENTENEMY.ShotCounter % 20 == 0) CURRENTENEMY.Shoot(2, 0);

			if (CURRENTENEMY.ShotCounter == 40)
			{
				int Distance = rand() % 400 + 200;
				CURRENTENEMY.Angle = PlayerY + YVel * Distance / 10;
				CURRENTENEMY.Timer = PlayerX - Distance;
				CURRENTENEMY.ShotCounter = 0;
			}

			CURRENTENEMY.XVel = (CURRENTENEMY.Timer - CURRENTENEMY.WorldX) / 10;
			CURRENTENEMY.YVel = (CURRENTENEMY.Angle - CURRENTENEMY.WorldY) / 10;

			if (abs(CURRENTENEMY.Timer - CURRENTENEMY.WorldX) > 1) CURRENTENEMY.WorldX += CURRENTENEMY.XVel;
			if (abs(CURRENTENEMY.Angle - CURRENTENEMY.WorldY) > 1) CURRENTENEMY.WorldY += CURRENTENEMY.YVel;
			CURRENTENEMY.StayInLevel();

			CURRENTENEMY.Frametime++;

			if (CURRENTENEMY.Frametime > 10)
			{
				CURRENTENEMY.Frame++;
				CURRENTENEMY.Frametime = 0;
				if (CURRENTENEMY.Frame > 2) CURRENTENEMY.Frame = 0;
			}

			if (IsIntersecting(CURRENTENEMY.CollisionRect, PlayerRect))
			{
				Damaged = true;
				DamageDealt = 20;
			}

			ApplyThis = Ship;
			ApplyThis->w = 50;
			ApplyThis->h = 20;
			ApplySurface(0, 0, Ship, ApplyThis, &ShipFrames[CURRENTENEMY.Frame]);
			ApplyThis = rotozoomSurface(ApplyThis, CURRENTENEMY.YVel / 2, 1, 0);
			ApplySurface(CURRENTENEMY.WorldX - CameraX, CURRENTENEMY.WorldY - CameraY, ApplyThis, Screen);
			break;

		case 18: //RUSH MARS
			BossHealth = CURRENTENEMY.Health;

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

				BossStage = 1;

			case 1:
				CURRENTENEMY.ShotCounter++;
				if (CURRENTENEMY.ShotCounter % 20 == 0)
				{
					CURRENTENEMY.Shoot(3, PlayerX, PlayerY);
					CURRENTENEMY.ShotCounter = 0;
				}

				else if (CURRENTENEMY.ShotCounter == 70)
				{
					CURRENTENEMY.ShotCounter = 0;
					CURRENTENEMY.BulletPattern(2);
				}
				
				if (CURRENTENEMY.WorldY + 250 > Camera.LevelHeight - 20)
				{
					CURRENTENEMY.WorldY = Camera.LevelHeight - 270;
					CURRENTENEMY.YVel *= -1;
				}
				else if (CURRENTENEMY.WorldY < 20)
				{
					CURRENTENEMY.WorldY = 20;
					CURRENTENEMY.YVel *= -1;
				}

				if (CURRENTENEMY.WorldX - 105 < 20)
				{
					CURRENTENEMY.WorldX = 125;
					CURRENTENEMY.XVel *= -1;
				}
				else if (CURRENTENEMY.WorldX + 145 > LevelWidth - 20)
				{
					CURRENTENEMY.WorldX = LevelWidth - 165;
					CURRENTENEMY.XVel *= -1;
				}

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
				SDL_FillRect(Screen, RenderRect, 0xFFFFFF);
				*RenderRect = Two;
				SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
				*RenderRect = Three;
				SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
				*RenderRect = Four;
				SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
				*RenderRect = Five;
				SDL_FillRect(Screen, RenderRect, 0xC0C0C0);

				if (CURRENTENEMY.WorldX < PlayerX)
				{
					if (CURRENTENEMY.XVel < 0) CURRENTENEMY.XVel += 0.4;
					else CURRENTENEMY.XVel += 0.2;
				}

				else if (CURRENTENEMY.WorldX > PlayerX)
				{
					if (CURRENTENEMY.XVel > 0) CURRENTENEMY.XVel -= 0.4;
					else CURRENTENEMY.XVel -= 0.2;
				}

				if (CURRENTENEMY.WorldY < PlayerY)
				{
					if (CURRENTENEMY.YVel < 0) CURRENTENEMY.YVel += 0.4;
					else CURRENTENEMY.YVel += 0.2;
				}

				else if (CURRENTENEMY.WorldY > PlayerY)
				{
					if (CURRENTENEMY.YVel > 0) CURRENTENEMY.YVel -= 0.4;
					else CURRENTENEMY.YVel -= 0.2;
				}

				if (abs(CURRENTENEMY.XVel) > 5) CURRENTENEMY.XVel = copysign(5, (float)CURRENTENEMY.XVel);
				if (abs(CURRENTENEMY.XVel) > 5) CURRENTENEMY.XVel = copysign(5, (float)CURRENTENEMY.XVel);

				CURRENTENEMY.WorldX += CURRENTENEMY.XVel;
				CURRENTENEMY.WorldY += CURRENTENEMY.YVel;

				PlayerRectangle.x = PlayerX - Camera.x;
				PlayerRectangle.y = PlayerY - Camera.y;

				if (IsIntersecting(PlayerRectangle, One) || IsIntersecting(PlayerRectangle, Two) || IsIntersecting(PlayerRectangle, Three) || IsIntersecting(PlayerRectangle, Four))
				{
					Damaged = true;
					DamageDealt = 50;
				}
				break;
			};
			break;

		case 19: //ADXERS Left Arm
			CURRENTENEMY.WorldX += CURRENTENEMY.XVel * CURRENTENEMY.Speed;
			CURRENTENEMY.WorldY += CURRENTENEMY.YVel * CURRENTENEMY.Speed;

			CURRENTENEMY.StayInLevel();

			if (rand() < 6000)
			{
				if (CURRENTENEMY.XVel != 0 && abs(CURRENTENEMY.Frametime - CURRENTENEMY.WorldY) >= 5)
				{
					CURRENTENEMY.XVel = 0;
					CURRENTENEMY.YVel = copysign(1, PlayerY - CURRENTENEMY.WorldY);
				}

				else if (abs(CURRENTENEMY.Frame - CURRENTENEMY.WorldX) >= 5)
				{
					CURRENTENEMY.YVel = 0;
					CURRENTENEMY.XVel = copysign(1, PlayerX - 500 - CURRENTENEMY.WorldX);
				}
			}

			CURRENTENEMY.ShotCounter++;

			if (CURRENTENEMY.ShotCounter % 30 == 0)
			{
				Angle = CalculateProjectileAngle(CURRENTENEMY.WorldX + CURRENTENEMY.CollisionRect.w/2, CURRENTENEMY.WorldY + CURRENTENEMY.CollisionRect.h/2, PlayerX, PlayerY);

				for (int u = -45; u <= 45; u += 45) CURRENTENEMY.Shoot(3, Angle + u);
			}

			if (CURRENTENEMY.ShotCounter % 60 == 0)
			{
				CURRENTENEMY.Frame = PlayerX - 500;
				CURRENTENEMY.Frametime = PlayerY;
				CURRENTENEMY.XVel = rand() % 2 * copysign(1, PlayerX - 500 - CURRENTENEMY.WorldX);
				CURRENTENEMY.YVel = (1 - abs(CURRENTENEMY.XVel)) * copysign(1, PlayerY - CURRENTENEMY.WorldY);
			}

			if (CURRENTENEMY.ShotCounter == 500)
			{
				CURRENTENEMY.BulletPattern(2);
				CURRENTENEMY.ShotCounter = 0;
			}

			RenderRect->x = CURRENTENEMY.WorldX - Camera.x;
			RenderRect->y = CURRENTENEMY.WorldY - Camera.y;
			RenderRect->w = CURRENTENEMY.CollisionRect.w;
			RenderRect->h = CURRENTENEMY.CollisionRect.h;
			SDL_FillRect(Screen, RenderRect, 0xFFFFFF);

			if (Camera.x <= 20)
			{
				RenderRect->x = 20 - Camera.x;
				RenderRect->w = CURRENTENEMY.WorldX - 20;
			}

			else if (CURRENTENEMY.WorldX > Camera.x)
			{
				RenderRect->x = 0;
				RenderRect->w = CURRENTENEMY.WorldX - Camera.x;
			}

			else RenderRect->w = 0;

			RenderRect->h = 10;
			RenderRect->y = (CURRENTENEMY.CollisionRect.h - RenderRect->h) / 2 + CURRENTENEMY.WorldY - Camera.y;
			SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
			break;

		case 20: //ADXERS right arm
			
			CURRENTENEMY.Angle += CURRENTENEMY.Timer;

			if (!InBetween(225, CURRENTENEMY.Angle, 315))
			{
				CURRENTENEMY.Timer *= -1;
				CURRENTENEMY.Angle += CURRENTENEMY.Timer * 2;
			}

			CURRENTENEMY.ShotCounter++;
			if (CURRENTENEMY.ShotCounter == 15) 
			{
				CURRENTENEMY.Shoot(3, CURRENTENEMY.Angle);
				CURRENTENEMY.ShotCounter = 0;
			}

			if ((PlayerX + 500 - CURRENTENEMY.Frame) * (PlayerX + 500 - CURRENTENEMY.Frame) + (PlayerY - CURRENTENEMY.Frametime) * (PlayerY - CURRENTENEMY.Frametime) > 40000)
			{
				CURRENTENEMY.Frame = PlayerX + 500;
				CURRENTENEMY.Frametime = PlayerY;
			}

			XDiff = CURRENTENEMY.Frame - CURRENTENEMY.WorldX;
			YDiff = CURRENTENEMY.Frametime - CURRENTENEMY.WorldY;

			if (abs(XDiff) > 5) CURRENTENEMY.WorldX += XDiff / 5;
			if (abs(YDiff) > 5) CURRENTENEMY.WorldY += YDiff / 5;
			CURRENTENEMY.StayInLevel();

			RenderRect->x = CURRENTENEMY.WorldX - Camera.x;
			RenderRect->y = CURRENTENEMY.WorldY - Camera.y;
			RenderRect->w = CURRENTENEMY.CollisionRect.w;
			RenderRect->h = CURRENTENEMY.CollisionRect.h;
			SDL_FillRect(Screen, RenderRect, 0xFFFFFF);

			if (Camera.y <= 20)
			{
				RenderRect->y = 20 - Camera.y;
				RenderRect->h = CURRENTENEMY.WorldY - 20;
			}

			else if (CURRENTENEMY.WorldY > Camera.y)
			{
				RenderRect->y = 0;
				RenderRect->h = CURRENTENEMY.WorldY - Camera.y;
			}

			else RenderRect->h = 0;

			RenderRect->w = 10;
			RenderRect->x = (CURRENTENEMY.CollisionRect.w - RenderRect->w) / 2 + CURRENTENEMY.WorldX - Camera.x;
			SDL_FillRect(Screen, RenderRect, 0xC0C0C0);
			break;

		case 21: //ADXERS
			{
				BossHealth = (CURRENTENEMY.Health - 1) % 4000;

				if (rand() < 1000) CURRENTENEMY.Frame *= -1;
				if (rand() < 1000) CURRENTENEMY.Frametime *= -1;

				if (DeathRect.x + DeathRect.w + CURRENTENEMY.Frame > LevelWidth - 20) CURRENTENEMY.Frame *= -1;
				else if (DeathRect.x + CURRENTENEMY.Frame < 20) CURRENTENEMY.Frame *= -1;

				DeathRect.x += CURRENTENEMY.Frame;

				switch (BossStage)
				{
				case 0:
					if (CURRENTENEMY.WorldY < 20) CURRENTENEMY.WorldY++;
					else 
					{
						CURRENTENEMY.CollisionRect.x = CURRENTENEMY.WorldX;
						CURRENTENEMY.CollisionRect.y = CURRENTENEMY.WorldY;
						BossStage = 1;
					}

					RenderRect->x = CURRENTENEMY.WorldX - Camera.x;
					RenderRect->y = CURRENTENEMY.WorldY - Camera.y;
					RenderRect->w = CURRENTENEMY.CollisionRect.w;
					RenderRect->h = CURRENTENEMY.CollisionRect.h;
					SDL_FillRect(Screen,RenderRect,0xFFFFFF);
					break;

				case 1:

					CURRENTENEMY.ShotCounter++;
					if (CURRENTENEMY.ShotCounter < 50 && CURRENTENEMY.ShotCounter % 2 == 0) CURRENTENEMY.BulletPattern(5);
					if (CURRENTENEMY.ShotCounter % 25 == 0) CURRENTENEMY.Shoot(3,PlayerX,PlayerY);
					else if (CURRENTENEMY.ShotCounter == 210)
					{
						CURRENTENEMY.ShotCounter = 0;
						for (int u = 120; u <= 240; u += 60)
						{
							CURRENTENEMY.CollisionRect.h *= 2;
							CURRENTENEMY.Shoot(9,u);
							CURRENTENEMY.Shoot(4,u);
							CURRENTENEMY.CollisionRect.h /= 2;
						}
					}

					RenderRect->x = CURRENTENEMY.WorldX - Camera.x;
					RenderRect->y = CURRENTENEMY.WorldY - Camera.y;
					RenderRect->w = CURRENTENEMY.CollisionRect.w;
					RenderRect->h = CURRENTENEMY.CollisionRect.h;
					SDL_FillRect(Screen, RenderRect, 0xFFFFFF);

					if (CURRENTENEMY.Health <= 16000)
					{
						CreateDebris(5, 6, CURRENTENEMY.WorldX + CURRENTENEMY.CollisionRect.w / 2, CURRENTENEMY.WorldY + CURRENTENEMY.CollisionRect.h / 2, 0, 0, 0xFFFFFF);
						BossStage = 2;
						CURRENTENEMY.WorldY = -40;
						CURRENTENEMY.WorldX = 850;
						CURRENTENEMY.ShotCounter = 0;
						CURRENTENEMY.Angle = 0;
					}

					break;

				case 2:

					CURRENTENEMY.WorldY++;
					if (CURRENTENEMY.WorldY == 40) 
					{
						BossStage = 3;

						BossTheme = Mix_LoadMUS("Resources/Sounds/Music/Too.ogg");
						Mix_PlayMusic(BossTheme, -1);

						Pickup pukciP;
						pukciP.WorldX = PlayerX - 300;
						pukciP.WorldY = PlayerY;
						pukciP.Type = 3;

						for (int u = 0; u <= 3; u++)
						{
							pukciP.WorldX += 100;
							PickupVector.push_back(pukciP);
						}
					}

					RenderRect->x = 850 - Camera.x;
					RenderRect->y = CURRENTENEMY.WorldY - 20 - Camera.y;
					RenderRect->w = 330;
					RenderRect->h = 20;
					SDL_FillRect(Screen, RenderRect, 0x262626);

					RenderRect->x = CURRENTENEMY.WorldX - Camera.x;
					RenderRect->y = CURRENTENEMY.WorldY - Camera.y;
					RenderRect->w = 60;
					RenderRect->h = 100;
					SDL_FillRect(Screen, RenderRect, 0xFFFFFFF);

					RenderRect->x += 270;
					SDL_FillRect(Screen, RenderRect, 0x262626);
					break;

				case 3:
				case 4:

					CURRENTENEMY.ShotCounter++;

					if (CURRENTENEMY.ShotCounter % 40 == 0) CURRENTENEMY.Shoot(3, PlayerX, PlayerY);
					
					else if (CURRENTENEMY.ShotCounter == 70) 
					{
						Distance = sqrt((CURRENTENEMY.WorldX + 30 - PlayerX) * (CURRENTENEMY.WorldX + 30 - PlayerX) + (CURRENTENEMY.WorldY + 50 - PlayerY) * (CURRENTENEMY.WorldY + 50 - PlayerY));
						if (CURRENTENEMY.Angle == 0) CURRENTENEMY.Shoot(11, CURRENTENEMY.WorldX - Distance , 90);
						else CURRENTENEMY.Shoot(11, CURRENTENEMY.WorldX + Distance, 90);
						CURRENTENEMY.ShotCounter = 0;
					}

					if (BossStage == 4)
					{
						if (CURRENTENEMY.ShotCounter % 8 == 0) CURRENTENEMY.Shoot(8, rand() % 180 + 90);
						else if (CURRENTENEMY.ShotCounter == 68) CURRENTENEMY.BulletPattern(5);
					}

					if (rand() < 100)
					{
						CURRENTENEMY.Angle = 1-CURRENTENEMY.Angle;
						CURRENTENEMY.WorldX = 850 + 270 * CURRENTENEMY.Angle;
					}

					if (CURRENTENEMY.Angle == 0) AngleOffset -= 0.5;
					else AngleOffset += 0.5;

					RenderRect->x = 850 - Camera.x;
					RenderRect->y = CURRENTENEMY.WorldY - 20 - Camera.y;
					RenderRect->w = 330;
					RenderRect->h = 20;
					SDL_FillRect(Screen, RenderRect, 0x262626);

					RenderRect->x = CURRENTENEMY.WorldX - Camera.x;
					RenderRect->y = CURRENTENEMY.WorldY - Camera.y;
					RenderRect->w = 60;
					RenderRect->h = 100;
					SDL_FillRect(Screen, RenderRect, 0xFFFFFFF);

					if (CURRENTENEMY.Angle == 0) RenderRect->x += 270;
					else RenderRect->x -= 270;

					SDL_FillRect(Screen, RenderRect, 0x262626);

					if (CURRENTENEMY.Health <= 8000)
					{
						CreateDebris(5, 6, CURRENTENEMY.WorldX + CURRENTENEMY.CollisionRect.w / 2, CURRENTENEMY.WorldY + CURRENTENEMY.CollisionRect.h / 2, 0, 0, 0xFFFFFF);
						BossStage = 5;
						CURRENTENEMY.WorldY = 20;
						CURRENTENEMY.WorldX = 500;
						CURRENTENEMY.ShotCounter = 0;
						CURRENTENEMY.Angle = 0;
						CURRENTENEMY.WorldY = -160;

						Pickup pukciP;
						pukciP.WorldX = PlayerX - 300;
						pukciP.WorldY = PlayerY;
						pukciP.Type = 3;

						EnemyProjectileVector.clear();

						for (int u = 0; u <= 3; u++)
						{
							pukciP.WorldX += 100;
							PickupVector.push_back(pukciP);
						}

					}

					else if (CURRENTENEMY.Health <= 12000) 
					{
						BossStage = 4;
					}
					break;

				case 5:
					CURRENTENEMY.WorldY++;
					if (CURRENTENEMY.WorldY == 40) 
					{
						BossStage = 6;
						DeathRect.w = 0;

						BossTheme = Mix_LoadMUS("Resources/Sounds/Music/Free.ogg");
						Mix_PlayMusic(BossTheme, -1);
					}

					RenderRect->x = 500 - Camera.x;
					RenderRect->y = CURRENTENEMY.WorldY - 20 - Camera.y;
					RenderRect->w = 1000;
					RenderRect->h = 20;
					SDL_FillRect(Screen, RenderRect, 0x262626);

					RenderRect->x = CURRENTENEMY.WorldX - Camera.x;
					RenderRect->y = CURRENTENEMY.WorldY - Camera.y;
					RenderRect->w = 60;
					RenderRect->h = 100;
					SDL_FillRect(Screen, RenderRect, 0xFFFFFFF);

					RenderRect->x += 940;
					SDL_FillRect(Screen, RenderRect, 0x262626);
					break;

				case 6:
					CURRENTENEMY.ShotCounter++;

					if (CURRENTENEMY.ShotCounter % 3 == 0) 
					{
						int x;
						if (CURRENTENEMY.Angle == 0) x = 940;
						else x = -940;

						EnemyProjectile OtherOne(3);
						OtherOne.WorldX = CURRENTENEMY.WorldX;
						OtherOne.WorldY = 20;
						OtherOne.Damage = 30;
						OtherOne.CollisionRect.w = 30;
						OtherOne.CollisionRect.h = 30;

						XDiff = CURRENTENEMY.WorldX - DeathRect.x;
						YDiff = CURRENTENEMY.WorldY - DeathRect.x;
						Distance = sqrt(XDiff * XDiff + YDiff * YDiff);
						int WastingMemory = atan(float(300.0/Distance)) * 180 / PI;
						int Angle = CalculateProjectileAngle(CURRENTENEMY.WorldX, CURRENTENEMY.WorldY, DeathRect.x, DeathRect.y);
						GetXYRatio(&OtherOne.XVel, &OtherOne.YVel, Angle + WastingMemory, 25);
						EnemyProjectileVector.push_back(OtherOne);
						GetXYRatio(&OtherOne.XVel, &OtherOne.YVel, Angle - WastingMemory, 25);
						EnemyProjectileVector.push_back(OtherOne);

						OtherOne.WorldX = CURRENTENEMY.WorldX + x;
						XDiff = CURRENTENEMY.WorldX - DeathRect.x + x;
						Distance = sqrt(XDiff * XDiff + YDiff * YDiff);
						WastingMemory = atan(float(300.0 / Distance)) * 180 / PI;
						Angle = CalculateProjectileAngle(CURRENTENEMY.WorldX + x, CURRENTENEMY.WorldY, DeathRect.x, DeathRect.y);
						GetXYRatio(&OtherOne.XVel, &OtherOne.YVel, Angle + WastingMemory, 25);
						EnemyProjectileVector.push_back(OtherOne);
						GetXYRatio(&OtherOne.XVel, &OtherOne.YVel, Angle - WastingMemory, 25);
						EnemyProjectileVector.push_back(OtherOne);
					}

					if (CURRENTENEMY.ShotCounter == 70)
					{
						CURRENTENEMY.BulletPattern(rand() % 5 + 1);
						CURRENTENEMY.ShotCounter = 0;
					}

					Distance = sqrt((DeathRect.x - PlayerX) * (DeathRect.x - PlayerX) + (DeathRect.y - PlayerY) * (DeathRect.y - PlayerY));
					if (Distance > 300)
					{
						DeathRect.x = PlayerX;
						DeathRect.y = PlayerY;
					}

					if (rand() < 100)
					{
						CURRENTENEMY.Angle = 1 - CURRENTENEMY.Angle;
						CURRENTENEMY.WorldX = 500 + 940 * CURRENTENEMY.Angle;
					}

					if (CURRENTENEMY.Angle == 0) AngleOffset -= 0.5;
					else AngleOffset += 0.5;

					RenderRect->x = 500 - Camera.x;
					RenderRect->y = CURRENTENEMY.WorldY - 20 - Camera.y;
					RenderRect->w = 1000;
					RenderRect->h = 20;
					SDL_FillRect(Screen, RenderRect, 0x262626);

					RenderRect->x = CURRENTENEMY.WorldX - Camera.x;
					RenderRect->y = CURRENTENEMY.WorldY - Camera.y;
					RenderRect->w = 60;
					RenderRect->h = 100;
					SDL_FillRect(Screen, RenderRect, 0xFFFFFFF);

					if (CURRENTENEMY.Angle == 0) RenderRect->x += 940;
					else RenderRect->x -= 940;

					SDL_FillRect(Screen, RenderRect, 0x262626);

					if (CURRENTENEMY.Health <= 2000)
					{
						CreateDebris(5, 6, CURRENTENEMY.WorldX + CURRENTENEMY.CollisionRect.w / 2, CURRENTENEMY.WorldY + CURRENTENEMY.CollisionRect.h / 2, 0, 0, 0xFFFFFF);
						BossStage = 7;
						CURRENTENEMY.WorldY = 20;
						CURRENTENEMY.WorldX = 850;
						CURRENTENEMY.ShotCounter = 0;
						CURRENTENEMY.Angle = 0;
						CURRENTENEMY.CollisionRect.w = 60;
						CURRENTENEMY.CollisionRect.h = 60;
						CURRENTENEMY.YVel = 10;
						CURRENTENEMY.OnFire == true;
						CURRENTENEMY.BurnTimer = 5;
						EnemyProjectileVector.clear();
					}

					break;

				case 7:
					{
						CURRENTENEMY.ShotCounter++;
						CURRENTENEMY.BurnTimer = 5;
						if (CURRENTENEMY.ShotCounter % 15 == 0)
						{
							CURRENTENEMY.Shoot(3,CURRENTENEMY.ShotCounter);
						}

						if (CURRENTENEMY.ShotCounter % 90 == 0)
						{
							CURRENTENEMY.Shoot(3, PlayerX, PlayerY);
						}

						else if (CURRENTENEMY.ShotCounter % 190 == 0)
						{
							CURRENTENEMY.BulletPattern(1);
						}

						if (CURRENTENEMY.ShotCounter % 250 == 0)
						{
							CURRENTENEMY.BulletPattern(5);
						}

						else if (CURRENTENEMY.ShotCounter == 361)
						{
							CURRENTENEMY.BulletPattern(2);
							CURRENTENEMY.ShotCounter == 0;
						}


						if (CURRENTENEMY.WorldX < PlayerX && CURRENTENEMY.XVel <= 8) CURRENTENEMY.XVel+=0.5;
						else if (CURRENTENEMY.WorldX > PlayerX && CURRENTENEMY.XVel >= -8) CURRENTENEMY.XVel-=0.5;
						if (CURRENTENEMY.WorldY < PlayerY && CURRENTENEMY.YVel <= 8) CURRENTENEMY.YVel+=0.5;
						else if (CURRENTENEMY.WorldY > PlayerY && CURRENTENEMY.YVel >= -8) CURRENTENEMY.YVel-=0.5;

						if (CURRENTENEMY.WorldY + CURRENTENEMY.YVel < 20) CURRENTENEMY.YVel *= -1;
						else if (CURRENTENEMY.WorldY + CURRENTENEMY.YVel > LevelHeight - 80) CURRENTENEMY.YVel *= -1;
						if (CURRENTENEMY.WorldX + CURRENTENEMY.XVel < 20 || CURRENTENEMY.WorldX + CURRENTENEMY.XVel > LevelWidth - 80) CURRENTENEMY.XVel *= -1;
						
						CURRENTENEMY.WorldY += CURRENTENEMY.YVel;
						CURRENTENEMY.WorldX += CURRENTENEMY.XVel;

						
						RenderRect->w = 20;
						RenderRect->h = 20;
						int X = 840;
						int Y = 20;

						SDL_FillRect(Screen, RenderRect, 0x262626);

						XDiff = CURRENTENEMY.WorldX - 830;
						YDiff = CURRENTENEMY.WorldY - 20;

						for (int u = 0; u < 16; u++)
						{
							X += XDiff / 16;
							Y += YDiff / 16;
							RenderRect->x = X - Camera.x;
							RenderRect->y = Y - Camera.y;
							RenderRect->w = 20;
							RenderRect->h = 20;
							SDL_FillRect(Screen, RenderRect, 0x262626);
						}

						RenderRect->w = 60;
						RenderRect->h = 60;
						RenderRect->x = CURRENTENEMY.WorldX - Camera.x;
						RenderRect->y = CURRENTENEMY.WorldY - Camera.y;
						SDL_FillRect(Screen, RenderRect, 0xFFFFFFF);

						if (IsIntersecting(CURRENTENEMY.CollisionRect, PlayerRect))
						{
							Damaged = true;
							DamageDealt = 50;
						}
						
					}

				}
			}
			break;
		};

		if (CURRENTENEMY.DamageFrames != 0)
		{
			ApplyText(CURRENTENEMY.WorldX + CURRENTENEMY.XOffset - Camera.x, CURRENTENEMY.WorldY + CURRENTENEMY.YOffset - Camera.y, std::to_string(CURRENTENEMY.Damage), Small, Red);
			CURRENTENEMY.DamageFrames--;

			if (CURRENTENEMY.DamageFrames == 0)
			{
				FloatSomeText(CURRENTENEMY.WorldX + CURRENTENEMY.XOffset, CURRENTENEMY.WorldY + CURRENTENEMY.YOffset, std::to_string(CURRENTENEMY.Damage), Red);
				CURRENTENEMY.Damage = 0;
			}
		}

		if (CURRENTENEMY.Health <= 0)
		{
			if (CURRENTENEMY.Health <= -60) CURRENTENEMY.Gib();

			if (CURRENTENEMY.Damage != 0) FloatSomeText(CURRENTENEMY.WorldX + CURRENTENEMY.CollisionRect.w / 2, CURRENTENEMY.WorldY - 30, std::to_string(CURRENTENEMY.Damage), Red);

			if (CURRENTENEMY.Type != 5 && CURRENTENEMY.Type != 6)
			{

				int tni = rand() % 100 + 1; //tni = int backwards

				if (CURRENTENEMY.Type == 8)
				{
					Pickup pukciP; //pukciP = Pickup backwards
					pukciP.WorldX = CURRENTENEMY.WorldX;
					pukciP.WorldY = CURRENTENEMY.WorldY;
					pukciP.Type = 4;
					PickupVector.push_back(pukciP);
				}

				else if (tni > 95)
				{
					Pickup pukciP;
					pukciP.WorldX = CURRENTENEMY.WorldX;
					pukciP.WorldY = CURRENTENEMY.WorldY;
					pukciP.Type = 3;
					PickupVector.push_back(pukciP);
				}

				else if (tni < 75 && CURRENTENEMY.Type == 1)
				{
					Pickup pukciP; //pukciP = Pickup backwards
					pukciP.WorldX = CURRENTENEMY.WorldX;
					pukciP.WorldY = CURRENTENEMY.WorldY;
					pukciP.Type = 1;
					PickupVector.push_back(pukciP);
				}

				else if (tni < 50 && CURRENTENEMY.Type == 2)
				{
					Pickup pukciP;
					pukciP.WorldX = CURRENTENEMY.WorldX;
					pukciP.WorldY = CURRENTENEMY.WorldY;
					pukciP.Type = 2;
					PickupVector.push_back(pukciP);
				}

				else if (tni < 40 && CURRENTENEMY.Type == 10)
				{
					Pickup pukciP;
					pukciP.WorldX = CURRENTENEMY.WorldX;
					pukciP.WorldY = CURRENTENEMY.WorldY;
					pukciP.Type = 5;
					PickupVector.push_back(pukciP);
				}

				else if (tni > 60 && CURRENTENEMY.Type == 10)
				{
					Pickup pukciP;
					pukciP.WorldX = CURRENTENEMY.WorldX;
					pukciP.WorldY = CURRENTENEMY.WorldY;
					pukciP.Type = 4;
					PickupVector.push_back(pukciP);
				}

				else if (tni > 60 && CURRENTENEMY.Type == 15)
				{
					Pickup pukciP;
					pukciP.WorldX = CURRENTENEMY.WorldX;
					pukciP.WorldY = CURRENTENEMY.WorldY;
					pukciP.Type = 7;
					PickupVector.push_back(pukciP);
				}

				CreateDebris(5, 6, CURRENTENEMY.WorldX, CURRENTENEMY.WorldY, CURRENTENEMY.XVel * 4, CURRENTENEMY.YVel * 4, 0xFFFFFF);

				if (CURRENTENEMY.Type == 3) //Explosion
				{
					for (int OutOfRange = 0; OutOfRange < 360; OutOfRange += 45)
					{
						CURRENTENEMY.Shoot(1, OutOfRange);
					}
				}
			}

			if (CURRENTENEMY.Type == 4 || CURRENTENEMY.Type == 9 || CURRENTENEMY.Type == 13 || CURRENTENEMY.Type >= 16)
			{
				Boss = false;
				Temp1 = CURRENTENEMY.WorldX;
				Temp2 = CURRENTENEMY.WorldY;
			}
		}

	}
	EnemyVector.erase(
		std::remove_if(
		EnemyVector.begin(),
		EnemyVector.end(),
		std::mem_fun_ref(&Enemy::IsNotActive)),
		EnemyVector.end());

	Enemies = EnemyVector.size();
}