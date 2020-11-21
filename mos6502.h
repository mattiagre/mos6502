#pragma once

///
/// Implementation of the MOS 6502 CPU
/// 


#include <cstdint>
#include <string>
#include <array>
#include <map>

#include "bus.h"

// Binary Coded Decimal support enabled 
#define BCD_SUPPORTED


// A MOS 6502 processor
class mos6502 
{
public:

	mos6502() = default;
	mos6502(Bus* bus);

	// Executes a single clock cycles 
	// Returns true if the processor has finished the current opcode
	bool clock();

	/*									 */
	/*			  Interrupts		     */
	/*									 */

	// Brings the CPU to a known state 
	void reset();
	// Requests an maskable interrupt 
	void irq();
	// Executes an non-maskable interrupt
	void nmi();


public:

	// A disassambly routine that maps every address from start_addr to end_addr to std::string
	// that contains the instruction in that address 
	std::map<uint16_t, std::string> disassemble(uint16_t start_addr, uint16_t end_addr) const;

public:

	/*									  */		
	/*			   Registers	          */
	/*									  */	
	uint8_t   A  = 0x00; // Accumulator 
	uint8_t	  X  = 0x00; // X index
	uint8_t	  Y  = 0x00; // Y index
	uint8_t	  P	 = 0x00; // Status register
	uint8_t	  SP = 0x00; // Stack Pointer
	uint16_t  PC = 0x00; // Program Counter


public:
	// Flags of the status register
	// bit: 7 6 5 4 3 2 1 0 
	//		N V U B D I Z C
	//		| | | | | | | |
	//		| |	| | | | | +-> Carry 
	//		| | | | | | +---> Zero
	//		| | | | | +-----> Interrupt Disable
	//		| | | | +-------> Decimal 
	//		| | | +---------> Break
	//		| | +-----------> Unused
	//		| +-------------> Overflow
	//		+---------------> Negative
	enum Flags {
		C = 1 << 0,	// Carry bit
		Z = 1 << 1,	// The result is zero
		I = 1 << 2, // Interrupt disabled
		D = 1 << 3, // Decimal mode
		B = 1 << 4,	// Break
		U = 1 << 5,	// Unused
		V = 1 << 6,	// The operation caused overflow
		N = 1 << 7	// The result is negative
	};

private:
	// Get the status of the flag (true: set, false: clear)
	bool getFlagStatus(Flags flag) const;
	// Sets or clears the flag according to set variable
	void setFlagStatus(Flags flag, bool set);
	

private:
	/*													          */	
	/*		     Useful variables used by the emulator            */	
	/*															  */

	// Current instruction's opcode read by clock()
	uint8_t  opcode		 = 0x00;
	// Number of clock cycles to execute before the next instruction
	uint8_t  cycles		 = 0x00; 
	// Absolute address calculated by the addressing mode
	uint16_t abs_address = 0x0000;
	// Relative address calculated by relative addressing mode
	uint16_t rel_address = 0x0000;
	// Last value fetched by clock()
	uint8_t fetched		 = 0x00;
public:
	// Number of clock cycles already executed
	uint32_t clock_count = 0;


private:
	/*									*/
	/*		   Addressing modes			*/
	/*									*/

	bool IMP();	// Implicit  
	bool ACC();	// Accumulator 
	bool IMM();	// Immediate
	bool ZP0();	// Page zero
	bool ZPX();	// Page zero, offset X
	bool ZPY();	// Page zero, offset Y
	bool ABS();	// Absolute
	bool ABX();	// Assolute, offset X
	bool ABY();	// Assolute, offset Y
	bool IND();	// Indirect
	bool IXD();	// Indexed Indirect
	bool IYD();	// Indirect Indexed
	bool REL();	// Relative

	// Returns the data to be processed by the instruction 
	uint8_t fetch();


private:
	/*											 */
	/*			    Instructions				 */
	/*											 */

	bool ADC(); bool AND(); bool ASL(); bool BCC();
	bool BCS(); bool BEQ(); bool BIT(); bool BMI(); 
	bool BNE(); bool BPL(); bool BRK(); bool BVC();
	bool BVS(); bool CLC(); bool CLD(); bool CLI();
	bool CLV(); bool CMP(); bool CPX(); bool CPY();
	bool DEC(); bool DEX(); bool DEY(); bool EOR();
	bool INC(); bool INX(); bool INY(); bool JMP();
	bool JSR(); bool LDA(); bool LDX(); bool LDY();
	bool LSR(); bool NOP(); bool ORA(); bool PHA();
	bool PHP(); bool PLA(); bool PLP(); bool ROL();
	bool ROR(); bool RTI(); bool RTS(); bool SBC();
	bool SEC(); bool SED(); bool SEI(); bool STA();
	bool STX(); bool STY(); bool TAX(); bool TAY();
	bool TSX(); bool TXA(); bool TXS(); bool TYA();


private:
	/*											 */
	/*		    (some) Illegal opcodes		     */
	/*											 */

	bool AAX(); bool ANC(); bool ARR(); bool ASR();
	bool ATX(); bool AXA(); bool DCP(); bool ISC(); 
	bool KIL(); bool LAS(); bool LAX(); bool RLA(); 
	bool RRA(); bool SAX(); bool SLO(); bool SRE(); 
	bool SXA(); bool SYA(); bool TAS(); bool XAA();

	
	/*                                       */
	/*                  Bus                  */
	/*                                       */

public:
	// The bus connected to the CPU
	Bus* bus;

private:
	// Writes a byte in at given address
	void write(uint16_t address, uint8_t data);
	// Reads the byte pointed by the address
	uint8_t read(uint16_t address) const;


private:

	/*																	   */
	/*		                  Instructions utilities				       */
	/*																	   */

	// Structure that represent an instruction
	struct Instruction {
		std::string name;				  // The instruction's name
		bool (mos6502::* operation)();	  // Pointer to instruction's routine
		bool (mos6502::* address_mode)(); // Pointer to an addressing mode
		uint8_t cycles;					  // Number of cycles required 
	};

	// Array that contains all instructions
	static std::array<Instruction, 256> lookup;
};

