#include"ButtonClass.h"

Button::Button()
{
	ButtonRect.x = 10;
	ButtonRect.y = 10;
	ButtonRect.w = 10;
	ButtonRect.h = 10;
	ID = 0;
	Normal = NULL; 
	NotNormal = NULL;
}

bool Button::IsMouseOver(int Mx, int My)
{
	if (Mx >= ButtonRect.x && Mx <= ButtonRect.x + ButtonRect.w && My >= ButtonRect.y && My <= ButtonRect.y + ButtonRect.h) return true; //we oop now
	else return false;
}