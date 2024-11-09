# Cilog C80
## Introduction
Cilog C80 is aimed to be a simple Zilog Z80 Microprocessor emulator written in C99. It is designed to be simple and easy to understand, and is intended to be used as a learning tool, but also as a base for more complex emulators. This project is still in its early stages, and is not yet fully functional!

## Features
- [ ] Z80 CPU emulation
- [ ] Memory emulation
- [ ] Basic I/O emulation
- [ ] Instruction set implementation
- [ ] Interrupts
- [ ] GUI
- [ ] Debugger
- [ ] Disassembler
- [ ] Assembler
- [ ] Memory viewer
- [ ] Memory editor
- [ ] Save states

## Testing
The project uses Unity as its testing framework. The tests are located in the `test` directory and can be run with CMake. The tests are automatically built when building the project.

## Building
To build the project, a linux environment is currently required. The project uses CMake as its build system. Following dependencies are required:
- CMake
- GCC
- (Optional) SDL2

## Cloning
This project utilizes submodules. To clone the project with all submodules, use the following command:
```
git clone https://github.com/leonv01/CilogC80.git
cd CilogC80
git submodule update --init --recursive
```

## External Libraries
- [Unity](https://github.com/ThrowTheSwitch/Unity)
- [GTK4](https://www.gtk.org/)

## Sources
- [Z80 CPU User Manual](https://www.zilog.com/docs/z80/um0080.pdf)
- [Z80 Instruction Set](https://clrhome.org/table/)


## License
This project is licensed under the MIT license.