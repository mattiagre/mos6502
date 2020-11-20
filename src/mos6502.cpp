///
/// Implementation of main processor's functionalities
/// 

#include <functional>
#include <cassert>

#include "../mos6502.h"


// Filling the opcodes lookup array
std::array<mos6502::Instruction, 256> mos6502::lookup {
/*						   0										 1										   2										 3										   4										 5										   6									     7										   8									     9										   A										 B										   C										 D										   E										 F						*/
/* 0 */{{"BRK", &mos6502::BRK, &mos6502::IMP, 7}, {"ORA", &mos6502::ORA, &mos6502::IXD, 6}, {"KIL", &mos6502::KIL, &mos6502::IMP, 2}, {"SLO", &mos6502::SLO, &mos6502::IXD, 8}, {"NOP", &mos6502::NOP, &mos6502::ZP0, 3}, {"ORA", &mos6502::ORA, &mos6502::ZP0, 3}, {"ASL", &mos6502::ASL, &mos6502::ZP0, 5}, {"SLO", &mos6502::SLO, &mos6502::ZP0, 5}, {"PHP", &mos6502::PHP, &mos6502::IMP, 3}, {"ORA", &mos6502::ORA, &mos6502::IMM, 2}, {"ASL", &mos6502::ASL, &mos6502::ACC, 2}, {"ANC", &mos6502::ANC, &mos6502::IMM, 2}, {"NOP", &mos6502::NOP, &mos6502::ABS, 4}, {"ORA", &mos6502::ORA, &mos6502::ABS, 4}, {"ASL", &mos6502::ASL, &mos6502::ABS, 6}, {"SLO", &mos6502::SLO, &mos6502::ABS, 6},
/* 1 */	{"BPL", &mos6502::BPL, &mos6502::REL, 2}, {"ORA", &mos6502::ORA, &mos6502::IYD, 5}, {"KIL", &mos6502::KIL, &mos6502::IMP, 2}, {"SLO", &mos6502::SLO, &mos6502::IYD, 8}, {"NOP", &mos6502::NOP, &mos6502::ZPX, 4}, {"ORA", &mos6502::ORA, &mos6502::ZPX, 4}, {"ASL", &mos6502::ASL, &mos6502::ZPX, 6}, {"SLO", &mos6502::SLO, &mos6502::ZPX, 6}, {"CLC", &mos6502::CLC, &mos6502::IMP, 2}, {"ORA", &mos6502::ORA, &mos6502::ABY, 4}, {"NOP", &mos6502::NOP, &mos6502::IMP, 2}, {"SLO", &mos6502::SLO, &mos6502::ABY, 7}, {"NOP", &mos6502::NOP, &mos6502::ABX, 4}, {"ORA", &mos6502::ORA, &mos6502::ABX, 4}, {"ASL", &mos6502::ASL, &mos6502::ABX, 7}, {"SLO", &mos6502::SLO, &mos6502::ABX, 7},
/* 2 */	{"JSR", &mos6502::JSR, &mos6502::ABS, 6}, {"AND", &mos6502::AND, &mos6502::IXD, 6}, {"KIL", &mos6502::KIL, &mos6502::IMP, 2}, {"RLA", &mos6502::RLA, &mos6502::IXD, 8}, {"BIT", &mos6502::BIT, &mos6502::ZP0, 3}, {"AND", &mos6502::AND, &mos6502::ZP0, 3}, {"ROL", &mos6502::ROL, &mos6502::ZP0, 5}, {"RLA", &mos6502::RLA, &mos6502::ZP0, 5}, {"PLP", &mos6502::PLP, &mos6502::IMP, 4}, {"AND", &mos6502::AND, &mos6502::IMM, 2}, {"ROL", &mos6502::ROL, &mos6502::ACC, 2}, {"ANC", &mos6502::ANC, &mos6502::IMM, 2}, {"BIT", &mos6502::BIT, &mos6502::ABS, 4}, {"AND", &mos6502::AND, &mos6502::ABS, 4}, {"ROL", &mos6502::ROL, &mos6502::ABS, 6}, {"RLA", &mos6502::RLA, &mos6502::ABS, 6},
/* 3 */	{"BMI", &mos6502::BMI, &mos6502::REL, 2}, {"AND", &mos6502::AND, &mos6502::IYD, 5}, {"KIL", &mos6502::KIL, &mos6502::IMP, 2}, {"RLA", &mos6502::RLA, &mos6502::IYD, 8}, {"NOP", &mos6502::NOP, &mos6502::ZPX, 4}, {"AND", &mos6502::AND, &mos6502::ZPX, 4}, {"ROL", &mos6502::ROL, &mos6502::ZPX, 6}, {"RLA", &mos6502::RLA, &mos6502::ZPX, 6}, {"SEC", &mos6502::SEC, &mos6502::IMP, 2}, {"AND", &mos6502::AND, &mos6502::ABY, 4}, {"NOP", &mos6502::NOP, &mos6502::IMP, 2}, {"RLA", &mos6502::RLA, &mos6502::ABY, 7}, {"NOP", &mos6502::NOP, &mos6502::ABX, 4}, {"AND", &mos6502::AND, &mos6502::ABX, 4}, {"ROL", &mos6502::ROL, &mos6502::ABX, 7}, {"RLA", &mos6502::RLA, &mos6502::ABX, 7},
/* 4 */	{"RTI", &mos6502::RTI, &mos6502::IMP, 6}, {"EOR", &mos6502::EOR, &mos6502::IXD, 6}, {"KIL", &mos6502::KIL, &mos6502::IMP, 2}, {"SRE", &mos6502::SRE, &mos6502::IXD, 8}, {"NOP", &mos6502::NOP, &mos6502::ZP0, 3}, {"EOR", &mos6502::EOR, &mos6502::ZP0, 3}, {"LSR", &mos6502::LSR, &mos6502::ZP0, 5}, {"SRE", &mos6502::SRE, &mos6502::ZP0, 5}, {"PHA", &mos6502::PHA, &mos6502::IMP, 3}, {"EOR", &mos6502::EOR, &mos6502::IMM, 2}, {"LSR", &mos6502::LSR, &mos6502::ACC, 2}, {"ASR", &mos6502::ASR, &mos6502::IMM, 2}, {"JMP", &mos6502::JMP, &mos6502::ABS, 3}, {"EOR", &mos6502::EOR, &mos6502::ABS, 4}, {"LSR", &mos6502::LSR, &mos6502::ABS, 6}, {"SRE", &mos6502::SRE, &mos6502::ABS, 6},
/* 5 */	{"BVC", &mos6502::BVC, &mos6502::REL, 2}, {"EOR", &mos6502::EOR, &mos6502::IYD, 5}, {"KIL", &mos6502::KIL, &mos6502::IMP, 2}, {"SRE", &mos6502::SRE, &mos6502::IYD, 8}, {"NOP", &mos6502::NOP, &mos6502::ZPX, 4}, {"EOR", &mos6502::EOR, &mos6502::ZPX, 4}, {"LSR", &mos6502::LSR, &mos6502::ZPX, 6}, {"SRE", &mos6502::SRE, &mos6502::ZPX, 6}, {"CLI", &mos6502::CLI, &mos6502::IMP, 2}, {"EOR", &mos6502::EOR, &mos6502::ABY, 4}, {"NOP", &mos6502::NOP, &mos6502::ACC, 2}, {"SRE", &mos6502::SRE, &mos6502::ABY, 7}, {"NOP", &mos6502::NOP, &mos6502::ABX, 4}, {"EOR", &mos6502::EOR, &mos6502::ABX, 4}, {"LSR", &mos6502::LSR, &mos6502::ABX, 7}, {"SRE", &mos6502::SRE, &mos6502::ABX, 7},
/* 6 */	{"RTS", &mos6502::RTS, &mos6502::IMP, 6}, {"ADC", &mos6502::ADC, &mos6502::IXD, 6}, {"KIL", &mos6502::KIL, &mos6502::IMP, 2}, {"RRA", &mos6502::RRA, &mos6502::IXD, 8}, {"NOP", &mos6502::NOP, &mos6502::ZP0, 3}, {"ADC", &mos6502::ADC, &mos6502::ZP0, 3}, {"ROR", &mos6502::ROR, &mos6502::ZP0, 5}, {"RRA", &mos6502::RRA, &mos6502::ZP0, 5}, {"PLA", &mos6502::PLA, &mos6502::IMP, 4}, {"ADC", &mos6502::ADC, &mos6502::IMM, 2}, {"ROR", &mos6502::ROR, &mos6502::ACC, 2}, {"ARR", &mos6502::ARR, &mos6502::IMM, 2}, {"JMP", &mos6502::JMP, &mos6502::IND, 5}, {"ADC", &mos6502::ADC, &mos6502::ABS, 4}, {"ROR", &mos6502::ROR, &mos6502::ABS, 6}, {"RRA", &mos6502::RRA, &mos6502::ABS, 6},
/* 7 */	{"BVS", &mos6502::BVS, &mos6502::REL, 2}, {"ADC", &mos6502::ADC, &mos6502::IYD, 5}, {"KIL", &mos6502::KIL, &mos6502::IMP, 2}, {"RRA", &mos6502::RRA, &mos6502::IYD, 8}, {"NOP", &mos6502::NOP, &mos6502::ZPX, 4}, {"ADC", &mos6502::ADC, &mos6502::ZPX, 4}, {"ROR", &mos6502::ROR, &mos6502::ZPX, 6}, {"RRA", &mos6502::RRA, &mos6502::ZPX, 6}, {"SEI", &mos6502::SEI, &mos6502::IMP, 2}, {"ADC", &mos6502::ADC, &mos6502::ABY, 4}, {"NOP", &mos6502::NOP, &mos6502::ACC, 2}, {"RRA", &mos6502::RRA, &mos6502::ABY, 7}, {"NOP", &mos6502::NOP, &mos6502::ABX, 4}, {"ADC", &mos6502::ADC, &mos6502::ABX, 4}, {"ROR", &mos6502::ROR, &mos6502::ABX, 7}, {"RRA", &mos6502::RRA, &mos6502::ABX, 7},
/* 8 */	{"NOP", &mos6502::NOP, &mos6502::IMP, 2}, {"STA", &mos6502::STA, &mos6502::IXD, 6}, {"KIL", &mos6502::KIL, &mos6502::IMP, 2}, {"AAX", &mos6502::AAX, &mos6502::IXD, 6}, {"STY", &mos6502::STY, &mos6502::ZP0, 3}, {"STA", &mos6502::STA, &mos6502::ZP0, 3}, {"STX", &mos6502::STX, &mos6502::ZP0, 3}, {"AAX", &mos6502::AAX, &mos6502::ZP0, 3}, {"DEY", &mos6502::DEY, &mos6502::IMP, 2}, {"NOP", &mos6502::NOP, &mos6502::IMM, 2}, {"TXA", &mos6502::TXA, &mos6502::IMP, 2}, {"XAA", &mos6502::XAA, &mos6502::IMM, 2}, {"STY", &mos6502::STY, &mos6502::ABS, 4}, {"STA", &mos6502::STA, &mos6502::ABS, 4}, {"STX", &mos6502::STX, &mos6502::ABS, 4}, {"AAX", &mos6502::AAX, &mos6502::ABS, 4},
/* 9 */	{"BCC", &mos6502::BCC, &mos6502::REL, 2}, {"STA", &mos6502::STA, &mos6502::IYD, 6}, {"KIL", &mos6502::KIL, &mos6502::IMP, 2}, {"AXA", &mos6502::AXA, &mos6502::IYD, 6}, {"STY", &mos6502::STY, &mos6502::ZPX, 4}, {"STA", &mos6502::STA, &mos6502::ZPX, 4}, {"STX", &mos6502::STX, &mos6502::ZPY, 4}, {"AAX", &mos6502::AAX, &mos6502::ZPY, 4}, {"TYA", &mos6502::TYA, &mos6502::IMP, 2}, {"STA", &mos6502::STA, &mos6502::ABY, 5}, {"TXS", &mos6502::TXS, &mos6502::IMP, 2}, {"TAS", &mos6502::TAS, &mos6502::ABY, 5}, {"NOP", &mos6502::NOP, &mos6502::IMP, 4}, {"STA", &mos6502::STA, &mos6502::ABX, 5}, {"SXA", &mos6502::SXA, &mos6502::ABY, 5}, {"AXA", &mos6502::AXA, &mos6502::ABY, 5},
/* A */	{"LDY", &mos6502::LDY, &mos6502::IMM, 2}, {"LDA", &mos6502::LDA, &mos6502::IXD, 6}, {"LDX", &mos6502::LDX, &mos6502::IMM, 2}, {"LAX", &mos6502::LAX, &mos6502::IXD, 6}, {"LDY", &mos6502::LDY, &mos6502::ZP0, 3}, {"LDA", &mos6502::LDA, &mos6502::ZP0, 3}, {"LDX", &mos6502::LDX, &mos6502::ZP0, 3}, {"LAX", &mos6502::LAX, &mos6502::ZP0, 3}, {"TAY", &mos6502::TAY, &mos6502::IMP, 2}, {"LDA", &mos6502::LDA, &mos6502::IMM, 2}, {"TAX", &mos6502::TAX, &mos6502::IMP, 2}, {"ATX", &mos6502::ATX, &mos6502::IMM, 2}, {"LDY", &mos6502::LDY, &mos6502::ABS, 4}, {"LDA", &mos6502::LDA, &mos6502::ABS, 4}, {"LDX", &mos6502::LDX, &mos6502::ABS, 4}, {"LAX", &mos6502::LAX, &mos6502::ABS, 4},
/* B */	{"BCS", &mos6502::BCS, &mos6502::REL, 2}, {"LDA", &mos6502::LDA, &mos6502::IYD, 5}, {"KIL", &mos6502::KIL, &mos6502::IMM, 2}, {"LAX", &mos6502::LAX, &mos6502::IYD, 5}, {"LDY", &mos6502::LDY, &mos6502::ZPX, 4}, {"LDA", &mos6502::LDA, &mos6502::ZPX, 4}, {"LDX", &mos6502::LDX, &mos6502::ZPY, 4}, {"LAX", &mos6502::LAX, &mos6502::ZPY, 4}, {"CLV", &mos6502::CLV, &mos6502::IMP, 2}, {"LDA", &mos6502::LDA, &mos6502::ABY, 4}, {"TSX", &mos6502::TSX, &mos6502::IMP, 2}, {"LAS", &mos6502::LAS, &mos6502::ABY, 7}, {"LDY", &mos6502::LDY, &mos6502::ABX, 4}, {"LDA", &mos6502::LDA, &mos6502::ABX, 4}, {"LDX", &mos6502::LDX, &mos6502::ABY, 4}, {"LAX", &mos6502::LAX, &mos6502::ABY, 4},
/* C */	{"CPY", &mos6502::CPY, &mos6502::IMM, 2}, {"CMP", &mos6502::CMP, &mos6502::IXD, 6}, {"NOP", &mos6502::NOP, &mos6502::IMP, 2}, {"DCP", &mos6502::DCP, &mos6502::IXD, 8}, {"CPY", &mos6502::CPY, &mos6502::ZP0, 3}, {"CMP", &mos6502::CMP, &mos6502::ZP0, 3}, {"DEC", &mos6502::DEC, &mos6502::ZP0, 5}, {"DCP", &mos6502::DCP, &mos6502::ZP0, 5}, {"INY", &mos6502::INY, &mos6502::IMP, 2}, {"CMP", &mos6502::CMP, &mos6502::IMM, 2}, {"DEX", &mos6502::DEX, &mos6502::IMP, 2}, {"SAX", &mos6502::SAX, &mos6502::IMM, 2}, {"CPY", &mos6502::CPY, &mos6502::ABS, 4}, {"CMP", &mos6502::CMP, &mos6502::ABS, 4}, {"DEC", &mos6502::DEC, &mos6502::ABS, 6}, {"DCP", &mos6502::DCP, &mos6502::ABS, 6},
/* D */	{"BNE", &mos6502::BNE, &mos6502::REL, 2}, {"CMP", &mos6502::CMP, &mos6502::IYD, 5}, {"KIL", &mos6502::KIL, &mos6502::IMP, 2}, {"DCP", &mos6502::DCP, &mos6502::IYD, 8}, {"NOP", &mos6502::NOP, &mos6502::ZPX, 4}, {"CMP", &mos6502::CMP, &mos6502::ZPX, 4}, {"DEC", &mos6502::DEC, &mos6502::ZPX, 6}, {"DCP", &mos6502::DCP, &mos6502::ZPX, 6}, {"CLD", &mos6502::CLD, &mos6502::IMP, 2}, {"CMP", &mos6502::CMP, &mos6502::ABY, 4}, {"NOP", &mos6502::NOP, &mos6502::IMP, 2}, {"DCP", &mos6502::DCP, &mos6502::ABY, 7}, {"NOP", &mos6502::NOP, &mos6502::ABX, 4}, {"CMP", &mos6502::CMP, &mos6502::ABX, 4}, {"DEC", &mos6502::DEC, &mos6502::ABX, 7}, {"DCP", &mos6502::DCP, &mos6502::ABX, 7},
/* E */	{"CPX", &mos6502::CPX, &mos6502::IMM, 2}, {"SBC", &mos6502::SBC, &mos6502::IXD, 6}, {"NOP", &mos6502::NOP, &mos6502::IMM, 2}, {"ISC", &mos6502::ISC, &mos6502::IXD, 8}, {"CPX", &mos6502::CPX, &mos6502::ZP0, 3}, {"SBC", &mos6502::SBC, &mos6502::ZP0, 3}, {"INC", &mos6502::INC, &mos6502::ZP0, 5}, {"ISC", &mos6502::ISC, &mos6502::ZP0, 5}, {"INX", &mos6502::INX, &mos6502::IMP, 2}, {"SBC", &mos6502::SBC, &mos6502::IMM, 2}, {"NOP", &mos6502::NOP, &mos6502::IMP, 2}, {"SBC", &mos6502::SBC, &mos6502::IMM, 2}, {"CPX", &mos6502::CPX, &mos6502::ABS, 4}, {"SBC", &mos6502::SBC, &mos6502::ABS, 4}, {"INC", &mos6502::INC, &mos6502::ABS, 6}, {"ISC", &mos6502::ISC, &mos6502::ABS, 6},
/* F */	{"BEQ", &mos6502::BEQ, &mos6502::REL, 2}, {"SBC", &mos6502::SBC, &mos6502::IYD, 5}, {"KIL", &mos6502::KIL, &mos6502::IMP, 2}, {"ISC", &mos6502::ISC, &mos6502::IYD, 8}, {"NOP", &mos6502::NOP, &mos6502::ZPX, 4}, {"SBC", &mos6502::SBC, &mos6502::ZPX, 4}, {"INC", &mos6502::INC, &mos6502::ZPX, 6}, {"ISC", &mos6502::ISC, &mos6502::ZPX, 6}, {"SED", &mos6502::SED, &mos6502::IMP, 2}, {"SBC", &mos6502::SBC, &mos6502::ABY, 4}, {"NOP", &mos6502::NOP, &mos6502::IMP, 2}, {"ISC", &mos6502::ISC, &mos6502::ABY, 7}, {"NOP", &mos6502::NOP, &mos6502::ABX, 4}, {"SBC", &mos6502::SBC, &mos6502::ABX, 4}, {"INC", &mos6502::INC, &mos6502::ABX, 7}, {"ISC", &mos6502::ISC, &mos6502::ABX, 7} } 
};


