///
/// Iplementation of all (legal) opcodes
///

#include <cstdint>

#include "../mos6502.h"


// ADd with Carry
// A = A + {fetched} + C
// Affects flags: N,V,Z,C
// Can require another cycle
bool mos6502::ADC()
{
#ifdef BCD_SUPPORTED
	if (!getFlagStatus(D))
	{
#endif
		uint8_t old_A = A;
		uint16_t result = static_cast<uint16_t>(A) + static_cast<uint16_t>(fetched) + getFlagStatus(C);
		A = result & 0x00FF;

		setFlagStatus(C, result > 0xFF);
		setFlagStatus(Z, (result & 0xFF) == 0);
		setFlagStatus(V, ((~(static_cast<uint16_t>(old_A) ^ static_cast<uint16_t>(fetched))) & (static_cast<uint16_t>(old_A) ^ result)) & 0x80);
		setFlagStatus(N, result & 0x80);
#ifdef BCD_SUPPORTED
	}
	else
	{
		// Implementation from http://www.6502.org/tutorials/decimal_mode.html#A
		uint8_t old_A = A;
		uint16_t AL = (A & 0x0F) + (fetched & 0x0F) + getFlagStatus(C);

		if (AL >= 0x0A)
			AL = ((AL + 0x06) & 0x0F) + 0x10;

		uint16_t temp = (A & 0xF0) + (fetched & 0xF0) + AL;

		if (temp >= 0xA0)
			temp += 0x60;

		A = temp & 0xFF;

		uint16_t bin_result = static_cast<uint16_t>(old_A) + static_cast<uint16_t>(fetched) + getFlagStatus(C);

		setFlagStatus(C, temp >= 0x100);
		setFlagStatus(Z, bin_result == 0x00);
		setFlagStatus(N, temp & (1 << 7));
		setFlagStatus(V, temp < -128 || temp > 127);
	}
#endif

	return true;
}


// bitwise AND with accumulator
// A = A & {fetched}
// Affects flags: N,Z
// Can require another cycle
bool mos6502::AND()
{
	A &= fetched;

	setFlagStatus(Z, A == 0x00);
	setFlagStatus(N, A & 0x80);

	return true;
}


// Arithmetic Shift Left 
// {fetched} = {fetched} << 1
// Affects flags: N,Z,C
bool mos6502::ASL()
{
	uint8_t old_fetched = fetched;
	fetched <<= 1;

	setFlagStatus(C, old_fetched & 0x80);
	setFlagStatus(Z, fetched == 0x00);
	setFlagStatus(N, fetched & 0x80);

	if (lookup[opcode].address_mode == &mos6502::ACC)
		A = fetched;
	else
		write(abs_address, fetched);

	return false;
}


// Branch on Carry Clear
// if (C == 0) goto PC + {relative}
// Affects flags: none
// Can require another cycle
bool mos6502::BCC()
{
	if (!getFlagStatus(C))
	{
		++cycles;

		abs_address = PC + rel_address;

		if ((PC & 0xFF00) != (abs_address & 0xFF00))
			++cycles;

		PC = abs_address;
	}

	return false;
}


// Branch on Carry Set
// if (C == 1) goto PC + {relative}
// Affects flags: none
// Can require another cycle
bool mos6502::BCS()
{
	if (getFlagStatus(C))
	{
		++cycles;

		abs_address = PC + rel_address;

		if ((PC & 0xFF00) != (abs_address & 0xFF00))
			++cycles;

		PC = abs_address;
	}

	return false;
}


// Branch on EQual
// if (Z == 1) goto PC + {relative}
// Affects flags: none
bool mos6502::BEQ()
{
	if (getFlagStatus(Z))
	{
		++cycles;

		abs_address = PC + rel_address;

		if ((PC & 0xFF00) != (abs_address & 0xFF00))
			++cycles;

		PC = abs_address;
	}

	return false;
}


