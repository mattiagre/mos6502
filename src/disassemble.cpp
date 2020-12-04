///
/// Implementation of mos6502::disasseble function
/// 

#include <cstdint>
#include <sstream>
#include <iomanip>
#include <map>

#include "../mos6502.h"


// Converts a integer value to a hex-formatted string, with a custom prefix and length 
template <typename Integer, std::streamsize length = sizeof(Integer) * 2>
std::string to_hex(Integer value, const std::string& prefix = "$")
{
	static_assert(std::is_integral_v<Integer>, "The \"Integer\" type has to be a integral one");

	static char digits[] = "0123456789ABCDEF";
	std::string hex = prefix + std::string{length, '0'};
	for (std::size_t i = 0, j = (length - 1) * 4; i < length; ++i, j -= 4) 
		hex[prefix.length() + i] = digits[(value >> j) & 0x0F];
	return hex;
}


std::map<uint16_t, std::string> mos6502::disassemble(uint16_t start_addr, uint16_t end_addr) const
{
	std::map<uint16_t, std::string> result;

	for (auto address = start_addr; address < end_addr; )
	{
		// Save the address of the instruction to add the line at the end
		auto inst_address = address;

		std::stringstream line;

		// Read the instruction in memory at current address
		uint8_t opcode = read(address++);
		const Instruction& instr = lookup[opcode];

		// Add the instruction's to the line
		line << instr.name;
		
		// No need to add something: addressing mode is implied
		if (instr.address_mode == &mos6502::IMP);

		else if (instr.address_mode == &mos6502::ACC)
			line << " A";

		// Add the data as a 1 byte constant value
		else if (instr.address_mode == &mos6502::IMM)
		{
			uint8_t data = read(address++);
			line << " #" << to_hex(data);
		}

		// Add the address as 2 byte constant value
		else if (instr.address_mode == &mos6502::ABS)
		{
			uint16_t lo, hi;
			lo = read(address++);
			hi = read(address++);

			uint16_t data = (hi << 8) | lo;
			line << " " << to_hex(data);
		}

		// Add the address as 2 byte constant value
		else if (instr.address_mode == &mos6502::ABX)
		{
			uint16_t lo, hi;
			lo = read(address++);
			hi = read(address++);

			uint16_t data = (hi << 8) | lo;
			line << " " << to_hex(data) << ",X";
		}

		// Add the address as 2 byte constant value
		else if (instr.address_mode == &mos6502::ABY)
		{
			uint16_t lo, hi;
			lo = read(address++);
			hi = read(address++);

			uint16_t data = (hi << 8) | lo;
			line << " " << to_hex(data) << ",Y";
		}

		// Add the address as 1 byte constant value
		else if (instr.address_mode == &mos6502::ZP0)
		{
			uint8_t data = read(address++);
			line << " " << to_hex(data);
		}

		// Add the address as 1 byte constant value
		else if (instr.address_mode == &mos6502::ZPX)
		{
			uint8_t data = read(address++);
			line << " " << to_hex(data) << ",X";
		}

		// Add the address as 1 byte constant value
		else if (instr.address_mode == &mos6502::ZPY)
		{
			uint8_t data = read(address++);
			line << " " << to_hex(data) << ",Y";
		}

		// Add the address as 2 byte constant value
		else if (instr.address_mode == &mos6502::IND)
		{
			uint16_t lo, hi;
			lo = read(address++);
			hi = read(address++);

			uint16_t data = (hi << 8) | lo;
			line << " (" << to_hex(data) << ")";
		}

		// Add the address as 1 byte constant value
		else if (instr.address_mode == &mos6502::IXD)
		{
			uint8_t data = read(address++);
			line << " (" << to_hex(data) << ",X)";
		}

		// Add the address as 1 byte constant value
		else if (instr.address_mode == &mos6502::IYD)
		{
			uint8_t data = read(address++);
			line << " (" << to_hex(data) << "),Y";
		}

		// Add the offset as 1 byte constant value
		else if (instr.address_mode == &mos6502::REL)
		{
			uint16_t data = read(address++);
			if (data & 0x80)
				data |= 0xFF00;
			line << " " << to_hex<uint16_t>(address + data);
		}

		result[inst_address] = line.str();
	}

	return result;
}