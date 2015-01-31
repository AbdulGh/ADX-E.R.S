#include<math.h>
#include"DebugWindow.h"
#include"Declarations.h"

double ArcTan(double Number)
{
	return atan(Number) * (180 / 3.1415);
}

double CalculateProjectileAngle( double x1, double y1, double x2, double y2)
{
	double Calculation = ArcTan( abs( y1 - y2 ) / abs ( x1 - x2 ) );

	if (x2 > x1)
	{
		if (y2 < y1) Calculation = 90 - Calculation; //Top right
		else Calculation += 90; //Bottom right
	}

	else
	{
		if (y2 < y1) Calculation += 270; //Top left
		else Calculation = 270 - Calculation;
	}
	return Calculation;
}

/*
lol

double CalculateProjectileAngle(float PlayerX, float PlayerY, float MouseX, float MouseY)
{
	float Theta, Calculation;
	PlayerY = LevelHeight - PlayerY;
	MouseY = LevelHeight - MouseY; //Y axis is inverted


	if((PlayerX > MouseX) && (MouseY > PlayerY))//If the mouse is to the top left of the player
	{
		Calculation = (MouseY - PlayerY) / (PlayerX - MouseX);
		Theta = atan(Calculation)* 180 / PI;
		return 270 + Theta;
	}

	else if ((MouseY > PlayerY) && (MouseX > PlayerX)) //If the mouse is to the top right of the player
	{
		Calculation = (MouseY - PlayerY) / (MouseX - PlayerX);
		Theta = atan(Calculation)* 180 / PI;
		return 90 - Theta;
	}

	else if ((PlayerX > MouseX) && (PlayerY > MouseY)) //If the mouse is to the bottom left of the player
	{
		Calculation = (PlayerX - MouseX) / (PlayerY - MouseY);
		Theta = atan(Calculation)* 180 / PI;
		return 180 + Theta;
	}

	else if ((MouseX > PlayerX) && (PlayerY > MouseY)) //If the mouse is to the bottom right of the player
	{
		Calculation = (MouseX - PlayerX) / (PlayerY - MouseY);
		Theta = atan(Calculation)* 180 / PI;
		return 180 - Theta;
	}
	else if (MouseX == PlayerX)
	{
		if (MouseY > PlayerY) return 0;
		else return 180;
	}
	else if (MouseY == PlayerY)
	{
		if(MouseX > PlayerX) return 90;
		else return 270;
	}
	else 
	{
		DebugWindow("Something has gone wrong in CalculateProjectileAngle");
		return 999;
	}
}
*/