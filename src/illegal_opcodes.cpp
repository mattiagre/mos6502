///
/// Implementation of (some) illegal opcodes
/// 

#include <cstdint>

#include "../mos6502.h"


// Constant for ATX, XAA instruction
constexpr uint8_t CONST = 0x00;


// STA + STX
// Affects flags: none
bool mos6502::AAX()
{
	uint8_t result = X & A;

	write(abs_address, result);

	return false;
}


// AND with C = N
// Affects flags: N,Z,C
bool mos6502::ANC()
{
	A &= fetched;

	// Set the carry flag as negative flag
	setFlagStatus(C, A & 0x80);
	setFlagStatus(Z, A == 0);
	setFlagStatus(N, A & 0x80);

	return false;
}


// AND + ROR
// Affects flags: N,V,Z,C
bool mos6502::ARR()
{
	A &= fetched;

	A >>= 1;

	// Bit 7 is setted as flag C (bit 0 is lost)
	A |= (getFlagStatus(C) ? 1 : 0) << 7;

	// Sets V and C flag in a different way
	setFlagStatus(C, A & (1 << 6));
	setFlagStatus(Z, A == 0x00);
	setFlagStatus(V, (A & (1 << 5) ^ (A & (1 << 6))));
	setFlagStatus(N, A & 0x80);

	return false;
}


// AND + LSR
// Affects flags: N,Z,C
bool mos6502::ASR()
{
	uint8_t old_A = A;

	A &= fetched;
	A >>= 1;

	setFlagStatus(C, old_A & 0x1);
	setFlagStatus(Z, fetched == 0x00);
	setFlagStatus(N, fetched & 0x80);

	return false;
}


// OR with {CONST} + AND
// X = A = (A | {CONST}) & {fetched}
// Affects flags: N,Z
bool mos6502::ATX()
{
	A |= CONST;
	// X register is also changed
	X = (A &= fetched);

	setFlagStatus(Z, A == 0x00);
	setFlagStatus(N, A & 0x80);

	return false;
}


// {address} =  A & X & (hi + 1)
// Affects flags: none
bool mos6502::AXA()
{
	uint8_t temp = A & X & (((abs_address >> 8) + 1) & 0x00FF);

	write(abs_address, temp);

	return false;
}


// DEC + CMP
// Affects flags: N,Z,C
bool mos6502::DCP()
{
	--fetched;
	write(abs_address, fetched);

	setFlagStatus(C, A >= fetched);
	setFlagStatus(Z, A == fetched);
	setFlagStatus(N, (A - fetched) & 0x80);

	return false;
}


// INC + SBC
// Affects flags: N,V,Z,C
bool mos6502::ISC()
{
	++fetched;
	write(abs_address, fetched);

	uint16_t subtraction = static_cast<uint16_t>(A) + (static_cast<uint16_t>(fetched) ^ 0x00FF) + getFlagStatus(C);
	A = subtraction & 0xFF;

	setFlagStatus(C, subtraction & 0xFF00);
	setFlagStatus(Z, (subtraction & 0xFF) == 0);
	setFlagStatus(N, subtraction & 0x80);
	setFlagStatus(V, ((~(static_cast<uint16_t>(A) ^ (static_cast<uint16_t>(fetched) ^ 0x00FF))) & (static_cast<uint16_t>(A) ^ subtraction)) & 0x80);

	return false;
}


// Stops the Program Counter
// Affects flags: none
bool mos6502::KIL()
{
	// Not implemented
	return false;
}


// A = X = SP = fetched & SP
// Affects flags: N,Z
bool mos6502::LAS()
{
	A = (X = (SP = (fetched &= SP)));

	setFlagStatus(Z, fetched == 0x00);
	setFlagStatus(N, fetched & 0x80);

	return false;
}


// LDA + TAX
// Affects flags: N,Z
bool mos6502::LAX()
{
	A = fetched;
	X = A;

	setFlagStatus(Z, A == 0x00);
	setFlagStatus(N, A & 0x80);

	return true;
}


// ROL + AND
// Affects flags: N,Z,C
bool mos6502::RLA()
{
	uint8_t old_fetched = fetched;

	fetched <<= 1;
	fetched |= getFlagStatus(C) ? 1 : 0;
	A &= fetched;

	setFlagStatus(C, old_fetched & 0x80);
	setFlagStatus(Z, A == 0x00);
	setFlagStatus(N, A & 0x80);

	write(abs_address, fetched);

	return false;
}


// ROR + ADC
// Affects flags: N,V,Z,C
bool mos6502::RRA()
{
	uint8_t old_fetched = fetched;

	fetched >>= 1;
	fetched |= getFlagStatus(C) ? 0x80 : 0;

	setFlagStatus(C, old_fetched & 0x01);
	setFlagStatus(Z, fetched == 0);
	setFlagStatus(N, fetched & 0x80);

	write(abs_address, fetched);

	uint16_t addition = static_cast<uint16_t>(A) + static_cast<uint16_t>(fetched) + getFlagStatus(C);

	setFlagStatus(C, addition > 0xFF);
	setFlagStatus(Z, (addition & 0xFF) == 0);
	setFlagStatus(V, ((~((uint16_t)A ^ (uint16_t)fetched)) & ((uint16_t)A ^ addition)) & 0x80);
	setFlagStatus(N, addition & 0x80);

	A = addition & 0xFF;

	return false;
}


// {address} = A & X
// Affects flags: none
bool mos6502::SAX()
{
	uint8_t temp = A & X;

	write(abs_address, temp);

	return false;
}


// ASL + ORA
// Affects flags: N,V,Z,C
bool mos6502::SLO()
{
	uint8_t old_fetched = fetched;

	fetched <<= 1;

	setFlagStatus(C, old_fetched & 0x80);
	setFlagStatus(Z, A == 0x00);
	setFlagStatus(N, A & 0x80);

	write(abs_address, fetched);

	A |= fetched;

	return false;
}


// LSR + EOR
// Affects flags: N,Z,C
bool mos6502::SRE()
{
	uint8_t old_fetched = fetched;

	fetched >>= 1;

	setFlagStatus(C, old_fetched & 0x01);

	write(abs_address, fetched);

	A ^= fetched;

	setFlagStatus(Z, A == 0);
	setFlagStatus(N, A & 0x80);

	return false;
}


// {address} = X & (hi + 1)
// Affects flags: none
bool mos6502::SXA()
{
	uint8_t temp = X & (((abs_address >> 8) + 1) & 0x00FF);

	write(abs_address, temp);

	return false;
}


// {address} = Y & (hi + 1)
// Affects flags: none
bool mos6502::SYA()
{
	uint8_t temp = Y & (((abs_address >> 8) + 1) & 0x00FF);

	write(abs_address, temp);

	return false;
}


// SP = A & X, {address} = SP & (hi + 1)
// Affects flags: none
bool mos6502::TAS()
{
	SP = A & X;
	uint8_t temp = SP & (((abs_address >> 8) + 1) & 0x00FF);

	write(abs_address, temp);

	return false;
}


// A = (A | CONST) & X & #imm
// Affects flags: N,Z
bool mos6502::XAA()
{
	A = ((A | CONST) & X) & fetched;

	setFlagStatus(Z, A == 0x00);
	setFlagStatus(N, A & 0x80);

	return false;
}
