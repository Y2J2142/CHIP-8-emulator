#include "CHIP8.h"
#include <algorithm>
#include <time.h>
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
	for (auto &  ar : gfx)
		fill(begin(ar), end(ar), false);
	
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
		case 0x1000:
			pc = opcode & 0x0FFF;
			break;
		case 0x2000:
			stack[++sp] = pc;
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
			if (V[opcode & 0x0F00] == V[0x00F0])
				pc += 2;
			pc += 2;
			break;
		case 0x6000:
			V[opcode & 0x0F00] = 0x00FF;
			pc += 2;
			break;
		case 0x7000:
			V[opcode & 0x0F00] += opcode & 0x00FF;
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
				V[0x0F00] = V[0x00F0] - V[0x0F0];
				pc += 2;
				break;
			case 0x000E:
				V[0xF] = V[0x0F00] >> 7;
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
				uint8_t n = opcode & 0x000F;
				uint8_t x = V[opcode & 0x0F00], y = V[opcode & 0x00F0];
					for (int i = 0; i <= n; ++i, ++y)
					{
						x %= 64;
						y %= 32;
						for (auto bit = 0; bit < 8; ++bit, ++x)
						{
							
							if ((gfx[y][x] ^ ((I >> bit) & 1)) == 0 && gfx[y][x])
								V[0xF] = 1;
							gfx[y][x] ^= (I >> bit) & 1;
						}
					}
			}
			pc += 2;
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
			}
			pc += 2;
			break;
			case 0x0065:
			{
				uint16_t dupa = I;
				for (auto i = 0; i < 0xF; ++i, ++dupa)
					V[i] = memory[dupa];
			}
			pc += 2;
			break;
			}
	}
}

CHIP8::CHIP8()
{
}


CHIP8::~CHIP8()
{
}
