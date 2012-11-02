#ifndef CONSOLE_HPP
#define CONSOLE_HPP


namespace vg
{
	enum console_color
	{
		Black,
		DarkBlue,
		DarkGreen,
		DarkCyan,
		DarkRed,
		DarkMagenta,
		DarkBrown,
		LightGray,
		DarkGray,
		Blue,
		Green,
		Cyan,
		Red,
		Magenta,
		Yellow,
		White
	};


	void clear_console();
	void set_console_color(console_color foreground, console_color background);
}


#endif
