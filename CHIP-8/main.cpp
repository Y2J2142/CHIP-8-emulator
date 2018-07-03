#include "CHIP8.h"
#include <iostream>
#include <filesystem>
#include <stdlib.h>
#include <Windows.h>
int main()
{
	CHIP8 chip8;
	chip8.initialize();

	chip8.loadFile("sierp.ch8");
	while (true)
	{
		chip8.draw();
		chip8.emulateCycle();
		if (chip8.delay_timer > 0)
			--chip8.delay_timer;
		Sleep(1000 / 60);
		system("pause");
	}

}