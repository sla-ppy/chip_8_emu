#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>

class Chip8 {
public:
    //MEMORY, STACK:
    uint8_t registers[16]{};
    uint8_t memory[4096]{};
    uint16_t index{};
    uint16_t progCounter{};
    uint16_t stack[16]{};
    uint8_t stackPtr{};
    // TIMERS:
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    // INPUT:
    uint8_t keypad[16]{};

    uint32_t graphics[64 * 32]{};
    uint16_t opcode{};

    // RNG
    std::default_random_engine randGen;
    std::uniform_int_distribution<uint8_t> randByte;

    // time_since_epoch() returns "duration", which we the immediately call count() on
    Chip8();

    // FUNC:
    void loadROM(char const* fileName);
};


#endif //CHIP8_H