#ifndef TASKSCHEDULER_H
#define TASKSCHEDULER_H

#include "Tools.h"

class TaskScheduler : protected Tools
{
public:
	TaskScheduler();
	BOOL isTaskExists(const std::string& task_name);
	BOOL createTask(std::string binary_path = "", std::string task_name = "");
	BOOL deleteTask(const std::string& task_name);
};


#endif // !TASKSCHEDULER_H
