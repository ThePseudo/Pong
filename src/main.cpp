#include <iostream>
#include "App.h"
int main()
{
	try {
		App app;
		app.Init();
		app.Run();
		app.Destroy();
	}
	catch (std::exception &e)
	{
		std::cout << "Fatal error: " << e.what() << std::endl;
		return -1;
	}
	return 0;
}
