#include"PlayerClass.h"
#include"FloatText.h"
#define CURRENTRECT RectVector.at(i)

Player::Player()
{
	Health = 100; 
	Lives = 3;
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
			if (DamageDealt > 0 && InvunFrames == 0)
			{
				InvunFrames = 111;
				Invincible = true;
				FloatSomeText(WorldX,WorldY - 20,std::to_string(static_cast<long double>(DamageDealt)),Red);
				Shake = true;
				Mag = 30;
				Dur = 10;
			}
			else if (Health > 100) Health = 100;
		}
	}

	int TempX = 0;
	int TempY = 0;

	if (NormalMovement == true)
	{
		SDL_PumpEvents();
		Uint8 *Memestates = SDL_GetKeyState(NULL);
		if ((Memestates[SDLK_RIGHT] || Memestates[SDLK_d]) && XVel <= TopSpeed) 
		{
			if (XVel + Speed > TopSpeed) XVel = TopSpeed;
			else XVel += Speed;
		}
		else if ((Memestates[SDLK_LEFT] || Memestates[SDLK_a])&& XVel >= -1 * TopSpeed) 
		{
			if (XVel - Speed < TopSpeed * -1) XVel = TopSpeed * -1;
			else XVel -= Speed;
		}
		else 
		{
			if (XVel > 0) XVel /= 2;
			else XVel /= 2;
		}
		if ((Memestates[SDLK_DOWN] || Memestates[SDLK_s]) && YVel <= TopSpeed) 
		{
			if (YVel + Speed > TopSpeed) YVel = TopSpeed;
			else YVel += Speed;
		}
		else if ((Memestates[SDLK_UP] || Memestates[SDLK_w]) && YVel >= -1 * TopSpeed) 
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
				if(InBetween(CURRENTRECT.x, PlayerRect.x, CURRENTRECT.x + CURRENTRECT.w) && !InBetween(CURRENTRECT.x, PlayerRect.x + PlayerRect.w, CURRENTRECT.x + CURRENTRECT.w))
				{
					PlayerRect.x += CURRENTRECT.x + CURRENTRECT.w - PlayerRect.x + 1;
					XVel = 0;
				}

				else if (!InBetween(CURRENTRECT.x, PlayerRect.x, CURRENTRECT.x + CURRENTRECT.w) && InBetween(CURRENTRECT.x, PlayerRect.x + PlayerRect.w, CURRENTRECT.x + CURRENTRECT.w))
				{
					PlayerRect.x -= PlayerRect.x + PlayerRect.w - (CURRENTRECT.x) + 1;
					XVel = 0;
				}

				if(IsIntersecting(CURRENTRECT,PlayerRect))
				{
					if(InBetween(CURRENTRECT.y, PlayerRect.y, CURRENTRECT.y + CURRENTRECT.h) && !InBetween(CURRENTRECT.y, PlayerRect.y + PlayerRect.h, CURRENTRECT.y + CURRENTRECT.h))
					{
						PlayerRect.y += CURRENTRECT.y + CURRENTRECT.h - PlayerRect.y + 1;
						YVel = 0;
					}

					else if (!InBetween(CURRENTRECT.y, PlayerRect.y, CURRENTRECT.y + CURRENTRECT.h) && InBetween(CURRENTRECT.y, PlayerRect.y + PlayerRect.h, CURRENTRECT.y + CURRENTRECT.h))
					{
						PlayerRect.y -= PlayerRect.y + PlayerRect.h - CURRENTRECT.y + 1;
						YVel = 0;
					}
				}
			}
		}
		WorldX = PlayerRect.x + Camera.x + 1;
		WorldY = PlayerRect.y + Camera.y + 1;
	}
	if (Render == true) ApplySurface(WorldX - Camera.x, WorldY - Camera.y, CurrentSprite, Screen);
}

