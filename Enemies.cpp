#include"Enemies.h"
#include"CalculateProjectileAngle.h"
#include"GetXYRatio.h"

void Enemy::Bleed(int ProjectileXVel, int ProjectileYVel)
{
	CreateDebris(3,3,WorldX,WorldY,ProjectileXVel,ProjectileYVel,0x808080);
}

void Enemy::Shoot(int TargetX, int TargetY, int Type)
{
	float XInc;
	float YInc;
	GetXYRatio(&XInc,&YInc,CalculateProjectileAngle(WorldX,WorldY,TargetX,TargetY),5);

	EnemyProjectile PushThis;
	PushThis.WorldX = WorldX;
	PushThis.WorldY = WorldY;
	PushThis.XVel = XInc;
	PushThis.YVel = YInc;
}