#include <iostream>

void sleep(unsigned seconds)
{
	if (seconds == 0)
	{
		return;
	}
	if (seconds == 1)
	{
		std::cout << "Error E5102 occured. Press [Enter] to continue\n";
	}
	else
	{
		std::cout << "Press [Enter] " << (seconds - 1) << " seconds after you read this message\n";
	}
	std::string ignored;
	std::getline(std::cin, ignored);
}

int main()
{
	sleep(1);
}
