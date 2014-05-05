#include<Windows.h>
#include"OpenDebugWindow.h"

std::vector <std::string> DebugLog;

void OpenDebugWindow(std::string Argument)
{
	if(Argument != "Open")
	{
		DebugLog.push_back(Argument);
	}
	else
	{
		AllocConsole();
		freopen("conin$","r",stdin);
		freopen("conout$","w",stdout);
		freopen("conout$","w",stderr);
		printf("Debug log:\n");
		for(int x = 0; x <= DebugLog.size() - 1;x++)
		{
			printf(DebugLog.at(x).c_str());
			printf("\n");
		}
		system("PAUSE");
	}
}
