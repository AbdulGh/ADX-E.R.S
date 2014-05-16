#include"FadeText.h"
#include"Declarations.h"

std::vector <std::string> FadeVector;

std::string CurrentString = "Blastoff!";
int CurrentAlpha = 1;
int CurrentDirection = 0;

void FadeText(std::string Text)
{
	FadeVector.push_back(Text);
	CurrentAlpha = 1;
	CurrentDirection = 0;
}

void CheckText()
{
	if (FadeVector.size() != 0)
	{
		if (CurrentString == "Blastoff!")
		{
			CurrentString = FadeVector.at(0);
			CurrentAlpha = 1;
			CurrentDirection = 0;
			FadeVector.erase(FadeVector.begin(), FadeVector.begin() + 1);
		}	
	}

	if (CurrentString != "Blastoff!")
	{
		Message = TTF_RenderText_Solid(SysSmall,CurrentString.c_str(),Blue);
		SDL_SetAlpha(Message,SDL_SRCALPHA,CurrentAlpha);
		ApplySurface((ScreenWidth - Message->w)/2, ScreenHeight - 100,Message,Screen);

		if (CurrentAlpha > 255) CurrentDirection = 1;
		else if (CurrentAlpha < 4 && CurrentDirection == 1) CurrentString = "Blastoff!";

		if (CurrentDirection == 0) CurrentAlpha += 3;
		else CurrentAlpha -= 3;
	}
}