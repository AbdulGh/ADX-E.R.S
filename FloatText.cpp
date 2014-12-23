#include"FloatText.h"

std::vector <FloatText> FloatVector;

void FloatSomeText(int WorldX, int WorldY, std::string String, SDL_Color Colour, int Type)
{
	if (FloatVector.size() <= 200)
	{
		FloatText PushThis;
		PushThis.x = WorldX;
		PushThis.y = WorldY;
		PushThis.Alpha = 255;
		PushThis.Colour = Colour;
		PushThis.String = String;
		if (Type == 1) PushThis.Time = 0;
		else if (Type == 2) PushThis.Time = 30;
		PushThis.Type = Type;
		FloatVector.push_back(PushThis);
	}
}

#define CURRENTFLOAT FloatVector.at(i)

void DoFloat(float CameraX,float CameraY)
{
	for (int i = 0; i < FloatVector.size(); i++)
	{
		CURRENTFLOAT.Time++;
		switch (CURRENTFLOAT.Type)
		{
		case 1:
			Message = TTF_RenderText_Solid(Small,CURRENTFLOAT.String.c_str(),CURRENTFLOAT.Colour);
			break;
		case 2:
			Message = TTF_RenderText_Solid(SmallSmall,CURRENTFLOAT.String.c_str(),CURRENTFLOAT.Colour);
		}

		if (CURRENTFLOAT.Time > 60)
		{
			switch (CURRENTFLOAT.Type)
			{
			case 1:
				CURRENTFLOAT.y -= 0.1;
				CURRENTFLOAT.Alpha -= 2;
				Message = TTF_RenderText_Solid(Small,CURRENTFLOAT.String.c_str(),CURRENTFLOAT.Colour);
				break;
			case 2:
				CURRENTFLOAT.y -= 0.5;
				CURRENTFLOAT.Alpha -= 4;
				Message = TTF_RenderText_Solid(SmallSmall,CURRENTFLOAT.String.c_str(),CURRENTFLOAT.Colour);
			}
			if (Message != NULL) SDL_SetAlpha(Message,SDL_SRCALPHA,CURRENTFLOAT.Alpha);
		}

		ApplySurface(CURRENTFLOAT.x - CameraX, CURRENTFLOAT.y - CameraY, Message, Screen);

		if (CURRENTFLOAT.Alpha <= 4) FloatVector.erase(FloatVector.begin() + i);
	}
}