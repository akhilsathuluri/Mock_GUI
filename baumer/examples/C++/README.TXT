********************************************************************************
*
* BGAPI SDK EXAMPLES C++
* 
* (c) BAUMER OPTRONIC, RADEBERG, GERMANY
*
********************************************************************************
*
*
* SUMMARY:
*           1     PRECONDITIONS
*           
*           2     CONTENTS
*           
*           3     HOW TO INSTALL EXAMPLES
*           
*           4     HOW TO RUN EXAMPLES
*           
*           5     LIMITATIONS
*           
*
********************************************************************************
*
*
*           1     PRECONDITIONS
*           
*
*                 THE INSTALLATION OF THE SDK EXAMPLES C++ REQUIRES CMAKE. IF 
*                 YOU DO NOT HAVE CMAKE INSTALLED PLEASE DOWNLOAD FROM
*                 
*                 http://www.cmake.org/
*                 
*                 OR USE THE VERSION INCLUDED ON THIS BGAPI-CD. YOU DO NOT HAVE
*                 TO CONFIGURE ANYTHING AFTER INSTALLATION.
*                 CMAKE IS NECESSARY TO CREATE PROJECT AND SOLUTION FILES FROM
*                 SOURCE FILES.
*                 
*                 PLEASE NOTE:
*                 IF YOU INSTALLED THE CONTENT OF THE BGAPI-CD INTO PROGRAM- 
*                 FILES FOLDER PLEASE BE AWARE THAT YOU WILL NEED AN ADMINISTRA-
*                 TOR USER ACCOUNT TO INSTALL AND EXECUTE THESE EXAMPLES.
*                 
*                 IF YOU DO NOT WISH TO USE CMAKE FOR ANY REASON PLEASE CREATE
*                 PROJECTS AND SOLUTIONS ON YOUR OWN USING THE SOURCES FROM THE
*                 FOLDER
*                           ..\C++\src\*
*                           
*                 AND BINARIES FROM THE FOLDER
*                 
*                     ON WINDOWS:
*                     
*                           ..\BIN\$(YOUR_PLATFORM)\*.*
*                           $YOUR_PLATFORM = Win32 (32 Bit) // x64 (64Bit)
*
*                     ON LINUX:                           
*
*                           /usr/local/src/baumer/inc
*                            
*
*           2     CONTENTS
*           
*                 .\listings
*                      
*                     .\GigE    
*                               OUTPUT BY RUNNING EXAMPLES WITH A GIGE CAMERA
*                               { ONE TEXT FILE FOR EACH EXAMPLE }
*                     
*                     .\USB     
*                               OUTPUT BY RUNNING EXAMPLES WITH A USB CAMERA
*                               { ONE TEXT FILE FOR EACH EXAMPLE }
*
*                 .\src
*                 
*                     CONTAINS EXAMPLE CODE AND CMAKE PROJECT DESCRIPTION
*                                     
*                                     
*
*                 .\install_example_win.bat
*                               BATCH-FILE TO INSTALL EXAMPLES ON WINDOWS
*                 
*                 .\install_example_linux.sh
*                               SHELL-FILE TO INSTALL EXAMPLES ON LINUX
*                 
*                 .\CMakeLists.txt
*                               TEXTFILE TO CONFIGURE INSTALLATION BY CMAKE
*                 
*                 .\README.txt
*                               *THIS* FILE
*                      
*         
*                 PLEASE NOTE: AFTER INSTALLATION THIS CONTAINS SOME 
*                 FILES DEPENDANT ON YOUR CONFIGURATION AND PLATFORM.
*                               
*
*           3     HOW TO INSTALL EXAMPLES
*           
*
*                 IF YOU INSTALLED THE CONTENT OF THE BGAPI-CD TO ANY FOLDER
*                 <<<BUT>>> "PROGRAM FILES" JUST RUN 
*                 
*                 install_example_win.bat
*                 
*                 BY DOUBLE-CLICK OR HITTING ENTER AND FOLLOW THE INSTRUCTIONS
*                 BELOW.
*                 
*                 IF YOU INSTALLED THE CONTENT OF THE BGAPI-CD TO THE FOLDER
*                 "PROGRAM FILES" OR ANY OTHER FOLDER WHICH NEEDS AN ADMINISTRA-
*                 TOR USER ACCOUNT PLEASE START A NEW COMMAND PROMPT (e.g. VISUAL
*                 STUDIO COMMAND PROMPT) AND CHANGE THE PATH TO THE DIRECTORY 
*                 WHERE THE BATCH FILE IS LOCATED ("cd .."). THEN CALL
*                 
*                 install_example_win.bat
*                 
*                 THE BATCH-FILE WILL GUIDE YOU THROUGH THE INSTALLATION PROCESS.
*                 ON WRONG USER-INPUT BY DEFAULT VISUAL STUDIO 2010 Win32 IS SET.
*                 
*                 AS THE INSTALLATION IS FINISHED YOU SHOULD FIND A NEW FOLDER
*                 CONTAINING A SOLUTION AND OTHER FILES ACCORDING TO YOUR CHOICE
*                 WHICH IDE AND PLATFORM TO USE.
*                 
*                 IF NOT, POSSIBLY SOMETHING WENT WRONG. CONSIDER THE FOLLOWING
*                 ISSUES:
*                 
*                   - YOU DO NOT HAVE CMAKE INSTALLED.
*                     => INSTALL CMAKE AND TRY AGAIN.
*                     
*                   - YOU HAVE CMAKE INSTALLED, BUT IT WAS NOT IDENTIFIED.
*                     => OPEN A COMMAND WINDOW AND CHECK IF YOU FIND CMAKE IN PATH
*                        IF CMAKE IS INCLUDED IN PATH, RUN THE BATCH-FILE FROM 
*                        THIS COMMAND WINDOW.
*                        IF CMAKE IS NOT INCLUDED IN PATH REBOOT AND RUN THE
*                        BATCH-FILE AGAIN.
*                   - OR -
*                     => EDIT YOUR LOCAL "PATH" AND MAKE SURE IT CONTAINS
*                        THE PATH TO YOUR CMAKE BINARIES. IF YOU CHANGE THE SYSTEM
*                        ENVIROMENT YOU NEED TO REBOOT AFTERWARDS.
*                   - OR -
*                     => OPEN THE BATCH FILE FOR EDITING. UNCOMMENT LINE 13
*                        ("SET N_CMAKE=0") AND TRY AGAIN. 
*                        
*                   - INSTALLATION FAILED DUE TO MISSING USER RIGHTS.
*                     => CHANGE YOUR USER ACCOUNT TO ADMIN AND TRY AGAIN.
*                     
*                   - UNKNOWN COMPILER / UNKNOWN IDE / UNKNOWN PLATFORM
*                     => PLEASE READ § 3 LIMITATIONS    
*             
*
*           4     HOW TO RUN EXAMPLES
*           
*                         
*                 IF INSTALLED SUCCESSFULLY OPEN THE NEW CREATED FOLDER E.G.
*                 
*                 "build_vs10_c++_WIN_32"
*                 
*                 OPEN THE CREATED SOLUTION
*                 
*                 "Baumer_GAPI_SDK_Examples_C++.sln"
*                 
*                 ONCE OPENED JUST CHOOSE A STARTUP-PROJECT AND HIT F5.
*                 
*       
*           5     LIMITATIONS 
*           
*
*                 THE INSTALLATION AND EXECUTION OF THESE EXAMPLES WAS TESTED
*                 TO WORK WITH ALL IDEs AND PLATFORMs NAMED IN THE USER MENU OF 
*                 THE install_example_win.bat. HOWEVER IF YOU ARE A EXPERIENCED 
*                 USER OF CMAKE YOU ARE FREE TO CREATE ANY CONFIGURATION YOU WANT.
*                 IN THIS CASE DO NOT USE THE BATCH-FILE BUT CALL CMAKE WITHIN
*                 THE DIRECTORY WHERE THE BATCH FILE IS LOCATED.
*                 
*                 FOR FURTHER INSTRUCTIONS TYPE "CMAKE --HELP" INSIDE A COMMAND
*                 PROMPT.   
*                  

