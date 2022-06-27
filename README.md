# Fsociety-RAT
It was really fun to make this project! This project have a lot of great features and a very good website control for the bots.

The final Malware will bypassing UAC, installing rootkit on 32 bit systems, obfuscated strings, Anti-VM technique, Process Hollowing , Communicate with the website control using CURL (curl is preinstalled in every windows!). 

## RAT

### Malware Persistence Techniques

:heavy_check_mark: The malware hiding a DLL to bypass UAC and a Rootkit in new sections.    
:heavy_check_mark: Bypassing UAC using UAC hijacking on the program ComputerDefaults.exe in a "Windows " folder.    
:heavy_check_mark: Create a task for the malware in Task Schduler in the path "Microsoft\\Windows\\Security" with high privileges.    
:heavy_check_mark: If the system is 32-bit install and start the rootkit.    
:heavy_check_mark: If the system is 64-bit the malware will start Process Hollowing    

### Malware Attacks
The malware communicate with a control website, this is the current attacks:

:heavy_check_mark: Screenshot    
:heavy_check_mark: Task Scheduler       
:heavy_check_mark: Remote Shell    
:heavy_check_mark: File Manager    
:heavy_check_mark: File Explorer    
:heavy_check_mark: Keylogger    
:heavy_check_mark: Client Info    

### Website Control Images
Main website control - list the clients and show info about them

![plot](./Images/Main.PNG)

Victim profile page

![plot](./Images/Profile.PNG)


## Rootkit
The Rootkit is for 32-bit systems. https://github.com/ElliotAlderson51/Kernel-Rootkit-32Bit

### What the Rootkit can do?

#### Hook the SSDT

:heavy_check_mark: Protect Files (Read\Write\Create\Delete\Rename\Open\Execute)     
:heavy_check_mark: Hide Process    
:heavy_check_mark: Protect Process, Thread     
:heavy_check_mark: Protect Registry Keys (Open\Create\Delete\Set)        
:heavy_check_mark: Bypass privilege checks

## DLL - Bypass UAC
Create a DLL that will be hijacked to ComputerDefaults to start the malware with high privilege.        
https://github.com/ElliotAlderson51/Bypass-UAC

## section_injector
This Projects will inject a file to new section in another file.
```
Injector.exe "section name" "target" "file"
```

## Websites

### Fsociety Control Website
This is the Control Website the malware is communicate with to get the commands to execute.

### Fsociety Redirect Website
This website have hardcoded url in the malware code, the malware will get the Control Website url from this website.    
I create this website so if there is a problem with the Control Website you can just upload the Control Website again and change the url in this website.   

## Sub Projects
This solutions was helping me to build the final malware

### Initialize RAT
This automated the build of the final malware.

:heavy_check_mark: UPX the DLL file     
:heavy_check_mark: Inject the DLL and the Rootkit to new sections.

### Encoding_Decoding
This helps me to obfuscate the strings in the malware.

## TODO

* Obfuscate Imports

##   
This project is for EDUCATIONAL PURPOSES ONLY. You are the only responsable for your actions! Happy Hacking (;