// test BITs
// 
// Affects flags: N,V,Z
bool mos6502::BIT()
{
	uint8_t temp = A & fetched;

	setFlagStatus(Z, temp == 0x00);
	setFlagStatus(V, fetched & (1 << 6));
	setFlagStatus(N, fetched & (1 << 7));

	return false;
}


// Branch on MInus
// if (N == 1) goto PC + {relative}
// Affects flags: none
// Can require another cycle
bool mos6502::BMI()
{
	if (getFlagStatus(N))
	{
		++cycles;

		abs_address = PC + rel_address;

		if ((PC & 0xFF00) != (abs_address & 0xFF00))
			++cycles;

		PC = abs_address;
	}

	return false;
}


// Branch on Not Equal
// if (Z == 0) goto PC + {relative}
// Affects flags: none
// Can require another cycle
bool mos6502::BNE()
{
	if (!getFlagStatus(Z))
	{
		++cycles;

		abs_address = PC + rel_address;

		if ((PC & 0xFF00) != (abs_address & 0xFF00))
			++cycles;

		PC = abs_address;
	}

	return false;
}


// Branch on PLus
// if (N == 0) goto PC + {relative}
// Affects flags: none
// Can require another cycle
bool mos6502::BPL()
{
	if (!getFlagStatus(N))
	{
		++cycles;

		abs_address = PC + rel_address;

		if ((PC & 0xFF00) != (abs_address & 0xFF00))
			++cycles;

		PC = abs_address;
	}

	return false;
}


// BReaK
// Push PC, push P with B flag set, PC = {#FFFF} << 8 OR {#FFFE}
// Affects Flags: B 
bool mos6502::BRK()
{
	++PC;
	write(0x0100 + SP--, (PC >> 8) & 0xFF);
	write(0x0100 + SP--, PC & 0xFF);

	write(0x0100 + SP--, P | B | U);

	setFlagStatus(I, true);
	setFlagStatus(B, false);

	uint16_t lo, hi;
	abs_address = 0xFFFE;

	lo = read(abs_address++);
	hi = read(abs_address);

	PC = (hi << 8) | lo;

	return false;
}


// Branch on oVerflow Clear
// if (V == 0) goto PC + {relative}
// Affects flags: none
// Can require another cycle
bool mos6502::BVC()
{
	if (!getFlagStatus(V))
	{
		++cycles;

		abs_address = PC + rel_address;

		if ((PC & 0xFF00) != (abs_address & 0xFF00))
			++cycles;

		PC = abs_address;
	}

	return false;
}


// Branch on oVerflow Set
// if (V == 1) goto PC + {relative}
// Affects flags: none
// Can require another cycle
bool mos6502::BVS()
{
	if (getFlagStatus(V))
	{
		++cycles;

		abs_address = PC + rel_address;

		if ((PC & 0xFF00) != (abs_address & 0xFF00))
			++cycles;

		PC = abs_address;
	}

	return false;
}


// CLear Carry
// C = 0
// Affects flags: C
bool mos6502::CLC()
{
	setFlagStatus(C, false);

	return false;
}


// CLear Decimal
// D = 0
// Affects flags: D
bool mos6502::CLD()
{
	setFlagStatus(D, false);

	return false;
}


// CLear Interrupt
// I = 0
// Affects flags: I
bool mos6502::CLI()
{
	setFlagStatus(I, false);

	return false;
}


// CLear Overflow
// V = 0
// Affects flags: V
bool mos6502::CLV()
{
	setFlagStatus(V, false);

	return false;
}


// CoMPare accumulator
// Compares A with {fetched}
// Affects flags: N,Z,C
// Can require another cycle
bool mos6502::CMP()
{
	setFlagStatus(C, A >= fetched);
	setFlagStatus(Z, A == fetched);
	setFlagStatus(N, (A - fetched) & 0x80);

	return true;
}


