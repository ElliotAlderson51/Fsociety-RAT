#include <ntddk.h>


void deprotect()
{
	__asm
	{
		cli
		push eax				// Save EAX
		mov  eax, CR0			// Put CR0 contents into EAX
		and eax, 0FFFEFFFFh	    // Turn off write protection
		mov  CR0, eax			// Put back the value after the modifications
		pop  eax				// Load former EAX value
	}
}

void protect()
{
	__asm
	{
		push eax				// Save EAX
		mov  eax, CR0			// Put CR0 contents into EAX
		or eax, NOT 0FFFEFFFFh	// Turn on write protection
		mov  CR0, eax			// Put back the value after the modifications
		pop  eax				// Load former EAX value
		sti
	}
}