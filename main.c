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

int cycle(Chip8 *chip8) {
    // joining 2 bytes from memory
    uint16_t opcode = (chip8->memory[chip8->PC] << 8) | chip8->memory[chip8->PC + 1];
    chip8->PC += 2;

    // fetching variables from opcode 
    //(shifting x and y nibles to first position)
    uint8_t X   = (opcode & 0x0F00) >> 8;
    uint8_t Y   = (opcode & 0x00F0) >> 4;
    uint8_t N   = (opcode & 0x000F);
    uint8_t NN  = (opcode & 0x00FF);
    uint16_t NNN = (opcode & 0x0FFF);

    //decoding opcode group
    switch (opcode & 0xF000) {
        case 0x0000: 
            switch (opcode & 0x00FF) {
                case 0x00E0: // CLS
                    memset(chip8->screen, 0, sizeof(chip8->screen));
                    break;
                case 0x00EE: //RET
                    chip8->PC = chip8->stack[chip8->SP];    
                    chip8->SP -= 1;            
                    break;
                default: // SYS (NNN) ignored by modern interpreters
                    break;
            }
            break;

        case 0x1000: // JP (NNN)
            chip8->PC = NNN;
            break; 

        case 0x2000: // CALL (NNN)
            chip8->SP += 1;
            chip8->stack[chip8->SP] = chip8->PC;
            chip8->PC = NNN;
            break;

        case 0x3000: // SE (V[X], NN)
            if (chip8->V[X] == NN) {
                chip8->PC += 2;
            }
            break;

        case 0x4000: // SNE (V[X], NN)
            if (chip8->V[X] != NN) {
                chip8->PC += 2;
            }
            break;

        case 0x5000: // SE (V[X], V[Y])
            if (chip8->V[X] == chip8->V[Y]) {
                chip8->PC += 2;
            }
            break;

        case 0x6000: // LD (V[X], NN)
            chip8->V[X] = NN;
            break;

        case 0x7000: // ADD (V[X], NN)
            chip8->V[X] += NN;
            break;

        case 0x8000: 
            switch (opcode & 0x000F) {
                case 0x0000: //LD (V[X], V[Y])
                    chip8->V[X] = chip8->V[Y];
                    break;

                case 0x0001: //OR (V[X], V[Y])
                    chip8->V[X] = chip8->V[X] | chip8->V[Y];
                    break;

                case 0x0002: //AND (V[X], V[Y])
                    chip8->V[X] = chip8->V[X] & chip8->V[Y];
                    break;

                case 0x0003: //XOR (V[X], V[Y])
                    chip8->V[X] = chip8->V[X] ^ chip8->V[Y];
                    break;

                case 0x0004: //ADD (V[X], V[Y])
                    break;   

                case 0x0005: //SUB (V[X], V[Y])
                    break;

                case 0x0006: //SHR (V[X], V[Y])
                    break;

                case 0x0007: //SUBN (V[X], V[Y])
                    break;

                case 0x000E: //SHL (V[X], V[Y])
                    break;
            }
            break;

        case 0x9000: // SNE (V[X], V[Y])
            if (chip8->V[X] != chip8->V[Y]) {
                chip8->PC += 2;
            }
            break;

        case 0xA000: // LD (I, NNN)
            chip8->I = NNN;
            break;

        case 0xB000: // JP (V[0], NNN)
            chip8->PC = chip8->V[0] + NNN;
            break; 

        default:
            return 1;
    }
    return 0;
}

int main() {
    Chip8 chip8;
    if (init(&chip8) != 0) {
        return 1;
    }
    while (1) {
        cycle(&chip8);
    }
    return 0; 
}