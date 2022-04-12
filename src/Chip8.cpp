#include <fstream>
#include <chrono>
#include <random>

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
