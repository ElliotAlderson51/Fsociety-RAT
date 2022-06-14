/*
##################################################
#                                                #
#         Malware Coder: Elliot Alderson         #
#                                                #
#   Github: https://github.com/ElliotAlderson51  #
#                                                #
##################################################
*/

// I take the code from a Stackoverflow question: 
// https://stackoverflow.com/questions/41750144/c-how-to-detect-the-virtual-machine-your-application-is-running-in-has-focus

#ifndef ANTIVM_H
#define ANTIVM_H

#include <Windows.h>

namespace ANTIVM
{
	//Check if the software running on VM
    bool IsVMWare()
    {
        bool res = true;

        __try
        {
            __asm
            {
                push   edx
                push   ecx
                push   ebx

                mov    eax, 'VMXh'
                mov    ebx, 0
                mov    ecx, 10 // get VMWare version
                mov    edx, 'VX' // port number

                in     eax, dx // read port
                               // on return EAX returns the VERSION

                cmp    ebx, 'VMXh' // compare with target
                setz[res] // set return value

                pop    ebx
                pop    ecx
                pop    edx
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            res = false;
        }

        return res;
    }
}

#endif // !ANTIVM_H
