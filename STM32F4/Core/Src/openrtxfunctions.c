#include "openrtxfunctions.h"
#include "ST7735S.h"

void delayUs(unsigned int useconds)
{
    // This delay has been calibrated to take x microseconds
    // It is written in assembler to be independent on compiler optimization
		__asm volatile(
				"mov   r1, #24          \n"          // Move 24 into r1
				"mul   r2, %0, r1       \n"          // Multiply input useconds by 24 and store in r2
				"mov   r1, #0           \n"          // Initialize r1 to 0
		"___loop_u: "
				"cmp   r1, r2           \n"          // Compare r1 with r2 (useconds * 24)
				"itt   lo               \n"          // If condition "lo" (less) is true
				"addlo r1, r1, #1       \n"          // If less, increment r1
				"blo   ___loop_u        \n"          // If below (blo), loop again
				:                         // No output operands
				: "r"(useconds)           // Input operand: useconds passed in a general-purpose register
				: "r1", "r2"              // Clobbered registers: r1 and r2 are modified
		);

}

void delayMs(unsigned int mseconds)
{
    register const unsigned int count=30000;

    for(unsigned int i=0;i<mseconds;i++)
    {
			__asm volatile(
					"mov   r1, #0         \n"        // Initialize R1 to 0
					"___loop_m: cmp   r1, %0     \n"  // Compare R1 with count (passed in the input)
					"itt   lo              \n"        // If condition "lo" (less) is true
					"addlo r1, r1, #1      \n"        // If less, increment R1
					"blo   ___loop_m       \n"        // If below (blo), loop
					:                           // Output operands (none)
					: "r"(count)                // Input operand: count passed in a general-purpose register (r0, r1, etc.)
					: "r1"                       // Clobbered registers: r1 is modified
			);
	}
}



