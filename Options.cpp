#include"Declarations.h"
#include"CreateButton.h"
#include"Options.h"

void Options()
{
	bool Done = false;

	SDL_Surface *ArrowRotated = rotozoomSurface(Arrow,3.1418,1,0);
	CreateButton(20, 300, 1, ArrowRotated, ArrowRotated, 0);
	CreateButton(420, 300, 2, Arrow, Arrow, 0);
	CreateButton(20, 400, 3, ArrowRotated, ArrowRotated, 0);
	CreateButton(420, 400, 4, Arrow, Arrow, 0);
	CreateButton(20, 500, 5, ArrowRotated, ArrowRotated, 0);
	CreateButton(420, 500, 6, Arrow, Arrow, 0);
	CreateButton(20, 600, 7, ArrowRotated, ArrowRotated, 0);
	CreateButton(420, 600, 8, Arrow, Arrow, 0);
	CreateButton(20, 700, 9, ArrowRotated, ArrowRotated, 0);
	CreateButton(420, 700, 10, Arrow, Arrow, 0);
	SDL_Surface *Text;
	int x = 0, y = 0;
	while (!Done)
	{
		ClearScreen();
		DoButtons();
		DoMouse(&x, &y);

		SpareStream.str("");
		SpareStream << "Volume: " << Volume;
		Text = TTF_RenderText_Solid(Small, SpareStream.str().c_str(), White);
		ApplySurface(20 + (400 - Text->w)/ 2, 300, Text, Screen);

		SpareStream.str("");
		SpareStream << "Screen shake: " << ScreenShake;
		Text = TTF_RenderText_Solid(Small, SpareStream.str().c_str(), White);
		ApplySurface(20 + (400 - Text->w) / 2, 400, Text, Screen);

		SpareStream.str("");
		SpareStream << "Step sounds: " << StepSoundsEnabled;
		Text = TTF_RenderText_Solid(Small, SpareStream.str().c_str(), White);
		ApplySurface(20 + (400 - Text->w) / 2, 500, Text, Screen);

		SpareStream.str("");
		SpareStream << "Windowed: " << Windowed;
		Text = TTF_RenderText_Solid(Small, SpareStream.str().c_str(), White);
		ApplySurface(20 + (400 - Text->w) / 2, 600, Text, Screen);

		SpareStream.str("");
		SpareStream << "Resolution: " << ScreenWidth << " x " << ScreenHeight;
		Text = TTF_RenderText_Solid(Small, SpareStream.str().c_str(), White);
		ApplySurface(20 + (400 - Text->w) / 2, 700, Text, Screen);

		SDL_Flip(Screen);
		SDL_Delay(10);
	}

	std::fstream SettingsOutput;
	SettingsOutput.open("Resources/Settings");
	SettingsOutput << Volume;
	SettingsOutput << ScreenShake;
	SettingsOutput << StepSoundsEnabled;
	SettingsOutput << Windowed;
	SettingsOutput << ScreenWidth;
	SettingsOutput << ScreenHeight;
	SettingsOutput << FirstTime;
	SettingsOutput.close();

	ClearButtons();
	SDL_FreeSurface(ArrowRotated);
	SDL_FreeSurface(Text);
}