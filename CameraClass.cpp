#include"CameraClass.h"
#include<math.h>

Viewport::Viewport() 
{
	ScreenWidth = 1920;
	ScreenHeight = 1080;
	TargetX = 0;
	TargetY = 0;
	x = 0;
	y = 0;
}


void Viewport::MoveViewport(int NewX, int NewY)
{
	if (NewX < 0) TargetX = 0;
	else if (NewX + ScreenWidth > LevelWidth) TargetX = LevelWidth - ScreenWidth;
	else TargetX = NewX;

	if (NewY < 0) TargetY = 0;
	else if (NewY + ScreenHeight > LevelHeight) TargetY = LevelHeight - ScreenHeight;
	else TargetY = NewY;
}

void Viewport::Update()
{
	float XDiff = (TargetX - x);
	float YDiff = (TargetY - y);
	float Distance = sqrt( (XDiff * XDiff) + (YDiff * YDiff));
	if (Distance > 5)
	{
		if (abs(XDiff) > 100) x += 30 * XDiff / abs(XDiff);
		else x += XDiff / 4;
		if (abs(YDiff) > 100) y += 30 * YDiff / abs(YDiff);
		else y += YDiff / 4;
	}
}