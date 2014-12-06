#include"GetXYRatio.h"
#define LECONVERSION (3.1418 / 180)

void GetXYRatio(float * x, float * y, float Angle, float SPEED)
{
	if (Angle > 360) Angle -= 360;

	if (Angle > 270)
	{
		Angle -= 270;
		*x = -1 * SPEED * cos(Angle * LECONVERSION);
		*y = -1 * SPEED * sin(Angle * LECONVERSION);
	}
	else if (Angle > 180)
	{
		Angle = 270 - Angle;
		*x = -1 * SPEED * cos(Angle * LECONVERSION);
		*y =  SPEED * sin(Angle * LECONVERSION);
	}
	else if (Angle > 90)
	{
		Angle -= 90;
		*x = SPEED * cos(Angle * LECONVERSION);
		*y =  SPEED * sin(Angle * LECONVERSION);
	}
	else
	{
		Angle = 90 - Angle;
		*x = SPEED * cos(Angle * LECONVERSION);
		*y = -1 * SPEED * sin(Angle * LECONVERSION);
	}
}