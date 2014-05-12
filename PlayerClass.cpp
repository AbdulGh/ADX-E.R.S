#include"PlayerClass.h"
#define CURRENTRECT RectVector.at(i)

Player::Player()
{
	Health = 100;
	Lives = 10;
	Damage = 1.0f;
	Speed = 0.5f;
	InvunFrames = 0;
	TopSpeed = 10;
	XVel = 0;
	YVel = 0;
	WorldX = 0;
	WorldY = 0;

	Reset = false;
	NormalDeath = true;
	NormalMovement = true;
	NormalCollision = true;
	Render = true;


	CurrentSprite = PlayerNormal;
}

void Player::Update()
{

	if (Health <= 0 && NormalDeath == true)
	{
		Health = 100;
		Lives -= 1;
		Reset = true;
	}
	
	if (InvunFrames != 0)
	{
		InvunFrames--;
		if (InvunFrames % 10 == 0) Render = !Render;
	}

	if (Damaged == true)
	{
		if (InvunFrames > 0) Damaged = false;
		else
		{
			Health -= DamageDealt;
			Damaged = false;
			InvunFrames = 91;
		}
	}

	int TempX = 0;
	int TempY = 0;

	if (NormalMovement == true)
	{
		SDL_PumpEvents();
		Uint8 *Memestates = SDL_GetKeyState(NULL);
		if (Memestates[SDLK_RIGHT] && XVel <= TopSpeed) 
		{
			if (XVel + Speed > TopSpeed) XVel = TopSpeed;
			else XVel += Speed;
		}
		else if (Memestates[SDLK_LEFT] && XVel >= -1 * TopSpeed) 
		{
			if (XVel - Speed < TopSpeed * -1) XVel = TopSpeed * -1;
			else XVel -= Speed;
		}
		else 
		{
			if (XVel > 0) XVel /= 2;
			else XVel /= 2;
		}
		if (Memestates[SDLK_DOWN] && YVel <= TopSpeed) 
		{
			if (YVel + Speed > TopSpeed) YVel = TopSpeed;
			else YVel += Speed;
		}
		else if (Memestates[SDLK_UP] && YVel >= -1 * TopSpeed) 
		{
			if (YVel - Speed < TopSpeed * -1) YVel = TopSpeed * -1;
			else YVel -= Speed;
		}
		else 
		{
			if (YVel > 0) YVel /= 2;
			else YVel /= 2;
		}
		if (abs(XVel) < 0.5) XVel = 0;
		if (abs(YVel) < 0.5) YVel = 0;
		TempX = WorldX + XVel;
		TempY = WorldY + YVel;
	}

	if (NormalCollision == true)
	{
		SDL_Rect PlayerRect;
		PlayerRect.x = TempX - Camera.x;
		PlayerRect.y = TempY - Camera.y;
		PlayerRect.w = CurrentSprite->w;
		PlayerRect.h = CurrentSprite->h;
		for (int i = 0; i < RectVector.size(); i++)
		{
			if (IsIntersecting(CURRENTRECT,PlayerRect))
			{
				if (PlayerRect.y > CURRENTRECT.y && PlayerRect.y < CURRENTRECT.y +  CURRENTRECT.h) YVel = 0;
				else if (PlayerRect.y + PlayerRect.h > CURRENTRECT.y && PlayerRect.y + PlayerRect.h < CURRENTRECT.y +  CURRENTRECT.h) YVel = 0;
				TempY = WorldY + YVel;
				PlayerRect.y = TempY - Camera.y;

				if (PlayerRect.x > CURRENTRECT.x && PlayerRect.x < CURRENTRECT.x +  CURRENTRECT.w) XVel = 0;
				else if (PlayerRect.x + PlayerRect.w > CURRENTRECT.x && PlayerRect.x + PlayerRect.w < CURRENTRECT.x +  CURRENTRECT.w) XVel = 0;
				TempX = WorldX + XVel;
				PlayerRect.x = TempX - Camera.x;
			}
		}
		WorldX += XVel;
		WorldY += YVel;
	}
	if (Render == true) ApplySurface(WorldX - Camera.x, WorldY - Camera.y, CurrentSprite, Screen);
}