// ComPare X register
// Compares X with {fetched}
// Affects flags: N,Z,C
bool mos6502::CPX()
{
	setFlagStatus(C, X >= fetched);
	setFlagStatus(Z, X == fetched);
	setFlagStatus(N, (X - fetched) & 0x80);

	return false;
}


// ComPare Y register
// Compares Y with {fetched}
// Affects flags: N,Z,C
bool mos6502::CPY()
{
	setFlagStatus(C, Y >= fetched);
	setFlagStatus(Z, Y == fetched);
	setFlagStatus(N, (Y - fetched) & 0x80);

	return false;
}


// DECrement memory
// {fetched} = {fetched} - 1
// Affects flags: N,Z
bool mos6502::DEC()
{
	--fetched;

	setFlagStatus(Z, fetched == 0x00);
	setFlagStatus(N, fetched & 0x80);

	write(abs_address, fetched);

	return false;
}


// DEcrement X
// X = X - 1
// Affects flags: N,Z
bool mos6502::DEX()
{
	--X;

	setFlagStatus(Z, X == 0x00);
	setFlagStatus(N, X & 0x80);

	return false;
}


// DEcrement Y
// Y = Y - 1
// Affects flags: N,Z
bool mos6502::DEY()
{
	--Y;

	setFlagStatus(Z, Y == 0x00);
	setFlagStatus(N, Y & 0x80);

	return false;
}


// bitwise Exclusive OR
// A = A ^ {fetched}
// Affects flags: N,Z
// Can require another cycle
bool mos6502::EOR()
{
	A ^= fetched;

	setFlagStatus(Z, A == 0x00);
	setFlagStatus(N, A & 0x80);

	return true;
}


// INCrement memory
// {fetched} = {fetched} + 1
// Affects flags: N,Z
bool mos6502::INC()
{
	++fetched;

	setFlagStatus(Z, fetched == 0x00);
	setFlagStatus(N, fetched & 0x80);

	write(abs_address, fetched);

	return false;
}


// INCrement X
// X = X + 1
// Affects flags: N,Z
bool mos6502::INX()
{
	++X;

	setFlagStatus(Z, X == 0x00);
	setFlagStatus(N, X & 0x80);

	return false;
}


// INCrement Y
// Y = Y + 1
// Affects flags: N,Z
bool mos6502::INY()
{
	++Y;
	setFlagStatus(Z, Y == 0x00);
	setFlagStatus(N, Y & 0x80);

	return false;
}


// JuMP
// PC = address
// Affects flags: none
bool mos6502::JMP()
{
	PC = abs_address;

	return false;
}


// Jump to SubRoutine
// PUSH PC - 1, PC = address
// Affets flags: none
bool mos6502::JSR()
{
	PC--;

	write(0x0100 + SP--, (PC >> 8) & 0xFF);
	write(0x0100 + SP--, PC & 0xFF);

	PC = abs_address;

	return false;
}


// LoaD Accumulator
// A = {fetched}
// Affects flags: N,Z
// Can require another cycle
bool mos6502::LDA()
{
	A = fetched;

	setFlagStatus(Z, A == 0x00);
	setFlagStatus(N, A & 0x80);

	return true;
}


// LoaD X register
// X = {fetched}
// Affects flags: N,Z
// Can require another cycle
bool mos6502::LDX()
{
	X = fetched;

	setFlagStatus(Z, X == 0x00);
	setFlagStatus(N, X & 0x80);

	return true;
}


// LoaD Y register
// Y = {fetched}
// Affects flags: N,Z
// Can require another cycle
bool mos6502::LDY()
{
	Y = fetched;

	setFlagStatus(Z, Y == 0x00);
	setFlagStatus(N, Y & 0x80);

	return true;
}


// Logical Shift Right
// {fetched} = {fetched} >> 1
// Affects flags: N,Z,C
bool mos6502::LSR()
{
	uint8_t old_fetched = fetched;

	fetched >>= 1;

	setFlagStatus(C, old_fetched & 0x1);
	setFlagStatus(Z, fetched == 0x00);
	setFlagStatus(N, fetched & 0x80);

	if (lookup[opcode].address_mode == &mos6502::ACC)
		A = fetched;
	else
		write(abs_address, fetched);

	return false;
}


