#include <cstdint>

uint16_t opcode;
uint8_t  memory[4096];
uint8_t V[16];
uint16_t I, pc;
bool gfx[64 * 32];
uint8_t delay_timer, sound_timer;
uint16_t stack[16];
uint16_t sp;
bool key[16];