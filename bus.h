#pragma once

///
///	Implementation of a bus that provides 64 KiB of RAM
/// 

#include <cstdint>
#include <array>


class mos6502;


// A basic bus that provides 64 KiB of RAM
class Bus
{
public:

	// A pointer to the instance of 6502 CPU connected to the bus
	mos6502* cpu;


	// Writes a byte at that address
	void write(uint16_t address, uint8_t data);
	// Reads the byte located at that address
	uint8_t read(uint16_t address, bool readonly);
	
	// 64 KiB of RAM (initialized to 0) 
	std::array<uint8_t, 64 * 1024> ram{ };
};