mos6502::mos6502(Bus* bus)
	: bus{bus}
{ }


bool mos6502::getFlagStatus(Flags flag) 
{
	return P & flag;
}


void mos6502::setFlagStatus(Flags flag, bool set) 
{
	if (set) 
		P |= flag;
	else 
		P &= ~flag;
}


// A = X = Y = 0, P = %00100100, SP = 0xFD, PC = {FFFD} << 8 | {FFFC}
// Takes 7 cycles 
void mos6502::reset() 
{
	A  = 0x00;
	X  = 0x00;
	Y  = 0x00;
	P  = 0x00 | U | I;
	SP = 0xFD;

	uint16_t lo = read(0xFFFC);
	uint16_t hi = read(0xFFFD);
	PC = (hi << 8) | lo;

	abs_address = 0;
	rel_address = 0;
	
	cycles = 7;
}


// Push PC, push P, PC = {FFFF} << 8 | {FFFE}, set I flag
// Takes 7 cycles 
void mos6502::irq() 
{
	if (!getFlagStatus(I)) 
	{
		write(0x0100 + SP--, (PC >> 8) & 0x00FF);
		write(0x0100 + SP--, PC & 0x00FF);

		setFlagStatus(B, false);
	
		write(0x0100 + SP--, P);

		
		uint16_t lo = read(0xFFFE);
		uint16_t hi = read(0xFFFF);
		PC = (hi << 8) | lo;

		setFlagStatus(I, true);

		cycles = 7;
	}
}


