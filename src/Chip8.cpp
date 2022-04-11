#include <fstream>

#include "Chip8.h"

// memory is reserved from 0x000 - 0x1FF
const unsigned int START_ADDRESS{0x200};

Chip8::Chip8() {
    progCounter = START_ADDRESS;
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
