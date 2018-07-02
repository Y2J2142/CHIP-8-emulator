#include "CHIP8.h"
#include <algorithm>

using std::fill;
using std::begin;
using std::end;
using std::copy_n;


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


}

CHIP8::CHIP8()
{
}


CHIP8::~CHIP8()
{
}