// Push PC, Push P, PC = {FFFB} << 8 | {FFFA}, set I flag
void mos6502::nmi() 
{
	write(0x0100 + SP--, (PC >> 8) & 0x00FF);
	write(0x0100 + SP--, PC & 0x00FF);

	setFlagStatus(B, false);

	write(0x0100 + SP--, P);

	uint16_t lo = read(0xFFFA);
	uint16_t hi = read(0xFFFB);
	PC = (hi << 8) | lo;

	setFlagStatus(I, true);

	cycles = 7;
}


bool mos6502::clock()
{
	if (cycles == 0) 
	{
		opcode = read(PC++);
		
		cycles = lookup[opcode].cycles;

		bool clck1 = std::invoke(lookup[opcode].address_mode, *this);

		fetched = fetch();

		bool clck2 = std::invoke(lookup[opcode].operation, *this);

		// If needed, add another cycle
		if (clck1 && clck2)
			++cycles;
	}

	--cycles; 

	// Increment the number of cycles executed
	++clock_count;

	return cycles == 0;
}


uint8_t mos6502::fetch() 
{
	if (lookup[opcode].address_mode != &mos6502::IMP) 
	{
		if (lookup[opcode].address_mode == &mos6502::ACC)
			return A;
		
		return read(abs_address);
	}
	else
		return 0;
}


void mos6502::write(uint16_t address, uint8_t data)
{
	bus->write(address, data);
}


uint8_t mos6502::read(uint16_t address)
{
	return bus->read(address, true);
}