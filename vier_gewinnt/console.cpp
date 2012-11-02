#include "console.hpp"

#ifdef _WIN32
#	include <Windows.h>
#endif


namespace vg
{
	void clear_console()
	{
#ifdef _WIN32
		COORD coordScreen = {0, 0};
		DWORD charsWritten = 0;
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		DWORD conSize = 0;
		HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(console, &csbi);
		conSize = csbi.dwSize.X * csbi.dwSize.Y;
		FillConsoleOutputCharacter(console, ' ', conSize, coordScreen, &charsWritten);
		GetConsoleScreenBufferInfo(console, &csbi);
		FillConsoleOutputAttribute(console, csbi.wAttributes, conSize, coordScreen,
			&charsWritten);
		SetConsoleCursorPosition(console, coordScreen);
#endif
	}

	void set_console_color(console_color foreground, console_color background)
	{
#ifdef _WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), foreground | (background << 4));
#endif
	}
}
