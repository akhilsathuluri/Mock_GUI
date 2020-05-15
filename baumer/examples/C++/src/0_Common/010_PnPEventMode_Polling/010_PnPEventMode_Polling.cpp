#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "bgapi2_genicam/bgapi2_genicam.hpp"
#include <string.h>
#include <sstream>

#if defined(_WIN32) // predefined Macro for ALL VC++ projects!
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#endif

using namespace BGAPI2;

int main(int argc, char* argv[])
{
    BGAPI2::SystemList *systemList = NULL;
    BGAPI2::System * pSystemGigE = NULL;
    BGAPI2::System * pSystemUSB3 = NULL;
    BGAPI2::InterfaceList * interfaceListGigE = NULL;
    BGAPI2::InterfaceList * interfaceListUSB3 = NULL;
    BGAPI2::Interface * pInterface = NULL;
    BGAPI2::DeviceList *deviceList = NULL;
    BGAPI2::Events::PnPEvent* pPnPEvent = NULL;
    bool displayDeviceLists = true;
    std::string upTime = (argc > 1) ? argv[1] : "200";
    int waitCounter = atoi(upTime.c_str());
    int returncode = 0;

    std::cout << "+------------------------------+" << std::endl;
    std::cout << "| 010_PnPEventMode_Polling.cpp |" << std::endl;
    std::cout << "+------------------------------+" << std::endl;
    std::cout << " " << std::endl;

    try
    {
        systemList = SystemList::GetInstance();
        systemList->Refresh();

        pPnPEvent = new BGAPI2::Events::PnPEvent();

        returncode = systemList->size() == 0 ? 1 : returncode;

        for (SystemList::iterator sysIterator = systemList->begin(); sysIterator != systemList->end(); sysIterator++)
        {
            std::cout << "  System Filename : " << sysIterator->second->GetFileName() << std::endl;
            std::cout << "  System Version  : " << sysIterator->second->GetVersion()  << std::endl;
            std::cout << "  System Path     : " << sysIterator->second->GetPathName() << std::endl;
            if( sysIterator->second->GetTLType() == "GEV" )
            {
                if(pSystemGigE == NULL)
                {
                    pSystemGigE = sysIterator->second;
                    pSystemGigE->Open();
                    std::cout << "  System GigE     : opened" << std::endl;
                }
            }

            if( sysIterator->second->GetTLType() == "U3V" )
            {
                if(pSystemUSB3 == NULL)
                {
                    pSystemUSB3 = sysIterator->second;
                    pSystemUSB3->Open();
                    std::cout << "  System USB3     : opened" << std::endl;
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;


        if(pSystemGigE != NULL)
        {
            //INIT PNP EVENTS GigE
            std::cout << pSystemGigE->GetDisplayName() << std::endl;
            interfaceListGigE = pSystemGigE->GetInterfaces();
            interfaceListGigE->Refresh(100);
            for (InterfaceList::iterator ifIterator = interfaceListGigE->begin(); ifIterator != interfaceListGigE->end(); ifIterator++)
            {
                pInterface = ifIterator->second;
                InterfaceList::iterator checkLastIterator = ifIterator;
                checkLastIterator++;
                pInterface->Open();
                deviceList = pInterface->GetDevices();
                deviceList->Refresh(100);

                std::cout << " |" << std::endl;
                std::cout << " +-- " << pInterface->GetDisplayName() << " opened"<< std::endl;

                if(checkLastIterator != interfaceListGigE->end())
                    std::cout << " |" ;
                else
                    std::cout << "  " ;

                pInterface->RegisterPnPEvent(BGAPI2::Events::EVENTMODE_POLLING);
                BGAPI2::Events::EventMode currentEventMode = pInterface->GetEventMode();
                BGAPI2::String sCurrentEventMode = "";
                switch(currentEventMode)
                {
                    case BGAPI2::Events::EVENTMODE_POLLING:
                        sCurrentEventMode = "EVENTMODE_POLLING";
                        break;
                    case BGAPI2::Events::EVENTMODE_UNREGISTERED:
                        sCurrentEventMode = "EVENTMODE_UNREGISTERED";
                        break;
                    case BGAPI2::Events::EVENTMODE_EVENT_HANDLER:
                        sCurrentEventMode = "EVENTMODE_EVENT_HANDLER";
                        break;
                    default:
                        sCurrentEventMode = "EVENTMODE_UNKNOWN";
                }
                std::cout << "     Register Event Mode to: " << sCurrentEventMode << std::endl;
            }
        }

        if(pSystemUSB3 != NULL)
        {
            //INIT PNP EVENTS USB3
            std::cout << std::endl << std::endl;
            std::cout << pSystemUSB3->GetDisplayName() << std::endl;
            interfaceListUSB3 = pSystemUSB3->GetInterfaces();
            interfaceListUSB3->Refresh(100);
            for (InterfaceList::iterator ifIterator = interfaceListUSB3->begin(); ifIterator != interfaceListUSB3->end(); ifIterator++)
            {
                pInterface = ifIterator->second;
                InterfaceList::iterator checkLastIterator = ifIterator;
                checkLastIterator++;
                pInterface->Open();
                deviceList = pInterface->GetDevices();
                deviceList->Refresh(100);

                std::cout << " |" << std::endl;
                std::cout << " +-- " << pInterface->GetDisplayName() << " opened"<< std::endl;
                if(checkLastIterator != interfaceListUSB3->end())
                    std::cout << " |" ;
                else
                    std::cout << "  " ;
                pInterface->RegisterPnPEvent(BGAPI2::Events::EVENTMODE_POLLING);
                BGAPI2::Events::EventMode currentEventMode = pInterface->GetEventMode();
                BGAPI2::String sCurrentEventMode = "";
                switch(currentEventMode)
                {
                    case BGAPI2::Events::EVENTMODE_POLLING:
                        sCurrentEventMode = "EVENTMODE_POLLING";
                        break;
                    case BGAPI2::Events::EVENTMODE_UNREGISTERED:
                        sCurrentEventMode = "EVENTMODE_UNREGISTERED";
                        break;
                    case BGAPI2::Events::EVENTMODE_EVENT_HANDLER:
                        sCurrentEventMode = "EVENTMODE_EVENT_HANDLER";
                        break;
                    default:
                        sCurrentEventMode = "EVENTMODE_UNKNOWN";
                }
                std::cout << "     Register Event Mode to: " << sCurrentEventMode << std::endl;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;


        while (0<waitCounter)
        {
            if(displayDeviceLists == true)
            {

                if(pSystemGigE != NULL)
                {
                    //LIST UP GigE DEVICES
                    std::cout << std::endl << std::endl;
                    std::cout << pSystemGigE->GetDisplayName() << std::endl;
                    for (InterfaceList::iterator ifIterator = interfaceListGigE->begin(); ifIterator != interfaceListGigE->end(); ifIterator++)
                    {
                        pInterface = ifIterator->second;
                        InterfaceList::iterator checkLastIterator = ifIterator;
                        checkLastIterator++;
                        deviceList = pInterface->GetDevices();
                        deviceList->Refresh(100);

                        std::cout << " |" << std::endl;
                        std::cout << " +-- " << pInterface->GetDisplayName() << " (devices: " << deviceList->size() << ")" << " IP " << pInterface->GetNode("GevInterfaceSubnetIPAddress")->GetValue() << " Mask " << pInterface->GetNode("GevInterfaceSubnetMask")->GetValue() << std::endl;

                        if(deviceList->size() > 0)
                        {
                            for (DeviceList::iterator devIterator = deviceList->begin(); devIterator != deviceList->end(); devIterator++)
                            {
                                DeviceList::iterator checkLastDeviceIterator = devIterator;
                                checkLastDeviceIterator++;
                                if (checkLastIterator != interfaceListGigE->end())
                                {
                                    std::cout << " |    |" << std::endl;
                                    std::cout << " |    +-- ";
                                }
                                else
                                {
                                    std::cout << "      |" << std::endl;
                                    std::cout << "      +-- ";
                                }

                                std::cout << std::setw(8) << devIterator->second->GetModel() << " (" << devIterator->second->GetSerialNumber() << ")" << " Access " << devIterator->second->GetAccessStatus() << " IP " << devIterator->second->GetNodeList()->GetNode("GevDeviceIPAddress")->GetValue() << " Mask " << devIterator->second->GetNodeList()->GetNode("GevDeviceSubnetMask")->GetValue() << std::endl;

                                if ( (checkLastIterator != interfaceListGigE->end()) && (checkLastDeviceIterator == deviceList->end()) )
                                {
                                    std::cout << " |" << std::endl;
                                }
                            }
                        }
                        else
                        {
                            if (checkLastIterator != interfaceListGigE->end())
                            {
                                std::cout << " |    " << std::endl;
                            }
                            else
                            {
                                std::cout << "      " << std::endl;
                            }
                        }
                    }
                }

                if(pSystemUSB3 != NULL)
                {
                    //LIST UP USB3 DEVICES
                    std::cout << std::endl << std::endl;
                    std::cout << pSystemUSB3->GetDisplayName() << std::endl;
                    for (InterfaceList::iterator ifIterator = interfaceListUSB3->begin(); ifIterator != interfaceListUSB3->end(); ifIterator++)
                    {
                        pInterface = ifIterator->second;
                        deviceList = pInterface->GetDevices();
                        deviceList->Refresh(100);
                        InterfaceList::iterator checkLastIterator = ifIterator;
                        checkLastIterator++;
                        std::cout << " |" << std::endl;
                        std::cout << " +-- " << pInterface->GetDisplayName() << " (devices: " << deviceList->size() << ")" << std::endl;

                        if(deviceList->size() > 0)
                        {
                            for (DeviceList::iterator devIterator = deviceList->begin(); devIterator != deviceList->end(); devIterator++)
                            {
                                DeviceList::iterator checkLastDeviceIterator = devIterator;
                                checkLastDeviceIterator++;
                                if(checkLastIterator != interfaceListUSB3->end())
                                {
                                    std::cout << " |    |" << std::endl;
                                    std::cout << " |    +-- ";
                                }
                                else
                                {
                                    std::cout << "      |" << std::endl;
                                    std::cout << "      +-- ";
                                }
                                std::cout << std::setw(8) << devIterator->second->GetModel() << " (" << devIterator->second->GetSerialNumber() << ")" << " Access " << devIterator->second->GetAccessStatus() << std::endl;
                                if ((checkLastIterator != interfaceListUSB3->end()) && (checkLastDeviceIterator == deviceList->end()) )
                                {
                                    std::cout << " |" << std::endl;
                                }
                            }
                        }
                        else
                        {
                            if(checkLastIterator != interfaceListUSB3->end())
                            {
                                std::cout << " |    " << std::endl;
                            }
                            else
                            {
                                std::cout << "      " << std::endl;
                            }
                        }
                    }
                    std::cout << std::endl;
                }

                std::cout << std::endl;

                displayDeviceLists = false;

            } //end of if(displayDeviceLists == true)

            if(pSystemGigE != NULL)
            {
                //WAIT FOR PnP-EVENT GigE
                for (InterfaceList::iterator ifIterator = interfaceListGigE->begin(); ifIterator != interfaceListGigE->end(); ifIterator++)
                {
                    pInterface = ifIterator->second;
                    if(pInterface->GetPnPEvent( pPnPEvent, 200) == true) //timeout 200 msec
                    {
                        std::cout << std::endl;
                        std::cout << std::endl;
                        std::cout << " " << pInterface->GetDisplayName() << " PnP Event!" << std::endl;
                        std::cout << "   EventID " << pPnPEvent->GetId() << " PnPType: " << ((pPnPEvent->GetPnPType() == 0)? "removed" : "added  ") << " SerialNumber: " << pPnPEvent->GetSerialNumber() << std::endl;
                        std::cout << std::endl;
                        displayDeviceLists = true;

#if defined(_WIN32) // predefined Macro for ALL VC++ projects!
                        Sleep(1000);
#else
                        usleep(1000000);
#endif
                        //break;
                    }
                    else
                    {
                        std::cout << "." << std::flush;
                    }
                    waitCounter--;
                }
            }
            if(pSystemUSB3 != NULL)
            {
                //Wait FOR PnP-EVENT USB3
                for (InterfaceList::iterator ifIterator = interfaceListUSB3->begin(); ifIterator != interfaceListUSB3->end(); ifIterator++)
                {
                    pInterface = ifIterator->second;
                    if(pInterface->GetPnPEvent( pPnPEvent, 200) == true) //timeout 200 msec
                    {
                        std::cout << std::endl;
                        std::cout << std::endl;
                        std::cout << " " << pInterface->GetDisplayName() << " PnP Event!" << std::endl;
                        std::cout << "   EventID " << pPnPEvent->GetId() << " PnPType: " << ((pPnPEvent->GetPnPType() == 0)? "removed" : "added  ") << " SerialNumber: " << pPnPEvent->GetSerialNumber() << std::endl;
                        std::cout << std::endl;
                        displayDeviceLists = true;

                        #if defined(_WIN32) // predefined Macro for ALL VC++ projects!
                            Sleep(1000);
                        #else
                            usleep(1000000);
                        #endif
                        //break;
                    }
                    else
                    {
                        std::cout << "." << std::flush;
                    }
                    waitCounter--;
                }
            }
            waitCounter--;

            if (0 >= waitCounter) {
                std::cout << std::endl << "Input 'c' to continue or any other value to stop." << std::endl;
                std::string sInput;
                std::cin >> sInput;
                waitCounter = (sInput == "c") ? 100 : 0;
            }
        }// end of while(0<waitCounter)
        std::cout << std::endl;

        if(pSystemGigE != NULL)
        {
            //RESET EVENT MODE TO UNREGISTERED AND INTERFACE CLOSE GigE
            std::cout << std::endl << std::endl;
            std::cout << pSystemGigE->GetDisplayName() << std::endl;
            for (InterfaceList::iterator ifIterator = interfaceListGigE->begin(); ifIterator != interfaceListGigE->end(); ifIterator++)
            {
                pInterface = ifIterator->second;
                InterfaceList::iterator checkLastIterator = ifIterator;
                checkLastIterator++;
                std::cout << " |" << std::endl;
                std::cout << " +-- " << pInterface->GetDisplayName() << " closing " << std::endl;

                if(checkLastIterator != interfaceListGigE->end())
                    std::cout << " |" ;
                else
                    std::cout << "  " ;

                pInterface->UnregisterPnPEvent();
                BGAPI2::Events::EventMode currentEventMode = pInterface->GetEventMode();
                BGAPI2::String sCurrentEventMode = "";
                switch(currentEventMode)
                {
                    case BGAPI2::Events::EVENTMODE_POLLING:
                        sCurrentEventMode = "EVENTMODE_POLLING";
                        break;
                    case BGAPI2::Events::EVENTMODE_UNREGISTERED:
                        sCurrentEventMode = "EVENTMODE_UNREGISTERED";
                        break;
                    case BGAPI2::Events::EVENTMODE_EVENT_HANDLER:
                        sCurrentEventMode = "EVENTMODE_EVENT_HANDLER";
                        break;
                    default:
                        sCurrentEventMode = "EVENTMODE_UNKNOWN";
                }
                std::cout << "     Unregister Event Mode to:   " << sCurrentEventMode << std::endl;
                pInterface->Close();
            }
        }

        if(pSystemUSB3 != NULL)
        {
            //RESET EVENT MODE TO UNREGISTERED AND INTERFACE CLOSE USB3
            std::cout << std::endl << std::endl;
            std::cout << pSystemUSB3->GetDisplayName() << std::endl;
            for (InterfaceList::iterator ifIterator = interfaceListUSB3->begin(); ifIterator != interfaceListUSB3->end(); ifIterator++)
            {
                pInterface = ifIterator->second;
                InterfaceList::iterator checkLastIterator = ifIterator;
                checkLastIterator++;
                std::cout << " |" << std::endl;
                std::cout << " +-- " << pInterface->GetDisplayName() << " closing"<< std::endl;
                if(checkLastIterator != interfaceListUSB3->end())
                    std::cout << " |" ;
                else
                    std::cout << "  " ;
                pInterface->UnregisterPnPEvent();
                BGAPI2::Events::EventMode currentEventMode = pInterface->GetEventMode();
                BGAPI2::String sCurrentEventMode = "";
                switch(currentEventMode)
                {
                    case BGAPI2::Events::EVENTMODE_POLLING:
                        sCurrentEventMode = "EVENTMODE_POLLING";
                        break;
                    case BGAPI2::Events::EVENTMODE_UNREGISTERED:
                        sCurrentEventMode = "EVENTMODE_UNREGISTERED";
                        break;
                    case BGAPI2::Events::EVENTMODE_EVENT_HANDLER:
                        sCurrentEventMode = "EVENTMODE_EVENT_HANDLER";
                        break;
                    default:
                        sCurrentEventMode = "EVENTMODE_UNKNOWN";
                }
                std::cout << "     Unregister Event Mode to:   " << sCurrentEventMode << std::endl;
            }
        }
        std::cout << std::endl;

        if(pSystemGigE != NULL)
        {
            pSystemGigE->Close();
        }
        if(pSystemUSB3 != NULL)
        {
            pSystemUSB3->Close();
        }
        BGAPI2::SystemList::ReleaseInstance();
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "Error in function: " << ex.GetFunctionName() << std::endl << "Error description: " << ex.GetErrorDescription() << std::endl << std::endl;
        try
        {
            if(pSystemGigE != NULL)
            {
                //RESET EVENT MODE TO UNREGISTERED AND INTERFACE CLOSE GigE
                for (InterfaceList::iterator ifIterator = interfaceListGigE->begin(); ifIterator != interfaceListGigE->end(); ifIterator++)
                {
                    pInterface = ifIterator->second;
                    pInterface->UnregisterPnPEvent();
                    pInterface->Close();
                }
            }

            if(pSystemUSB3 != NULL)
            {
                //RESET EVENT MODE TO UNREGISTERED AND INTERFACE CLOSE USB3
                for (InterfaceList::iterator ifIterator = interfaceListUSB3->begin(); ifIterator != interfaceListUSB3->end(); ifIterator++)
                {
                    pInterface = ifIterator->second;
                    pInterface->UnregisterPnPEvent();
                    pInterface->Close();
                }
            }

            if(pSystemGigE != NULL)
            {
                pSystemGigE->Close();
            }
            if(pSystemUSB3 != NULL)
            {
                pSystemUSB3->Close();
            }
            BGAPI2::SystemList::ReleaseInstance();
        }
        catch (BGAPI2::Exceptions::IException& ex)
        {
            returncode = (returncode == 0) ? 1 : returncode;
            std::cout << "Error in function: " << ex.GetFunctionName() << std::endl << "Error description: " << ex.GetErrorDescription() << std::endl << std::endl;
        }
    }
    std::cout << "Input any number to close the program:";
    int endKey = 0;
    std::cin >> endKey;
    return returncode;
}
