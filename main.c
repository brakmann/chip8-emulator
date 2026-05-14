#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef struct {
        uint8_t memory[4096];
        uint16_t stack[16];
        uint8_t V[16];
        uint16_t I; // Address register
        uint16_t PC; // Program counter
        uint8_t SP; // Stack pointer
        uint8_t delay_timer;
        uint8_t sound_timer;
        uint8_t screen [32][64];
        uint8_t keypad[16];
    } Chip8;

    //Инициализировать систему и вывести все шрифты до чтения рома
    const uint8_t FONTS[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, //0
        0x20, 0x60, 0xA0, 0x20, 0x80, //1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
        0xF0, 0x10, 0x80, 0x10, 0xF0, //3
        0x90, 0x90, 0xF0, 0x10, 0x10, //4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
        0xF0, 0x10, 0x20, 0x20, 0x20, //7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
        0xF0, 0x90, 0xF0, 0x90, 0x90, //A
        0xE0, 0xA0, 0xF0, 0x90, 0xF0, //B
        0xF0, 0x80, 0x80, 0x80, 0xF0, //C
        0xE0, 0x90, 0x90, 0x90, 0xE0, //D
        0xF0, 0x80, 0xE0, 0x80, 0xF0, //E
        0xF0, 0x80, 0xE0, 0x80, 0x80 //F
    };
    const uint16_t START_ADDRESS = 0x200;

int load_rom(Chip8 *chip8, const char *path) {
    FILE *file;
    file = fopen(path, "rb");

    if (file == NULL) {
        return 1; // reading file error
    }

    // finding file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size > sizeof(chip8->memory) - START_ADDRESS) {
        fclose(file);
        return 2; // too large file
    }

    uint8_t rom[size];
    fread (rom, 1, size, file);
    memcpy(chip8->memory + START_ADDRESS, rom, size);
    fclose(file);
    return 0; // success
}

int init (Chip8 *chip8) {
    chip8->PC = START_ADDRESS;
    // initialize sprite memory
    memcpy(chip8->memory, FONTS, sizeof(FONTS));
    if (load_rom(chip8, "ROM/game.ch8") != 0) {
        printf("Ошибка загрузки ROM\n");
        return 1;  
    }
    return 0;
}

int main() {
    Chip8 chip8;
    if (init(&chip8) != 0) {
        return 1;
    }
    return 0; 
}