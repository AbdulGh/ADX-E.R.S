#include"FadeText.h"
#include"Declarations.h"

std::vector <std::string> FadeVector;
std::vector <int> AlphaVector;
std::vector <int> DirectionVector;

void FadeText(std::string Text)
{
	FadeVector.push_back(Text);
	AlphaVector.push_back(1);
	DirectionVector.push_back(0);
}

void CheckText()
{
	if (FadeVector.size() != 0)
	{
		for (int i = 0; i < FadeVector.size(); i++)
		{
			if(AlphaVector.at(i) > 0)
			{
				Message = TTF_RenderText_Solid(SysSmall,FadeVector.at(i).c_str(),Blue);
				
				if (AlphaVector.at(i) > 255) DirectionVector.at(i) = 1;
				if (DirectionVector.at(i) == 0) AlphaVector.at(i)+=2;
				else AlphaVector.at(i)-=2;

				SDL_SetAlpha(Message,SDL_SRCALPHA,AlphaVector.at(i));
				ApplySurface((ScreenWidth - Message->w)/2, ScreenHeight - 100,Message,Screen);
			}

			else
			{
				AlphaVector.erase(AlphaVector.begin() + i, AlphaVector.begin() + 1 + i);
				FadeVector.erase(FadeVector.begin() + i, FadeVector.begin() + i + 1);
				DirectionVector.erase(DirectionVector.begin() + i, DirectionVector.begin() + i + 1);
			}

		}
	}
}