// No OPeration
// 
// Affects flags: none
// Can require another cycle
bool mos6502::NOP()
{
	switch (opcode)
	{
	case 0x1C:
	case 0x3C:
	case 0x5C:
	case 0x7C:
	case 0xDC:
	case 0xFC:
		return true;
	}
	return false;
}


// bitwise OR with Accumulator
// A = A | {fetched}
// Affects flags: N,Z
// Can require another cycle
bool mos6502::ORA()
{
	A |= fetched;

	setFlagStatus(Z, A == 0x00);
	setFlagStatus(N, A & 0x80);

	return true;
}


// PusH Accumulator
// Push A
// Affects flags: none
bool mos6502::PHA()
{
	write(0x0100 + SP--, A);

	return false;
}


// PusH Processor status
// Push P with B flag
// Affects flags: none
bool mos6502::PHP()
{
	write(0x0100 + SP--, P | B | U);

	setFlagStatus(B, false);

	return false;
}


// PuLl Accumulator
// Pull A
// Affects flags: N,Z
bool mos6502::PLA()
{
	A = read(0x0100 + (++SP));

	setFlagStatus(Z, A == 0x00);
	setFlagStatus(N, A & 0x80);

	return false;
}


// PuLl Processor status
// Pull P
// Affects flags: U,B
bool mos6502::PLP()
{
	P = read(0x0100 + ++SP);

	setFlagStatus(B, false);
	setFlagStatus(U, true);

	return false;
}


// ROtate Left
// {fetched} = ({fetched} << 1) | C
// Affects flags: N,Z,C
bool mos6502::ROL()
{
	uint8_t old_fetched = fetched;

	fetched <<= 1;
	fetched |= getFlagStatus(C);

	setFlagStatus(C, old_fetched & 0x80);
	setFlagStatus(Z, fetched == 0x00);
	setFlagStatus(N, fetched & 0x80);

	if (lookup[opcode].address_mode == &mos6502::ACC)
		A = fetched;
	else
		write(abs_address, fetched);

	return false;
}


// ROtate Right
// {fetched} = ({fetched} >> 1) | (C << 7)
// Affects flags: N,Z,C
bool mos6502::ROR()
{
	uint8_t old_fetched = fetched;

	fetched >>= 1;
	fetched |= getFlagStatus(C) ? 0x80 : 0;

	setFlagStatus(C, old_fetched & 1);
	setFlagStatus(Z, fetched == 0);
	setFlagStatus(N, fetched & 0x80);

	if (lookup[opcode].address_mode == &mos6502::ACC)
		A = fetched;
	else
		write(abs_address, fetched);

	return false;
}


// ReTurn from Interrupt
// Pull P, Pull PC
// Affects flags: U,B
bool mos6502::RTI()
{
	P = read(0x0100 + ++SP);

	uint16_t lo, hi;
	lo = read(0x0100 + ++SP);
	hi = read(0x0100 + ++SP);
	PC = (hi << 8) | lo;

	setFlagStatus(B, false);
	setFlagStatus(U, true);

	return false;
}


// ReTurn from Subroutine
// Pull PC, PC = PC + 1
// Affects flags: none
bool mos6502::RTS()
{
	uint16_t lo, hi;
	lo = read(0x0100 + ++SP);
	hi = read(0x0100 + ++SP);
	PC = (hi << 8) | lo;
	++PC;

	return false;
}


