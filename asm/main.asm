ORG 0x1000       ; Origin, start at address 0x0000

LD A, 0x05       ; Load the value 5 into register A
LD B, 0x03       ; Load the value 3 into register B
ADD A, B         ; Add the value in register B to register A
LD (0x8000), A   ; Store the result at memory location 0x8000
LD A, 0x00
LD A, (0x8000)   ; Load the value at memory location 0x8000 into register A
LD C, 0x69       ; Load the value 69 into register C
SUB A, C         ; Subtract the value in register C from register A

HALT             ; Halt the CPU