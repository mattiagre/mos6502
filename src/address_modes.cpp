///
/// Implementation of all addressing modes  
/// 

#include <cstdint>

#include "../mos6502.h"


// IMPlicit
// Data resides in the instruction itself (ex. CLC)
bool mos6502::IMP()
{
	return false; 
}


// ACCumulator 
// Data resides in the accumulator (ex. ASL A)
bool mos6502::ACC()
{
	fetched = A;
	return false; 
}


// IMMediate
// Data resides in the next byte
bool mos6502::IMM()
{
	abs_address = PC++;

	return false;
}


// Zero Page (0)
// Data resides in zero page, next byte contains an address in zero page
bool mos6502::ZP0()
{
	abs_address = read(PC++);
	abs_address &= 0x00FF;

	return false;
}


// Zero Page with offset X register
// Data resides in zero page, next byte is added with X to obtain a zero page address
bool mos6502::ZPX()
{
	abs_address = read(PC++) + X;
	abs_address &= 0x00FF;

	return false;
}


// Zero Page with offset Y register
// Data resides in zero page, next byte is added with Y to obtain a zero page address
bool mos6502::ZPY()
{
	abs_address = read(PC++) + Y;
	abs_address &= 0x00FF;

	return false; 
}


// ABSolute
// The two next bytes contains the address of the data
bool mos6502::ABS()
{
	uint16_t lo = read(PC++);
	uint16_t hi = read(PC++);

	abs_address = (hi << 8) | lo;

	return false;
}


// ABsolute with offset X register
// The two next bytes contains the address of the data, which is added with X
// If page boundaries are crossed, another cycle could be required
bool mos6502::ABX()
{
	uint16_t lo = read(PC++);
	uint16_t hi = read(PC++);

	abs_address = ((hi << 8) | lo) + X;

	if ((abs_address & 0xFF00) != (hi << 8))
		return true;

	return false;
}


// ABsolute with offset Y register
// The two next bytes contains the address of the data, which is added with Y
// If page boundaries are crossed, another cycle could be required
bool mos6502::ABY()
{
	uint16_t lo = read(PC++);
	uint16_t hi = read(PC++);

	abs_address = ((hi << 8) | lo) + Y;

	if ((abs_address & 0xFF00) != (hi << 8))
		return true;

	return false;
}


// INDirect
// The next two bytes points to the lower byte of an address
bool mos6502::IND()
{
	uint16_t lo = read(PC++);
	uint16_t hi = read(PC++);

	uint16_t ptr = (hi << 8) | lo;

	// Bug simulation. If page boundaries are crossed, addresses read
	// are xxFF and xx00 instead of xxFF and (xx+1)00
	if (lo == 0x00FF)
		abs_address = (read(ptr & 0xFF00) << 8) | read(ptr);

	else
		abs_address = (read(ptr + 1) << 8) | read(ptr);

	return false;
}


// IndeXed inDirect 
// The next byte contains an address in zero page, which is 
// added to X to obtain the lower byte of an address
bool mos6502::IXD()
{
	uint16_t zeroPage = read(PC++);

	uint16_t lo = read((zeroPage + X) & 0x00FF);
	uint16_t hi = read((zeroPage + X + 1) & 0x00FF);

	abs_address = (hi << 8) | lo;

	return false; 
}


// Indirect (Y) inDexed
// The next byte contains an address in zero page that 
// points to the lower byte of an address that is added to Y
// If page boundaries are crossed, another cycle could be required
bool mos6502::IYD()
{
	uint16_t zeroPage = read(PC++);

	uint16_t lo = read(zeroPage & 0x00FF);
	uint16_t hi = read((zeroPage + 1) & 0x00FF);

	abs_address = ((hi << 8) | lo) + Y;

	if ((abs_address & 0xFF00) != (hi << 8))
		return true;

	return false; 
}


// RELative
// The next byte contains a relative address to
// be added to PC to get an absolute address
bool mos6502::REL()
{
	rel_address = read(PC++);

	// 8 bit ==> 16 bit
	if (rel_address & 0x80)
		rel_address |= 0xFF00;

	return false; 
}
