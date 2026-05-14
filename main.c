#include <stdio.h>
#include <stdint.h>

typedef struct {
        uint8_t memory[4096];
        uint16_t stack[16];
        uint8_t V[16];
        uint16_t I;
        uint16_t PC;
        uint8_t SP;
        uint8_t delay_timer;
        uint8_t sound_timer;
        uint8_t screen [32][64];
        uint8_t keypad[16];
    } Chip8;

int main() {
    return 0; 
}