// SuBtract with Carry
// A = A - {fetched} - (1 - C)
// Affects flags: V,N,Z,C
// Can require another cycle
bool mos6502::SBC()
{
#ifdef BCD_SUPPORTED
	if (!getFlagStatus(D)) 
	{
#endif
		uint8_t old_A = A;
		uint16_t result = static_cast<uint16_t>(A) + (static_cast<uint16_t>(fetched) ^ 0x00FF) + getFlagStatus(C);
		A = result & 0x00FF;

		setFlagStatus(C, result & 0xFF00);
		setFlagStatus(Z, (result & 0xFF) == 0x00);
		setFlagStatus(N, result & 0x80);
		setFlagStatus(V, ((~(static_cast<uint16_t>(old_A) ^ (static_cast<uint16_t>(fetched) ^ 0x00FF))) & (static_cast<uint16_t>(old_A) ^ result)) & 0x80);
#ifdef BCD_SUPPORTED
	}
	else
	{
		// Implementation from http://www.6502.org/tutorials/decimal_mode.html#A
		uint8_t old_A = A;
		uint16_t temp = A - fetched + getFlagStatus(C) - 1;

		if (A + getFlagStatus(C) < fetched + 1)
			temp -= 0x60;

		if ((A & 0x0F) + getFlagStatus(C) < (fetched & 0x0F) + 1)
			temp -= 0x06;

		A = temp & 0xFF;

		uint16_t bin_result = static_cast<uint16_t>(old_A) + (static_cast<uint16_t>(fetched) ^ 0x00FF) + (getFlagStatus(C) ? 1 : 0);

		setFlagStatus(C, bin_result & 0xFF00);
		setFlagStatus(Z, (bin_result & 0xFF) == 0x00);
		setFlagStatus(N, bin_result & 0x80);
		setFlagStatus(V, ((~(static_cast<uint16_t>(old_A) ^ (static_cast<uint16_t>(fetched) ^ 0x00FF))) & (static_cast<uint16_t>(old_A) ^ bin_result)) & 0x80);
	}
#endif

	return true;
}


// SEt Carry
// C = 1
// Affects flags: C
bool mos6502::SEC()
{
	setFlagStatus(C, true);

	return false;
}


// SEt Decimal
// D = 1
// Affects flags: D
bool mos6502::SED()
{
	setFlagStatus(D, true);

	return 0;
}


// SEt Interrupt
// I = 1
// Affects flags: I
bool mos6502::SEI()
{
	setFlagStatus(I, true);

	return false;
}


// STore Accumulator
// {address} = A
// Affects flags: none
bool mos6502::STA()
{
	write(abs_address, A);

	return false;
}


// STore X register
// {address} = X
// Affects flags: none
bool mos6502::STX()
{
	write(abs_address, X);

	return false;
}


// STore Y register
// {address} = Y
// Affects flags: none
bool mos6502::STY()
{
	write(abs_address, Y);

	return false;
}


// Transfer A to X
// X = A
// Affects flags: N,Z
bool mos6502::TAX()
{
	X = A;

	setFlagStatus(Z, X == 0x00);
	setFlagStatus(N, X & 0x80);

	return false;
}


// Transfer A to Y
// Y = A
// Affects flags: N,Z
bool mos6502::TAY()
{
	Y = A;

	setFlagStatus(Z, Y == 0x00);
	setFlagStatus(N, Y & 0x80);

	return false;
}


// Transfer Stack pointer to X
// X = SP
// Affects flags: N,Z
bool mos6502::TSX()
{
	X = SP;

	setFlagStatus(Z, X == 0x00);
	setFlagStatus(N, X & 0x80);

	return false;
}


// Transfer X to A
// A = X
// Affects flags: N,Z
bool mos6502::TXA()
{
	A = X;

	setFlagStatus(Z, A == 0x00);
	setFlagStatus(N, A & 0x80);

	return false;
}


// Transfer X to Stack pointer
// SP = X
// Affects flags: none
bool mos6502::TXS()
{
	SP = X;

	return false;
}


// Transfer Y to A
// A = Y
// Affects flags: N,Z
bool mos6502::TYA()
{
	A = Y;

	setFlagStatus(Z, A == 0x00);
	setFlagStatus(N, A & 0x80);

	return false;
}