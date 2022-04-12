// file read/write
#include <fstream>
// rng
#include <chrono>
#include <random>
// memset
#include <cstring>


#include "Chip8.h"

// memory is reserved from 0x000 - 0x1FF
const unsigned int START_ADDRESS{0x200};
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;

uint8_t fontset[FONTSET_SIZE] =
        {
                0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                0x20, 0x60, 0x20, 0x20, 0x70, // 1
                0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

// FIXME: might be in the wrong spot? check and verify l8r
Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {  // seed RNG with current time
    progCounter = START_ADDRESS;

    for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }

    // init RNG
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U); // generate value between 0-255
}

void Chip8::loadROM(char const* fileName) {
    // open file as BIN, move ptr to end of file
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);

    if (file.is_open()) {
        // size of file, allocate buffer to hold
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        // ptr to beginning of the file, fill buffer with data from reading
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();


        for (long i = 0; i < size; ++i) {
            // from START_ADDRESS since 0x000 - 0x1FF is reserved
            memory[START_ADDRESS + i] = buffer[i];
        }

        // free
        delete[] buffer;
    }
}

///////////////// OPCODES: /////////////////
void Chip8::OP_00E0() {
    // set video buffer to zeroes
    std::memset(video, 0, sizeof(video));
}
void Chip8::OP_00EE() {
    -- stackPtr;
    progCounter = stack[stackPtr];
}
void Chip8::OP_1nnn() {
    uint16_t address = opcode & 0x0FFFu;
    progCounter = address;
}
void Chip8::OP_2nnn() {
    uint16_t address = opcode & 0x0FFFu;

    stack[stackPtr] = progCounter;
    ++stackPtr;
}


void Chip8::OP_3xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] == byte) {
        progCounter += 2;
    }
}
void Chip8::OP_4xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] != byte) {
        progCounter += 2;
    }
}
void Chip8::OP_5xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] == registers[Vy]) {
        progCounter += 2;
    }
}
void Chip8::OP_6xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = byte;
}
void Chip8::OP_7xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] += byte;
}
void Chip8::OP_8xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x0F00u) >> 4u;

    registers[Vx] = registers[Vy];
}
void Chip8::OP_8xy1() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    // OR
    registers[Vx] |= registers[Vy];
}
void Chip8::OP_8xy2() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    // AND
    registers[Vx] &= registers[Vy];
}
void Chip8::OP_8xy3() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    // XOR
    registers[Vx] ^= registers[Vy];
}
void Chip8::OP_8xy4() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    // ADD
    uint16_t sum = registers[Vx] + registers[Vy];

    // set overflow flag to 1 or 0
    if (sum > 255U) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }
    registers[Vx] = sum & 0xFFu;
}
void Chip8::OP_8xy5() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] > registers[Vy]) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    // SUB
    registers[Vx] -= registers[Vy];
}
void Chip8::OP_8xy6() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    // save LSB in VF
    registers[0xF] = (registers[Vx] & 0x1u);

    // right shift
    registers[Vx] >>= 1;
}
void Chip8::OP_8xy7() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vy] > registers[Vx]) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    registers[Vx] = registers[Vy] - registers[Vx];
}
void Chip8::OP_8xyE() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    // save MSB in VF
    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

    registers[Vx] <<= 1;
}
void Chip8::OP_9xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] != registers[Vy]) {
        progCounter += 2;
    }
}

void Chip8::OP_Annn() {
    uint16_t address = opcode & 0x0FFFu;

    index = address;
}
void Chip8::OP_Bnnn() {
    uint16_t address = opcode & 0x0FFFu;

    progCounter = registers[0] + address;
}
void Chip8::OP_Cxkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = randByte(randGen) & byte;
}
void Chip8::OP_Dxyn() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    // wrap if going beyond screen boundaries
    uint8_t posX = registers[Vx] % VIDEO_WIDTH;
    uint8_t posY = registers[Vy] % VIDEO_HEIGHT;

    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row) {
        uint8_t spriteByte = memory[index + row];

        for (unsigned int col = 0; col < 8; ++col) {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t* screenPixel = &video[(posY + row)] * VIDEO_WIDTH + (posX + col);

            // sprite pixel is on
            if (spritePixel) {
                if (*screenPixel == 0xFFFFFFFF) {
                    registers[0xF] = 1;
                }

                // xor with sprite pixel
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}
