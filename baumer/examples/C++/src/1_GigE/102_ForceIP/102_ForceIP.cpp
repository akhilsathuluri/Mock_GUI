/*
    This example describes the FIRST STEPS of handling Baumer-GAPI SDK.
    The given source code applies to handling one system, one camera and twelfe images.
    Please see "Baumer-GAPI SDK Programmer's Guide" chapter 5.x

    Attention: If you want to use ForceIP on Linux reverse path filtering must be deactivated!
*/

#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "bgapi2_genicam/bgapi2_genicam.hpp"

using namespace BGAPI2;


int main()
{
    //DECLARATIONS OF VARIABLES
    BGAPI2::SystemList *systemList = NULL;
    BGAPI2::System * pSystem = NULL;
    BGAPI2::String sSystemID;

    BGAPI2::InterfaceList *interfaceList = NULL;
    BGAPI2::Interface * pInterface = NULL;
    BGAPI2::String sInterfaceID;

    BGAPI2::DeviceList *deviceList = NULL;
    BGAPI2::Device * pDevice = NULL;
    BGAPI2::String sDeviceID;

    BGAPI2::DataStreamList *datastreamList = NULL;
    BGAPI2::DataStream * pDataStream = NULL;
    BGAPI2::String sDataStreamID;

    BGAPI2::BufferList *bufferList = NULL;
    BGAPI2::Buffer * pBuffer = NULL;
    BGAPI2::String sBufferID;

    bo_int64 iDeviceIPAddress = 0;
    bo_int64 iDeviceSubnetMask = 0;
    bo_int64 iDeviceSubnet = 0;
    bo_int64 iInterfaceSubnet = 0;
    int returncode = 0;

    std::cout << std::endl;
    std::cout << "##################################################" << std::endl;
    std::cout << "# PROGRAMMER'S GUIDE Example 102_GEV_ForceIP.cpp #" << std::endl;
    std::cout << "##################################################" << std::endl;
    std::cout << std::endl << std::endl;


    std::cout << "SYSTEM LIST" << std::endl;
    std::cout << "###########" << std::endl << std::endl;

    //COUNTING AVAILABLE SYSTEMS (TL producers)
    try
    {
        systemList = SystemList::GetInstance();
        systemList->Refresh();
        std::cout << "5.1.2   Detected systems:  " << systemList->size() << std::endl;

        //SYSTEM DEVICE INFORMATION
        for (SystemList::iterator sysIterator = systemList->begin(); sysIterator != systemList->end(); sysIterator++)
        {
            std::cout << "  5.2.1   System Name:     " << sysIterator->second->GetFileName() << std::endl;
            std::cout << "          System Type:     " << sysIterator->second->GetTLType() << std::endl;
            std::cout << "          System Version:  " << sysIterator->second->GetVersion() << std::endl;
            std::cout << "          System PathName: " << sysIterator->second->GetPathName() << std::endl << std::endl;
        }
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }


    //OPEN THE FIRST SYSTEM IN THE LIST WITH A CAMERA CONNECTED
    try
    {
        for (SystemList::iterator sysIterator = systemList->begin(); sysIterator != systemList->end(); sysIterator++)
        {
            std::cout << "SYSTEM" << std::endl;
            std::cout << "######" << std::endl << std::endl;

            try
            {
                sysIterator->second->Open();
                std::cout << "5.1.3   Open next system " << std::endl;
                std::cout << "  5.2.1   System Name:     " << sysIterator->second->GetFileName() << std::endl;
                std::cout << "          System Type:     " << sysIterator->second->GetTLType() << std::endl;
                std::cout << "          System Version:  " << sysIterator->second->GetVersion() << std::endl;
                std::cout << "          System PathName: " << sysIterator->second->GetPathName() << std::endl << std::endl;
                sSystemID = sysIterator->first;
                std::cout << "        Opened system - NodeList Information " << std::endl;
                std::cout << "          GenTL Version:   " << sysIterator->second->GetNode("GenTLVersionMajor")->GetValue() << "." << sysIterator->second->GetNode("GenTLVersionMinor")->GetValue() << std::endl << std::endl;

                std::cout << "INTERFACE LIST" << std::endl;
                std::cout << "##############" << std::endl << std::endl;

                try
                {
                    interfaceList = sysIterator->second->GetInterfaces();
                    //COUNT AVAILABLE INTERFACES
                    interfaceList->Refresh(100); // timeout of 100 msec
                    std::cout << "5.1.4   Detected interfaces: " << interfaceList->size() << std::endl;

                    //INTERFACE INFORMATION
                    for (InterfaceList::iterator ifIterator = interfaceList->begin(); ifIterator != interfaceList->end(); ifIterator++)
                    {
                        std::cout << "  5.2.2   Interface ID:      " << ifIterator->first << std::endl;
                        std::cout << "          Interface Type:    " << ifIterator->second->GetTLType() << std::endl;
                        std::cout << "          Interface Name:    " << ifIterator->second->GetDisplayName() << std::endl << std::endl;
                    }
                }
                catch (BGAPI2::Exceptions::IException& ex)
                {
                    returncode = (returncode == 0) ? 1 : returncode;
                    std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
                    std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
                    std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
                }


                std::cout << "INTERFACE" << std::endl;
                std::cout << "#########" << std::endl << std::endl;

                //OPEN THE NEXT INTERFACE IN THE LIST
                try
                {
                    for (InterfaceList::iterator ifIterator = interfaceList->begin(); ifIterator != interfaceList->end(); ifIterator++)
                    {
                        if( ifIterator->second->GetTLType() == "GEV" )
                        {
                            try
                            {
                                std::cout << "5.1.5   Open interface " << std::endl;
                                std::cout << "  5.2.2   Interface ID:      " << ifIterator->first << std::endl;
                                std::cout << "          Interface Type:    " << ifIterator->second->GetTLType() << std::endl;
                                std::cout << "          Interface Name:    " << ifIterator->second->GetDisplayName() << std::endl;
                                ifIterator->second->Open();

                                //Global Discovery
                                ifIterator->second->GetNode("GlobalDiscovery")->SetBool(true);
                                std::cout << "          GlobalDiscovery:   " << ifIterator->second->GetNode("GlobalDiscovery")->GetBool() << std::endl;

                                //search for any camera is connetced to this interface
                                deviceList = ifIterator->second->GetDevices();
                                deviceList->Refresh(100);
                                if(deviceList->size() == 0)
                                {
                                    std::cout << "5.1.13   Close interface (" << deviceList->size() << " cameras found) " << std::endl << std::endl;
                                    ifIterator->second->Close();
                                }
                                else
                                {
                                    sInterfaceID = ifIterator->first;
                                    std::cout << "   " << std::endl;
                                    std::cout << "        Opened interface - NodeList Information " << std::endl;
                                    if( ifIterator->second->GetTLType() == "GEV" )
                                    {
                                        std::cout << "          GevInterfaceSubnetIPAddress: " << ifIterator->second->GetNode("GevInterfaceSubnetIPAddress")->GetValue() << std::endl;
                                        std::cout << "          GevInterfaceSubnetMask:      " << ifIterator->second->GetNode("GevInterfaceSubnetMask")->GetValue() << std::endl;
                                        std::cout << "          GevInterfaceMACAddress:      " << ifIterator->second->GetNode("GevInterfaceMACAddress")->GetValue() << std::endl;
                                    }
                                    if( ifIterator->second->GetTLType() == "U3V" )
                                    {
                                        //std::cout << "          NodeListCount:     " << ifIterator->second->GetNodeList()->GetNodeCount() << std::endl;
                                    }
                                    std::cout << "  " << std::endl;
                                    break;
                                }
                            }
                            catch (BGAPI2::Exceptions::ResourceInUseException& ex)
                            {
                                returncode = (returncode == 0) ? 1 : returncode;
                                std::cout << " Interface " << ifIterator->first << " already opened " << std::endl;
                                std::cout << " ResourceInUseException: " << ex.GetErrorDescription() << std::endl;
                            }
                        }
                    }
                }
                catch (BGAPI2::Exceptions::IException& ex)
                {
                    returncode = (returncode == 0) ? 1 : returncode;
                    std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
                    std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
                    std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
                }


                //if a camera is connected to the system interface then leave the system loop
                if(sInterfaceID != "")
                {
                        break;
                }
            }
            catch (BGAPI2::Exceptions::ResourceInUseException& ex)
            {
                returncode = (returncode == 0) ? 1 : returncode;
                std::cout << " System " << sysIterator->first << " already opened " << std::endl;
                std::cout << " ResourceInUseException: " << ex.GetErrorDescription() << std::endl;
            }
        }
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    if ( sSystemID == "" )
    {
        std::cout << " No System found " << std::endl;
        std::cout << std::endl << "End" << std::endl << "Input any number to close the program:";
        int endKey = 0;
        std::cin >> endKey;
        BGAPI2::SystemList::ReleaseInstance();
        return returncode;
    }
    else
    {
        pSystem = (*systemList)[sSystemID];
    }


    if (sInterfaceID == "")
    {
        std::cout << " No GigE camera found " << sInterfaceID << std::endl;
        std::cout << std::endl << "End" << std::endl << "Input any number to close the program:";
        int endKey = 0;
        std::cin >> endKey;
        pSystem->Close();
        BGAPI2::SystemList::ReleaseInstance();
        return returncode;
    }
    else
    {
        pInterface = (*interfaceList)[sInterfaceID];
    }


    std::cout << "DEVICE LIST" << std::endl;
    std::cout << "###########" << std::endl << std::endl;

    try
    {
        //COUNTING AVAILABLE CAMERAS
        deviceList = pInterface->GetDevices();
        deviceList->Refresh(100);
        std::cout << "5.1.6   Detected devices:         " << deviceList->size() << std::endl;

        //DEVICE INFORMATION BEFORE OPENING
        for (DeviceList::iterator devIterator = deviceList->begin(); devIterator != deviceList->end(); devIterator++)
        {
            std::cout << "  5.2.3   Device DeviceID:        " << devIterator->first << std::endl;
            std::cout << "          Device Model:           " << devIterator->second->GetModel() << std::endl;
            std::cout << "          Device SerialNumber:    " << devIterator->second->GetSerialNumber() << std::endl;
            std::cout << "          Device Vendor:          " << devIterator->second->GetVendor() << std::endl;
            std::cout << "          Device TLType:          " << devIterator->second->GetTLType() << std::endl;
            std::cout << "          Device AccessStatus:    " << devIterator->second->GetAccessStatus() << std::endl;
            std::cout << "          Device UserID:          " << devIterator->second->GetDisplayName() << std::endl << std::endl;


            //TEST DEVICE INFORMATION BEFORE DEVICE OPEN
            if(devIterator->second->GetTLType() == "GEV")
            {
                std::cout << "        Device NodeList Information before opening " << std::endl;
                NodeMap * pDeviceNodeMap = devIterator->second->GetNodeList();

                std::cout << "          Device MAC address (hex):      " << std::hex << std::setfill('0') << std::setw(2)
                                                                       << ((pDeviceNodeMap->GetNode("GevDeviceMACAddress")->GetInt() & 0xffffffffffffLL)>>40) << ":" << std::setw(2)
                                                                       << ((pDeviceNodeMap->GetNode("GevDeviceMACAddress")->GetInt() & 0xffffffffffLL)>>32) << ":" << std::setw(2)
                                                                       << ((pDeviceNodeMap->GetNode("GevDeviceMACAddress")->GetInt() & 0xffffffff)>>24) << ":" << std::setw(2)
                                                                       << ((pDeviceNodeMap->GetNode("GevDeviceMACAddress")->GetInt() & 0xffffff)>>16) << ":" << std::setw(2)
                                                                       << ((pDeviceNodeMap->GetNode("GevDeviceMACAddress")->GetInt() & 0xffff)>>8) << ":" << std::setw(2)
                                                                       << (pDeviceNodeMap->GetNode("GevDeviceMACAddress")->GetInt() & 0xff) << std::setfill(' ') << std::dec << std::endl;
                //DEVICE IP ADDRESS
                iDeviceIPAddress = pDeviceNodeMap->GetNode("GevDeviceIPAddress")->GetInt();
                std::cout << "          Device IP address (hex):       0x" << std::nouppercase << std::setfill('0') << std::hex << std::setw(8) << iDeviceIPAddress  << std::nouppercase << std::setfill(' ') << std::dec << std::endl;
                std::cout << "          Device IP address (string):    " << pDeviceNodeMap->GetNode("GevDeviceIPAddress")->GetValue() << std::endl;
                //DEVICE SUBNET MASK
                iDeviceSubnetMask = pDeviceNodeMap->GetNode("GevDeviceSubnetMask")->GetInt();
                std::cout << "          Device Subnet mask (hex):      0x" << std::nouppercase << std::setfill('0') << std::hex << std::setw(8) << iDeviceSubnetMask << std::nouppercase << std::setfill(' ') << std::dec << std::endl;
                std::cout << "          Device Subnet mask (string):   " << pDeviceNodeMap->GetNode("GevDeviceSubnetMask")->GetValue() << std::endl;

                //CHECK THE SUBNETS ARE MATCHING
                std::cout << "        Check subnet of device is matching to subnet of interface " << std::endl;
                iDeviceSubnet = iDeviceIPAddress & iDeviceSubnetMask; //bit wise AND
                std::cout << "          Device Subnet (hex):           0x" << std::nouppercase << std::setfill('0') << std::hex << std::setw(8) << iDeviceSubnet << std::nouppercase << std::setfill(' ') << std::dec << std::endl;
                std::cout << "          Device Subnet (string):        " << ((iDeviceSubnet & 0xffffffff)>>24) << "."
                                                                       << ((iDeviceSubnet & 0xffffff)>>16) << "."
                                                                       << ((iDeviceSubnet & 0xffff)>>8) << "."
                                                                       << (iDeviceSubnet & 0xff) << std::endl;

                iInterfaceSubnet = ((unsigned int)pInterface->GetNode("GevInterfaceSubnetIPAddress")->GetInt()) & ((unsigned int)pInterface->GetNode("GevInterfaceSubnetMask")->GetInt()); //bit wise AND
                std::cout << "          Interface Subnet (hex):        0x" << std::nouppercase << std::setfill('0') << std::hex << std::setw(8) << iInterfaceSubnet << std::nouppercase << std::setfill(' ') << std::dec << std::endl;
                std::cout << "          Interface Subnet (string):     " << ((iInterfaceSubnet & 0xffffffff)>>24) << "."
                                                                       << ((iInterfaceSubnet & 0xffffff)>>16) << "."
                                                                       << ((iInterfaceSubnet & 0xffff)>>8) << "."
                                                                       << (iInterfaceSubnet & 0xff) << std::endl;
                if(iDeviceSubnet == iInterfaceSubnet)
                {
                    std::cout << "          IP address of device matches to IP address of interface " << std::endl;
                }
                else
                {
                    std::cout << "          IP address of device does not match to IP address of interface " << std::endl << std::endl;
                    std::cout << "        Try ForceIP on camera to get temporary match " << std::endl;

                    try
                    {
                        //PREPARATION OF FORCE IP - SELECT THE MAC ADDDRESS
                        bo_int64 iDeviceMacAddress = pDeviceNodeMap->GetNode("GevDeviceMACAddress")->GetInt();
                        pDeviceNodeMap->GetNode("MACAddressNeededToForce")->SetInt(iDeviceMacAddress);

                        //PREPARATION OF FORCE IP - CREATE FORCE IP ADDDRESS
                        bo_int64 iForceIPAddress = iInterfaceSubnet + 1; // e.g 169.254.0.1
                        std::cout << "          Forced IP Address (hex):       0x" << std::nouppercase << std::setfill('0') << std::hex << std::setw(8) << iForceIPAddress << std::nouppercase << std::setfill(' ') << std::dec << std::endl;
                        std::cout << "          Forced IP Address (string):    "   << ((iForceIPAddress & 0xffffffff)>>24) << "."
                                                                                   << ((iForceIPAddress & 0xffffff)>>16) << "."
                                                                                   << ((iForceIPAddress & 0xffff)>>8) << "."
                                                                                   << (iForceIPAddress & 0xff) << std::endl;
                        std::cout << "          Interface IP Address (hex):    0x" <<std::nouppercase << std::setfill('0') << std::hex << std::setw(8) << pInterface->GetNode("GevInterfaceSubnetIPAddress")->GetInt() << std::nouppercase << std::setfill(' ') << std::dec << std::endl;
                        std::cout << "          Interface IP Address (string):    " << pInterface->GetNode("GevInterfaceSubnetIPAddress")->GetValue() << std::endl;

                        if ( iForceIPAddress == pInterface->GetNode("GevInterfaceSubnetIPAddress")->GetInt() )
                        {
                            //IF NEW IP ADDRESS IS ALREADY USED BY THE INTERFACE THEN USE NEXT
                            iForceIPAddress = iForceIPAddress + 1; // e.g. 169.254.0.2
                            std::cout << "          Warning. Created IP address is already used by the interface " << std::endl;
                            std::cout << "          Try next Forced IP Address " << std::endl;
                            std::cout << "          Forced IP Address (hex):       0x" << std::nouppercase << std::setfill('0') << std::hex << std::setw(8) << iForceIPAddress << std::nouppercase << std::setfill(' ') << std::dec << std::endl;
                            std::cout << "          Forced IP Address (string):    "   << ((iForceIPAddress & 0xffffffff)>>24) << "."
                                                                                       << ((iForceIPAddress & 0xffffff)>>16) << "."
                                                                                       << ((iForceIPAddress & 0xffff)>>8) << "."
                                                                                       << (iForceIPAddress & 0xff) << std::endl;
                            // more checks necessary, like another camera might use that IP address...
                        }
                        pDeviceNodeMap->GetNode("ForcedIPAddress")->SetInt(iForceIPAddress);

                        //PREPARATION OF FORCE IP - SET FORCE IP SUBNET MASK MATCHING TO INTERFACE SUBNET MASK
                        bo_int64 iInterfaceSubnetMask = pInterface->GetNode("GevInterfaceSubnetMask")->GetInt();
                        std::cout << "          Forced Subnet Mask (hex):      0x" << std::nouppercase << std::setfill('0') << std::hex << std::setw(8) << iInterfaceSubnetMask << std::nouppercase << std::setfill(' ') << std::dec << std::endl;
                        std::cout << "          Forced Subnet Mask (string):     " << pInterface->GetNode("GevInterfaceSubnetMask")->GetValue() << std::endl;
                        pDeviceNodeMap->GetNode("ForcedSubnetMask")->SetInt(iInterfaceSubnetMask);

                        //PREPARATION OF FORCE IP - SET FORCE IP GATEWAY
                        bo_int64 iForceIPGateway = 0;
                        std::cout << "          Forced Gateway (hex):          0x" << std::nouppercase << std::setfill('0') << std::hex << std::setw(8) << iForceIPGateway << std::nouppercase << std::setfill(' ') << std::dec << std::endl;
                        std::cout << "          Forced Gateway (string):       "   << ((iForceIPGateway & 0xffffffff)>>24) << "."
                                                                                   << ((iForceIPGateway & 0xffffff)>>16) << "."
                                                                                   << ((iForceIPGateway & 0xffff)>>8) << "."
                                                                                   << (iForceIPGateway & 0xff) << std::endl;
                        pDeviceNodeMap->GetNode("ForcedGateway")->SetInt(iForceIPGateway);

                        std::cout << "          Start of Force IP ... " << std::endl;
                        pDeviceNodeMap->GetNode("ForceIP")->Execute();
                        std::cout << "          End of Force IP" << std::endl;

                        deviceList->Refresh(100);
                        std::cout << "5.1.6   Detected devices after ForceIP: " << deviceList->size() << std::endl;
                    }
                    catch (BGAPI2::Exceptions::IException& ex)
                    {
                        returncode = (returncode == 0) ? 1 : returncode;
                        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
                        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
                        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
                    }
                }
            }
            std::cout << std::endl;
        }
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }


    std::cout << "DEVICE" << std::endl;
    std::cout << "######" << std::endl << std::endl;

    //OPEN THE FIRST CAMERA IN THE LIST
    try
    {
        for (DeviceList::iterator devIterator = deviceList->begin(); devIterator != deviceList->end(); devIterator++)
        {
            try
            {
                std::cout << "5.1.7   Open first device " << std::endl;
                std::cout << "          Device DeviceID:        " << devIterator->first << std::endl;
                std::cout << "          Device Model:           " << devIterator->second->GetModel() << std::endl;
                std::cout << "          Device SerialNumber:    " << devIterator->second->GetSerialNumber() << std::endl;
                std::cout << "          Device Vendor:          " << devIterator->second->GetVendor() << std::endl;
                std::cout << "          Device TLType:          " << devIterator->second->GetTLType() << std::endl;
                std::cout << "          Device AccessStatus:    " << devIterator->second->GetAccessStatus() << std::endl;
                std::cout << "          Device UserID:          " << devIterator->second->GetDisplayName() << std::endl << std::endl;
                devIterator->second->Open();
                sDeviceID = devIterator->first;
                std::cout << "        Opened device - RemoteNodeList Information " << std::endl;
                std::cout << "          Device AccessStatus:    " << devIterator->second->GetAccessStatus() << std::endl;

                //SERIAL NUMBER
                if(devIterator->second->GetRemoteNodeList()->GetNodePresent("DeviceSerialNumber"))
                    std::cout << "          DeviceSerialNumber:     " << devIterator->second->GetRemoteNode("DeviceSerialNumber")->GetValue() << std::endl;
                else if(devIterator->second->GetRemoteNodeList()->GetNodePresent("DeviceID"))
                    std::cout << "          DeviceID (SN):          " << devIterator->second->GetRemoteNode("DeviceID")->GetValue() << std::endl;
                else
                    std::cout << "          SerialNumber:           Not Available "  << std::endl;

                //DISPLAY DEVICEMANUFACTURERINFO
                if(devIterator->second->GetRemoteNodeList()->GetNodePresent("DeviceManufacturerInfo"))
                    std::cout << "          DeviceManufacturerInfo: " << devIterator->second->GetRemoteNode("DeviceManufacturerInfo")->GetValue() << std::endl;

                //DISPLAY DEVICEFIRMWAREVERSION OR DEVICEVERSION
                if(devIterator->second->GetRemoteNodeList()->GetNodePresent("DeviceFirmwareVersion"))
                    std::cout << "          DeviceFirmwareVersion:  " << devIterator->second->GetRemoteNode("DeviceFirmwareVersion")->GetValue() << std::endl;
                else if(devIterator->second->GetRemoteNodeList()->GetNodePresent("DeviceVersion"))
                    std::cout << "          DeviceVersion:          " << devIterator->second->GetRemoteNode("DeviceVersion")->GetValue() << std::endl;
                else
                    std::cout << "          DeviceVersion:          Not Available "  << std::endl;

                if(devIterator->second->GetTLType() == "GEV")
                {
                    std::cout << "          GevCCP:                 " << devIterator->second->GetRemoteNode("GevCCP")->GetValue() << std::endl;
                    std::cout << "          GevCurrentIPAddress:    " << devIterator->second->GetRemoteNode("GevCurrentIPAddress")->GetValue() << std::endl;
                    std::cout << "          GevCurrentSubnetMask:   " << devIterator->second->GetRemoteNode("GevCurrentSubnetMask")->GetValue() << std::endl;
                }
                std::cout << "  " << std::endl;
                break;
            }
            catch (BGAPI2::Exceptions::ResourceInUseException& ex)
            {
                returncode = (returncode == 0) ? 1 : returncode;
                std::cout << " Device  " << devIterator->first << " already opened " << std::endl;
                std::cout << " ResourceInUseException: " << ex.GetErrorDescription() << std::endl;
            }
            catch (BGAPI2::Exceptions::AccessDeniedException& ex)
            {
                returncode = (returncode == 0) ? 1 : returncode;
                std::cout << " Device  " << devIterator->first << " already opened " << std::endl;
                std::cout << " AccessDeniedException " << ex.GetErrorDescription() << std::endl;
            }
        }
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    if (sDeviceID == "")
    {
        std::cout << " No Device found " << sDeviceID << std::endl;
        std::cout << std::endl << "End" << std::endl << "Input any number to close the program:";
        int endKey = 0;
        std::cin >> endKey;
        pInterface->Close();
        pSystem->Close();
        BGAPI2::SystemList::ReleaseInstance();
        return returncode;
    }
    else
    {
        pDevice = (*deviceList)[sDeviceID];
    }


    std::cout << "DEVICE PARAMETER SETUP" << std::endl;
    std::cout << "######################" << std::endl << std::endl;

    try
    {
        //SET TRIGGER MODE OFF (FreeRun)
        pDevice->GetRemoteNode("TriggerMode")->SetString("Off");
        std::cout << "         TriggerMode:             " << pDevice->GetRemoteNode("TriggerMode")->GetValue() << std::endl;
        std::cout << std::endl;
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }


    std::cout << "DATA STREAM LIST" << std::endl;
    std::cout << "################" << std::endl << std::endl;

    try
    {
        //COUNTING AVAILABLE DATASTREAMS
        datastreamList = pDevice->GetDataStreams();
        datastreamList->Refresh();
        std::cout << "5.1.8   Detected datastreams:     " << datastreamList->size() << std::endl;

        //DATASTREAM INFORMATION BEFORE OPENING
        for (DataStreamList::iterator dstIterator = datastreamList->begin(); dstIterator != datastreamList->end(); dstIterator++)
        {
            std::cout << "  5.2.4.  DataStream ID:          " << dstIterator->first << std::endl << std::endl;
        }
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }


    std::cout << "DATA STREAM" << std::endl;
    std::cout << "###########" << std::endl << std::endl;

    //OPEN THE FIRST DATASTREAM IN THE LIST
    try
    {
        for (DataStreamList::iterator dstIterator = datastreamList->begin(); dstIterator != datastreamList->end(); dstIterator++)
        {
            std::cout << "5.1.9   Open first datastream " << std::endl;
            std::cout << "          DataStream ID:          " << dstIterator->first << std::endl << std::endl;
            dstIterator->second->Open();
            sDataStreamID = dstIterator->first;
            std::cout << "        Opened datastream - NodeList Information " << std::endl;
            std::cout << "          StreamAnnounceBufferMinimum:  " << dstIterator->second->GetNode("StreamAnnounceBufferMinimum")->GetValue() << std::endl;
            if( dstIterator->second->GetTLType() == "GEV" )
            {
                std::cout << "          StreamDriverModel:            " << dstIterator->second->GetNode("StreamDriverModel")->GetValue() << std::endl;
            }
            std::cout << "  " << std::endl;
            break;
        }
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    if (sDataStreamID == "")
    {
        std::cout << " No DataStream found " << sDataStreamID << std::endl;
        std::cout << std::endl << "End" << std::endl << "Input any number to close the program:";
        int endKey = 0;
        std::cin >> endKey;
        pDevice->Close();
        pInterface->Close();
        pSystem->Close();
        BGAPI2::SystemList::ReleaseInstance();
        return returncode;
    }
    else
    {
        pDataStream = (*datastreamList)[sDataStreamID];
    }


    std::cout << "BUFFER LIST" << std::endl;
    std::cout << "###########" << std::endl << std::endl;

    try
    {
        //BufferList
        bufferList = pDataStream->GetBufferList();

        // 4 buffers using internal buffer mode
        for(int i=0; i<4; i++)
        {
            pBuffer = new BGAPI2::Buffer();
            bufferList->Add(pBuffer);
        }
        std::cout << "5.1.10   Announced buffers:       " << bufferList->GetAnnouncedCount() << " using " << pBuffer->GetMemSize() * bufferList->GetAnnouncedCount() << " [bytes]" << std::endl;
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    try
    {
        for (BufferList::iterator bufIterator = bufferList->begin(); bufIterator != bufferList->end(); bufIterator++)
        {
            bufIterator->second->QueueBuffer();
        }
        std::cout << "5.1.11   Queued buffers:          " << bufferList->GetQueuedCount() << std::endl;
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }
    std::cout << " " << std::endl;


    std::cout << "CAMERA START" << std::endl;
    std::cout << "############" << std::endl << std::endl;

    //START DataStream acquisition
    try
    {
        pDataStream->StartAcquisitionContinuous();
        std::cout << "5.1.12   DataStream started " << std::endl;
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    //START CAMERA
    try
    {
        std::cout << "5.1.12   " << pDevice->GetModel() << " started " << std::endl;
        pDevice->GetRemoteNode("AcquisitionStart")->Execute();
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    //CAPTURE 12 IMAGES
    std::cout << " " << std::endl;
    std::cout << "CAPTURE 12 IMAGES BY IMAGE POLLING" << std::endl;
    std::cout << "##################################" << std::endl << std::endl;

    BGAPI2::Buffer * pBufferFilled = NULL;
    try
    {
        for(int i = 0; i < 12; i++)
        {
            pBufferFilled = pDataStream->GetFilledBuffer(1000); //timeout 1000 msec
            if(pBufferFilled == NULL)
            {
                std::cout << "Error: Buffer Timeout after 1000 msec" << std::endl;
            }
            else if(pBufferFilled->GetIsIncomplete() == true)
            {
                std::cout << "Error: Image is incomplete" << std::endl;
                // queue buffer again
                pBufferFilled->QueueBuffer();
            }
            else
            {
                std::cout << " Image " << std::setw(5) << pBufferFilled->GetFrameID() << " received in memory address " << std::hex << pBufferFilled->GetMemPtr() << std::dec << std::endl;
                // queue buffer again
                pBufferFilled->QueueBuffer();
            }
        }
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }
    std::cout << " " << std::endl;

    std::cout << "CAMERA STOP" << std::endl;
    std::cout << "###########" << std::endl << std::endl;

    //STOP CAMERA
    try
    {
        //SEARCH FOR 'AcquisitionAbort'
        if(pDevice->GetRemoteNodeList()->GetNodePresent("AcquisitionAbort"))
        {
            pDevice->GetRemoteNode("AcquisitionAbort")->Execute();
            std::cout << "5.1.12   " << pDevice->GetModel() << " aborted " << std::endl;
        }

        pDevice->GetRemoteNode("AcquisitionStop")->Execute();
        std::cout << "5.1.12   " << pDevice->GetModel() << " stopped " << std::endl;
        std::cout << std::endl;

        BGAPI2::String sExposureNodeName = "";
        if (pDevice->GetRemoteNodeList()->GetNodePresent("ExposureTime")) {
            sExposureNodeName = "ExposureTime";
        } else if (pDevice->GetRemoteNodeList()->GetNodePresent("ExposureTimeAbs")) {
            sExposureNodeName = "ExposureTimeAbs";
        }
        std::cout << "         ExposureTime:                   " << std::fixed << std::setprecision(0) << pDevice->GetRemoteNode(sExposureNodeName)->GetDouble() << " [" << pDevice->GetRemoteNode(sExposureNodeName)->GetUnit() << "]" << std::endl;
        if( pDevice->GetTLType() == "GEV" )
        {
            if(pDevice->GetRemoteNodeList()->GetNodePresent("DeviceStreamChannelPacketSize"))
                std::cout << "         DeviceStreamChannelPacketSize:  " << pDevice->GetRemoteNode("DeviceStreamChannelPacketSize")->GetInt() << " [bytes]" << std::endl;
            else
                std::cout << "         GevSCPSPacketSize:              " << pDevice->GetRemoteNode("GevSCPSPacketSize")->GetInt() << " [bytes]" << std::endl;
            std::cout << "         GevSCPD (PacketDelay):          " << pDevice->GetRemoteNode("GevSCPD")->GetInt() << " [tics]" << std::endl;
        }
        std::cout << std::endl;
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    //STOP DataStream acquisition
    try
    {
        if( pDataStream->GetTLType() == "GEV" )
        {
            //DataStream Statistic
            std::cout << "         DataStream Statistics " << std::endl;
            std::cout << "           DataBlockComplete:              " << pDataStream->GetNodeList()->GetNode("DataBlockComplete")->GetInt() << std::endl;
            std::cout << "           DataBlockInComplete:            " << pDataStream->GetNodeList()->GetNode("DataBlockInComplete")->GetInt() << std::endl;
            std::cout << "           DataBlockMissing:               " << pDataStream->GetNodeList()->GetNode("DataBlockMissing")->GetInt() << std::endl;
            std::cout << "           PacketResendRequestSingle:      " << pDataStream->GetNodeList()->GetNode("PacketResendRequestSingle")->GetInt() << std::endl;
            std::cout << "           PacketResendRequestRange:       " << pDataStream->GetNodeList()->GetNode("PacketResendRequestRange")->GetInt() << std::endl;
            std::cout << "           PacketResendReceive:            " << pDataStream->GetNodeList()->GetNode("PacketResendReceive")->GetInt() << std::endl;
            std::cout << "           DataBlockDroppedBufferUnderrun: " << pDataStream->GetNodeList()->GetNode("DataBlockDroppedBufferUnderrun")->GetInt() << std::endl;
            std::cout << "           Bitrate:                        " << pDataStream->GetNodeList()->GetNode("Bitrate")->GetDouble() << std::endl;
            std::cout << "           Throughput:                     " << pDataStream->GetNodeList()->GetNode("Throughput")->GetDouble() << std::endl;
            std::cout << std::endl;
        }
        if( pDataStream->GetTLType() == "U3V" )
        {
            //DataStream Statistic
            std::cout << "         DataStream Statistics " << std::endl;
            std::cout << "           GoodFrames:            " << pDataStream->GetNodeList()->GetNode("GoodFrames")->GetInt() << std::endl;
            std::cout << "           CorruptedFrames:       " << pDataStream->GetNodeList()->GetNode("CorruptedFrames")->GetInt() << std::endl;
            std::cout << "           LostFrames:            " << pDataStream->GetNodeList()->GetNode("LostFrames")->GetInt() << std::endl;
            std::cout << std::endl;
        }

        pDataStream->StopAcquisition();
        std::cout << "5.1.12   DataStream stopped " << std::endl;
        bufferList->DiscardAllBuffers();
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }
    std::cout << std::endl;


    std::cout << "RELEASE" << std::endl;
    std::cout << "#######" << std::endl << std::endl;

    //Release buffers
    std::cout << "5.1.13   Releasing the resources " << std::endl;
    try
    {
        while( bufferList->size() > 0)
        {
            pBuffer = bufferList->begin()->second;
            bufferList->RevokeBuffer(pBuffer);
            delete pBuffer;
        }
        std::cout << "         buffers after revoke:    " << bufferList->size() << std::endl;

        pDataStream->Close();
        pDevice->Close();
        pInterface->Close();
        pSystem->Close();
        BGAPI2::SystemList::ReleaseInstance();
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = 0==returncode?1:returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    std::cout << std::endl;
    std::cout << "End" << std::endl << std::endl;

    std::cout << "Input any number to close the program:";
    int endKey = 0;
    std::cin >> endKey;
    return returncode;
}
