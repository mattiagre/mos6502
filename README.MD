## A MOS 6502 CPU implementation in modern C++

Provides a class named `mos6502` that emulates a MOS 6502 and a simple bus with 64 KiB of RAM. 

#### What is implemented? 
- All official opcodes 
- Unofficial opcodes (from [Nesdev](http://nesdev.com/undocumented_opcodes.txt))
- BCD (Binary Coded Decimal) for `ADC` and `SBC`, that can be disabled removing `#define BCD_SUPPORTED`  
- A disassembly routine that converts bytes to instructions' string representation 

#### Test successfully passed:
- [Klaus Dormann test](https://github.com/Klaus2m5/6502_65C02_functional_tests] 
- [Nestest](https://www.qmtpro.com/~nes/misc/nestest.txt), with correct cycles, disabling BCD