// Inline Kernel Hook
#pragma warning (disable : 4309)
#pragma warning (disable : 4996)

#include <ntddk.h>

typedef struct _KERNEL_HOOK
{
	PVOID FunctionAddress;
	PVOID Hook;
	char jmp[5];
	char OrigBytes[5];
	PVOID OrigFunction;
}KERNEL_HOOK, * PKERNEL_HOOK;

PVOID KhGetFunctionAddress(PWSTR FunctionName)
{
	UNICODE_STRING usFunctionName;

	RtlInitUnicodeString(&usFunctionName, FunctionName);
	return MmGetSystemRoutineAddress(&usFunctionName);
}

BOOLEAN KhInitHook(PKERNEL_HOOK Hook, PVOID Address, PVOID HookFunction)
{
	ULONG OrigFunction, FunctionAddress;

	Hook->FunctionAddress = Address;
	Hook->OrigFunction = ExAllocatePool(NonPagedPool, 4096);

	if (!Hook->OrigFunction)
	{
		return FALSE;
	}

	memcpy(Hook->OrigBytes, Address, 5);
	memcpy(Hook->OrigFunction, Hook->OrigBytes, 5);

	Hook->jmp[0] = 0xe9; // First OpCode in jmp instruction
	*(PULONG)&Hook->jmp[1] = (ULONG)HookFunction - (ULONG)Address - 5;

	OrigFunction = (ULONG)Hook->OrigFunction + 5;
	FunctionAddress = (ULONG)Hook->FunctionAddress + 5;

	*(PUCHAR)((PUCHAR)Hook->OrigFunction + 5) = 0xe9;
	*(PULONG)((PUCHAR)Hook->OrigFunction + 6) = FunctionAddress - OrigFunction - 5;

	return TRUE;
}

void KhStartHook(PKERNEL_HOOK Hook)
{
	deprotect();

	memcpy(Hook->FunctionAddress, Hook->jmp, 5);

	protect();
}

void KhUnhook(PKERNEL_HOOK Hook)
{
	deprotect();

	memcpy(Hook->FunctionAddress, Hook->OrigBytes, 5);

	protect();
}

void KhRemoveHook(PKERNEL_HOOK Hook)
{
	ExFreePool(Hook->OrigFunction);
	memset(Hook, 0, sizeof(KERNEL_HOOK));
}