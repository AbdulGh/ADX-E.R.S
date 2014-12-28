#include"PlayerClass.h"
#include"FloatText.h"
#define CURRENTRECT RectVector.at(i)

#define TOPSPEED 10
#define ACCELERATION 0.8

Player::Player()
{
	Health = 100; 
	Lives = 3;
	InvunFrames = 0;
	XVel = 0;
	YVel = 0;
	WorldX = 0;
	WorldY = 0;

	Reset = false;
	NormalMovement = true;
	Render = true;

	CurrentSprite = PlayerNormal;
}

void Player::Update()
{	
	if (Health <= 0)
	{
		Health = 0;
		Lives -= 1;
		Reset = true;
	}
	
	if (InvunFrames != 0)
	{
		InvunFrames--;
		if (InvunFrames == 0)
		{
			Invincible = false;
			Render = true;
		}
		else if (InvunFrames % 5 == 0) Render = !Render;
	}

	if (Damaged == true)
	{
		Damaged = false;
		if (Invincible == false)
		{
			Health -= DamageDealt;
			if (DamageDealt > 0)
			{
				InvunFrames = 111;
				Invincible = true;
				FloatSomeText(WorldX,WorldY - 20,std::to_string(DamageDealt),Red);
				Shake = true;
				Mag = 30;
				Dur = 10;
			}
			else if (Health > 100) Health = 100;
		}
	}

	if (NormalMovement == true)
	{
		SDL_PumpEvents();
		Uint8 *Memestates = SDL_GetKeyState(NULL);

		if (Memestates[SDLK_RIGHT] || Memestates[SDLK_d]) 
		{
			if (XVel + ACCELERATION > TOPSPEED) XVel = TOPSPEED;
			else if (XVel < -1) XVel = -1;
			else XVel += ACCELERATION;
		}

		else if (Memestates[SDLK_LEFT] || Memestates[SDLK_a])
		{
			if (XVel - ACCELERATION < -TOPSPEED) XVel = -TOPSPEED;
			else if (XVel > 1) XVel = 1;
			else XVel -= ACCELERATION;
		}

		else 
		{
			XVel /= 1.8;
		}

		if (Memestates[SDLK_DOWN] || Memestates[SDLK_s]) 
		{
			if (YVel + ACCELERATION > TOPSPEED) YVel = TOPSPEED;
			else if (YVel < -1) YVel = -1;
			else YVel += ACCELERATION;
		}

		else if (Memestates[SDLK_UP] || Memestates[SDLK_w])
		{
			if (YVel - ACCELERATION < -TOPSPEED) YVel = -TOPSPEED;
			else if (YVel > 1) YVel = 1;
			else YVel -= ACCELERATION;
		}

		else 
		{
			YVel /= 1.8;
		}

		if (abs(XVel) < 0.1) XVel = 0;
		if (abs(YVel) < 0.1) YVel = 0;

		PlayerRect.x = WorldX + XVel - (Camera.x + 1);
		PlayerRect.y = WorldY + YVel - (Camera.y + 1);
		PlayerRect.w = CurrentSprite->w + 1;
		PlayerRect.h = CurrentSprite->h + 1;


		for (int i = 0; i < RectVector.size(); i++)
		{
			if (IsIntersecting(PlayerRect, CURRENTRECT))
			{
				PlayerRect.x -= XVel;
				
				if (!IsIntersecting(PlayerRect, CURRENTRECT)) XVel *= -1;

				else
				{
					PlayerRect.x += XVel;
					PlayerRect.y -= YVel;
					YVel *= -1;
					if (IsIntersecting(PlayerRect, CURRENTRECT)) XVel *= -1;
				}
			}
		}

		WorldX += XVel;
		WorldY += YVel;
	}

	if (Render == true) ApplySurface(WorldX - Camera.x, WorldY - Camera.y, CurrentSprite, Screen);
}

