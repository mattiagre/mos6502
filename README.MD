## A MOS 6502 CPU implementation in modern C++

Provides a class named `mos6502` and a simple bus with 64 KiB of RAM. 

Unofficial opcodes are supported. BCD is also implemented and can be
disabled removing `#define BCD_SUPPORTED` preprocessor directive from
mos6502.h