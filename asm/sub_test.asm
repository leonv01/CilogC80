ORG 0x0000

LD A, 0x05
LD B, 0x03
LD C, B
SUB A, B

LD A, 0x0A
LD C, 0x10

SUB A, C

HALT