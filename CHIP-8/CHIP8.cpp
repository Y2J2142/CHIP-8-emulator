#include "CHIP8.h"
#include <algorithm>
#include <time.h>
#include <iostream>
using std::fill;
using std::begin;
using std::end;
using std::copy_n;
using std::find;
using std::distance;

void CHIP8::initialize()
{
	pc = 0x200;
	opcode = 0;
	I = 0;
	sp = 0;
	fill(begin(gfx), end(gfx), false);
	
	fill(begin(stack), end(stack), 0);
	fill(begin(V), end(V), 0);
	fill(begin(memory), end(memory), 0);
	copy_n(begin(CHIP8::font), sizeof(CHIP8::font), begin(memory));
	delay_timer = 0xF;
	sound_timer = 0xF;
}

void CHIP8::emulateCycle()
{
	opcode = memory[pc] << 8 | memory[pc + 1];

	switch (opcode & 0xF000)
	{
		case 0x0000:
		{
			switch (opcode & 0x000F)
			{
			case 0x0000:
				fill(begin(gfx), end(gfx), false);
				pc += 2;
				break;
			case 0x000E:
				pc = stack[sp];
				--sp;
				break;
			}
		}
		case 0x1000:
			pc = opcode & 0x0FFF;
			break;
		case 0x2000:
			stack[++sp] = pc + 2;
			pc = opcode & 0x0FFF;
			break;
		case 0x3000:
			if (V[opcode & 0x0F00] == (opcode & 0x00FF))
				pc += 2;
			pc += 2;
			break;
		case 0x4000:
			if (V[opcode & 0x0F00] != (opcode & 0x00FF))
				pc += 2;
			pc += 2;
			break;
		case 0x5000:
			if (V[opcode & 0x0F00] == V[opcode & 0x00F0])
				pc += 2;
			pc += 2;
			break;
		case 0x6000:
			V[opcode & 0x0F00] = (opcode & 0x00FF);
			pc += 2;
			break;
		case 0x7000:
			V[opcode & 0x0F00] += (opcode & 0x00FF);
			pc += 2;
			break;
		case 0x8000:
			switch (opcode & 0x000F)
			{
			case 0x0000:
				V[opcode & 0x0F00] = V[opcode & 0x00F0];
				pc += 2;
				break;
			case 0x0001:
				V[opcode & 0x0F00] = V[opcode & 0x0F00] | V[opcode & 0x00F0];
				pc += 2;
				break;
			case 0x0002:
				V[opcode & 0x0F00] = V[opcode & 0x0F00] & V[opcode & 0x00F0];
				pc += 2;
				break;
			case 0x0003:
				V[opcode & 0x0F00] = V[opcode & 0x0F00] ^ V[opcode & 0x00F0];
				pc += 2;
				break;
			case 0x0004:
				if (V[opcode & 0x0F00] + V[opcode & 0x00F0] > 255)
					V[0xF] = 1;
				V[opcode & 0x0F00] = V[opcode & 0x0F00] + V[opcode & 0x00F0];
				pc += 2;
				break;
			case 0x0005:
				V[0xF] = V[opcode & 0x0F00] > V[opcode & 0x00F0];
				V[opcode & 0x0F00] = V[opcode & 0x0F00] - V[opcode & 0x00F0];
				pc += 2;
				break;
			case 0x0006:
				V[0xF] = V[0x0F00] & 1;
				V[0x0F00] /= 2;
				pc += 2;
				break;
			case 0x0007:
				V[0xF] = V[0x00F0] > V[0x0F00];
				V[0x0F00] = V[0x00F0] - V[0x0F00];
				pc += 2;
				break;
			case 0x000E:
				V[0xF] = (V[0x0F00] >> 7) & 1;
				V[0x0F00] *= 2;
				pc += 2;
				break;
			}
			break;
		case 0x9000:
			if (V[opcode & 0x0F00] != V[opcode & 0x00F0])
				pc += 2;
			pc += 2;
			break;
		case 0xA000:
			I = opcode & 0x0FFF;
			pc += 2;
		break;
		case 0xB000:
			pc = V[0] + opcode & 0x0FFF;
			break;
		case 0xC000:
			srand(time(NULL));
			V[opcode & 0x0F00] = (rand() % 256) & (opcode & 0x00FF);
			pc += 2;
			break;
		case 0xD000:
		{
			unsigned short x = V[(opcode & 0x0F00) >> 8];
			unsigned short y = V[(opcode & 0x00F0) >> 4];
			unsigned short height = opcode & 0x000F;
			unsigned short pixel;

			V[0xF] = 0;
			for (int yline = 0; yline < height; yline++)
			{
				pixel = memory[I + yline];
				for (int xline = 0; xline < 8; xline++)
				{
					if ((pixel & (0x80 >> xline)) != 0)
					{
						if (gfx[(x + xline + ((y + yline) * 64))] == 1)
						{
							V[0xF] = 1;
						}
						gfx[x + xline + ((y + yline) * 64)] ^= 1;
					}
				}
			}
			pc += 2;
		}
			break;
		case 0xE000:
			switch (opcode & 0x00FF)
			{
			case 0x009E:
				if (key[V[opcode & 0x0F00]])
					pc += 2;
				pc += 2;
				break;
			case 0x00A1:
				if (!key[V[opcode & 0x0F00]])
					pc += 2;
				pc += 2;
				break;
			}
			break;
		case 0xF000:
			switch (opcode & 0x00FF)
			{
			case 0x0007:
				V[opcode & 0x0F00] = delay_timer;
				pc += 2;
				break;
			case 0x000A:
				while (find(begin(key), end(key), true) == end(key));
				V[opcode & 0x0F00] = distance(begin(key), find(begin(key), end(key), true));
				pc += 2;
				break;
			case 0x0015:
				delay_timer = V[opcode & 0x0F00];
				pc += 2;
				break;
			case 0x0018:
				sound_timer = V[opcode & 0x0F00];
				pc += 2;
				break;
			case 0x001E:
				I += V[opcode & 0x0F00];
				pc += 2;
				break;
			case 0x0029:
				I = V[opcode & 0x0F00] * 5;
				pc += 2;
			case 0x0033:
			{
				uint8_t val = V[opcode & 0x0F00];
				memory[I] = val / 100;
				val %= 100;
				memory[I + 1] = val / 10;
				val %= 10;
				memory[I + 2] = val;
			}
				pc += 2;
				break;
			case 0x0055:
			{
				uint16_t dupa = I;
				for (auto i = 0; i < 0xF; ++i, ++dupa)
					memory[dupa] = V[i];
				I += (opcode & 0x0F00) + 1;
			}
			pc += 2;
			break;
			case 0x0065:
			{
				uint16_t dupa = I;
				for (auto i = 0; i < 0xF; ++i, ++dupa)
					V[i] = memory[dupa];
				I += (opcode & 0x0F00) + 1;
			}
			pc += 2;
			break;
			}
	}
}

