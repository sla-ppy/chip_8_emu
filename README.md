## CPU:

Example CHIP opcode:

`$7522`
- 7 - ADD
- 5 - Register 5
- $22 - The value 22

Add the value $22 to Register 5.

---

## Whats an EMU?
- Reads original machine code instructions
- Interprets them
- Replicates functionally of target machine on host machine
- ROM files contain instructions
- EMU reads them
- EMU mimics original machine

---

## Parts:
### 16 8-bit Registers:
- Labeled `V0 to VF`
- Able to hold any value from `0x00` to `0xFF`
- `VF is special:` Flag to hold information about comparision result

### 4K Bytes of Memory:
- `4096 bytes` of memory
- Address space: `0x000 to 0xFFF`
- 3 segments within:
	+ `0x000 to 0x1FF` - Reserved for the CHIP-8 interpreter, we never write or read from.
	+ `0x050 to 0x0A0` - Storage for the 16 built-in characters (`0 to F`)
	+ `0x200 to 0xFFF` - Stores instructions from ROM, anything left after ROM's space is free to use.

### 16-bit Index Register:
- Special register used to store mem addresses for use in operations.
---

### 16-bit Program Counter:
- Also known as `PC`.
- Special register that holds the address of the next instruction to execute.
- 16 bits since it needs to hold the maximum memory address: `0xFFF`

An opcode is two bytes, but its mem addressed as a single byte.

We fetch a value from PC, then PC+1, and connect them into a single value. Then we increment PC by 2 before excecuting any instruction, since some of them manipulate the PC. Some will add to the PC, some will subtract, some change it completely.

### 16-level Stack:
- `CALL` execute instruction in a different region.
- `RET` return, puts address from previous CALL function.
- The Stack holds the PC value when the CALL was excecuted and RET pulls it from the stack, and puts it back into PC, so the CPU can excecute the next cycle.

- CHIP-8 has 16 levels of stack, hence it holds 16 different PC's.

- Putting a PC onto the Stack is called pushing and pulling a PC off the stack is called popping.

### 8-bit Stack Pointer:
- Also known as `SP`
- We need 8 bits for the SP, since its represented by an array. We need 16 indices which a single byte can manage.
- On popping a value off, we dont actually delete, but copy the value and decrement the SP, so it points to the previous value.

### 8-bit Delay Timer:
- If the timer value is zero, it stay zero.
- If loaded with a value, decrement at the rate of 60Hz.

### 8-bit Sound Timer:
- Same behavior as Delay Timer, expect a single tone will buzz when the value is not zero.
- [If I want to include sound:](https://stackoverflow.com/questions/10110905/simple-sound-wave-generator-with-sdl-in-c/45002609#45002609)

### 16 Input Keys:
- Each key is either pressed or not pressed.
- Input Mapping:
```
Keypad       Keyboard
+-+-+-+-+    +-+-+-+-+
|1|2|3|C|    |1|2|3|4|
+-+-+-+-+    +-+-+-+-+
|4|5|6|D|    |Q|W|E|R|
+-+-+-+-+ => +-+-+-+-+
|7|8|9|E|    |A|S|D|F|
+-+-+-+-+    +-+-+-+-+
|A|0|B|F|    |Z|X|C|V|
+-+-+-+-+    +-+-+-+-+
```

### 64X32 Monochrome Display Memory:
- Memory buffer for storing graphics.
- Resolution: `64x32`
- Each pixel is either `on/off`. 2 colors represented.
- Will be stored as `uint32`
- `0xFFFFFFFF - ON`
- `0x00000000 - OFF`

---

### Loading The Fonts:
- 16 characters for expected total
- Each character is represented with 5 bytes - 40 bits

Quick example:
(The character represented below is 'F')
```
11110000
10000000
11110000
10000000
10000000
```

---

# The Instructions
- 34 in total
- [Useful source for more info on them](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)

```
00E0 - CLS 	        - Clear
00EE - RET 	        - Return
1nnn - JP addr      - Jump to nnn
2nnn - CALL addr    - Call subroutine at nnn
3xkk - SE Vx, byte  - Skip next instruction if Vx = kk
4xkk - SNE Vx, byte - Skip next instruction if Vx != kk
5xí0 - SE Vx, Vy	   - Skip next instruction if Vx = Vy
6xkk - LD Vx, byte  - Set Vx = kk
7xkk - ADD Vx, byte - Set Vx = Vx + kk
8xy0 - LD Vx, Vy    - Set Vx = Vy
8xy1 - OR Vx, Vy    - Set Vx = Vx OR Vy
8xy2 - AND Vx, Vy   - Set Vx = Vx AND Vy
8xy3 - XOR Vx, Vy   - Set Vx = Vx XOR Vy

8xy4 - ADD Vx, Vy   - Set Vx = Vx + Vy, set VF = carry
The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.

8xy5 - SUB Vx, Vy   - Set Vx = Vx - Vy, set VF = NOT borrow.
If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.

8xy6 - SHR Vx       - Set Vx = Vx SHR 1.
If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
```

## Source:
[Austin Morlan - Chip8 emulator](https://austinmorlan.com/posts/chip8_emulator/#16-8-bit-registers)