void CHIP8::loadFile(std::string filename)
{

		printf("Loading: %s\n", filename);

		// Open file
		FILE * pFile;
		auto err = fopen_s(&pFile, filename.c_str(), "r");
		if (pFile == NULL)
		{
			fputs("File error", stderr);
		}

		// Check file size
		fseek(pFile, 0, SEEK_END);
		long lSize = ftell(pFile);
		rewind(pFile);
		printf("Filesize: %d\n", (int)lSize);

		// Allocate memory to contain the whole file
		char * buffer = (char*)malloc(sizeof(char) * lSize);
		if (buffer == NULL)
		{
			fputs("Memory error", stderr);
		}

		// Copy the file into the buffer
		size_t result = fread(buffer, 1, lSize, pFile);
		if (result != lSize)
		{
			fputs("Reading error", stderr);
		}

		// Copy buffer to Chip8 memory
		if ((4096 - 512) > lSize)
		{
			for (int i = 0; i < lSize; ++i)
				memory[i + 512] = buffer[i];
		}
		else
			printf("Error: ROM too big for memory");

		// Close file, free buffer
		fclose(pFile);
		free(buffer);
}

void CHIP8::draw()
{
	system("cls");

	for (int i = 0; i < 32; ++i)
	{
		for (int j = 0; j < 64; ++j)
		{
			if (gfx[i*j])
				std::cout << char(219);
			else
				std::cout << " ";
		}

		std::cout << "\n";
	}
}

CHIP8::CHIP8()
{
}


CHIP8::~CHIP8()
{
}
