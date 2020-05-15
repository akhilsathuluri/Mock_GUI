/*
    This example describes the FIRST STEPS of handling Baumer-GAPI SDK.
    The given source code applies to handling one system, one camera and twenty images.
    Please see "Baumer-GAPI SDK Programmer's Guide" chapter 5.4
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

    //external buffer
    char * pMemoryBlock[4] = {0};
    int returncode = 0;

    std::cout << std::endl;
    std::cout << "#################################################################" << std::endl;
    std::cout << "# PROGRAMMER'S GUIDE Example 004_PartialScan_ExternalBuffer.cpp #" << std::endl;
    std::cout << "#################################################################" << std::endl;
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
                        try
                        {
                            std::cout << "5.1.5   Open interface " << std::endl;
                            std::cout << "  5.2.2   Interface ID:      " << ifIterator->first << std::endl;
                            std::cout << "          Interface Type:    " << ifIterator->second->GetTLType() << std::endl;
                            std::cout << "          Interface Name:    " << ifIterator->second->GetDisplayName() << std::endl;
                            ifIterator->second->Open();
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
        std::cout << " No camera found " << sInterfaceID << std::endl;
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
        

        //SET A PARTIAL SCAN (ROI...REGION OF INTEREST) IN THE RIGHT BOTTOM AREA OF THE IMAGE
        //====================================================================================

        std::cout << "    Set ROI parameters to the right bottom quarter of the sensor" << std::endl << std::endl;

        //IMAGE WIDTH
        //===========
        std::cout << "        Width" << std::endl;
        std::cout << "          description:            " << pDevice->GetRemoteNode("Width")->GetDescription() << std::endl;
        std::cout << "          interface type:         " << pDevice->GetRemoteNode("Width")->GetInterface() << std::endl;
        bo_int64 iImageWidth = 0;
        bo_int64 iImageWidthMin = 0;
        bo_int64 iImageWidthMax = 0;
        bo_int64 iImageWidthInc = 0;

        //get current value and limits
        iImageWidth = pDevice->GetRemoteNode("Width")->GetInt();
        iImageWidthMin = pDevice->GetRemoteNode("Width")->GetIntMin();
        iImageWidthMax = pDevice->GetRemoteNode("Width")->GetIntMax();
        iImageWidthInc = pDevice->GetRemoteNode("Width")->GetIntInc();

        std::cout << "          current value:          " << iImageWidth << std::endl;
        std::cout << "          possible value range:   " << iImageWidthMin << " to "  << iImageWidthMax << " with increment of " << iImageWidthInc << std::endl;

        //set new width value same to the half value of sensor width
        bo_int64 widthvalue = pDevice->GetRemoteNode("SensorWidth")->GetInt() / 2 / iImageWidthInc * iImageWidthInc; // find number to match the increment

        // check new value is within range
        if( widthvalue < iImageWidthMin)
            widthvalue = iImageWidthMin;

        if( widthvalue > iImageWidthMax)
            widthvalue = iImageWidthMax;
        
        pDevice->GetRemoteNode("Width")->SetInt(widthvalue);

        //recheck new width is set
        std::cout << "          set value to:           " << pDevice->GetRemoteNode("Width")->GetInt() << " is half value of the sensor width: " << pDevice->GetRemoteNode("SensorWidth")->GetInt() << std::endl << std::endl;
        

        //IMAGE OFFSET X
        //==============
        std::cout << "        OffsetX" << std::endl;
        std::cout << "          description:            " << pDevice->GetRemoteNode("OffsetX")->GetDescription() << std::endl;
        std::cout << "          interface type:         " << pDevice->GetRemoteNode("OffsetX")->GetInterface() << std::endl;
        bo_int64 iImageOffsetX = 0;
        bo_int64 iImageOffsetXMin = 0;
        bo_int64 iImageOffsetXMax = 0;
        bo_int64 iImageOffsetXInc = 0;

        //get current value and limits
        iImageOffsetX = pDevice->GetRemoteNode("OffsetX")->GetInt();
        iImageOffsetXMin = pDevice->GetRemoteNode("OffsetX")->GetIntMin();
        iImageOffsetXMax = pDevice->GetRemoteNode("OffsetX")->GetIntMax();
        iImageOffsetXInc = pDevice->GetRemoteNode("OffsetX")->GetIntInc();

        std::cout << "          current value:          " << iImageOffsetX << std::endl;
        std::cout << "          possible value range:   " << iImageOffsetXMin << " to "  << iImageOffsetXMax << " with increment of " << iImageOffsetXInc << std::endl;

        //set new OffsetX value to the half value of the sensor width
        bo_int64 OffsetXvalue = pDevice->GetRemoteNode("SensorWidth")->GetInt() / 2 / iImageOffsetXInc * iImageOffsetXInc; // find number to match the increment

        // check new value is within range
        if( OffsetXvalue < iImageOffsetXMin)
            OffsetXvalue = iImageOffsetXMin;

        if( OffsetXvalue > iImageOffsetXMax)
            OffsetXvalue = iImageOffsetXMax;
        
        pDevice->GetRemoteNode("OffsetX")->SetInt(OffsetXvalue);

        //recheck new OffsetX is set
        std::cout << "          set value to:           " << pDevice->GetRemoteNode("OffsetX")->GetInt() << " is half value of the sensor width: " << pDevice->GetRemoteNode("SensorWidth")->GetInt() << std::endl << std::endl;


        //IMAGE HEIGHT
        //=============
        std::cout << "        Height" << std::endl;
        std::cout << "          description:            " << pDevice->GetRemoteNode("Height")->GetDescription() << std::endl;
        std::cout << "          interface type:         " << pDevice->GetRemoteNode("Height")->GetInterface() << std::endl;
        bo_int64 iImageHeight = 0;
        bo_int64 iImageHeightMin = 0;
        bo_int64 iImageHeightMax = 0;
        bo_int64 iImageHeightInc = 0;

        //get current value and limits
        iImageHeight = pDevice->GetRemoteNode("Height")->GetInt();
        iImageHeightMin = pDevice->GetRemoteNode("Height")->GetIntMin();
        iImageHeightMax = pDevice->GetRemoteNode("Height")->GetIntMax();
        iImageHeightInc = pDevice->GetRemoteNode("Height")->GetIntInc();

        std::cout << "          current value:          " << iImageHeight << std::endl;
        std::cout << "          possible value range:   " << iImageHeightMin << " to "  << iImageHeightMax << " with increment of " << iImageHeightInc << std::endl;

        //set new height value same to the half value of sensor height
        bo_int64 heightvalue = pDevice->GetRemoteNode("SensorHeight")->GetInt() / 2 / iImageHeightInc * iImageHeightInc; // find number to match the increment

        // check new value is within range
        if( heightvalue < iImageHeightMin)
            heightvalue = iImageHeightMin;

        if( heightvalue > iImageHeightMax)
            heightvalue = iImageHeightMax;
        
        pDevice->GetRemoteNode("Height")->SetInt(heightvalue);

        //recheck new height is set
        std::cout << "          set value to:           " << pDevice->GetRemoteNode("Height")->GetInt() << " is half value of the sensor height: " << pDevice->GetRemoteNode("SensorHeight")->GetInt() << std::endl << std::endl;
        

        //IMAGE OFFSET Y
        //===============
        std::cout << "        OffsetY" << std::endl;
        std::cout << "          description:            " << pDevice->GetRemoteNode("OffsetY")->GetDescription() << std::endl;
        std::cout << "          interface type:         " << pDevice->GetRemoteNode("OffsetY")->GetInterface() << std::endl;
        bo_int64 iImageOffsetY = 0;
        bo_int64 iImageOffsetYMin = 0;
        bo_int64 iImageOffsetYMax = 0;
        bo_int64 iImageOffsetYInc = 0;

        //get current value and limits
        iImageOffsetY = pDevice->GetRemoteNode("OffsetY")->GetInt();
        iImageOffsetYMin = pDevice->GetRemoteNode("OffsetY")->GetIntMin();
        iImageOffsetYMax = pDevice->GetRemoteNode("OffsetY")->GetIntMax();
        iImageOffsetYInc = pDevice->GetRemoteNode("OffsetY")->GetIntInc();

        std::cout << "          current value:          " << iImageOffsetY << std::endl;
        std::cout << "          possible value range:   " << iImageOffsetYMin << " to "  << iImageOffsetYMax << " with increment of " << iImageOffsetYInc << std::endl;

        //set new OffsetY value to the half value of the sensor height
        bo_int64 OffsetYvalue = pDevice->GetRemoteNode("SensorHeight")->GetInt() / 2 / iImageOffsetYInc * iImageOffsetYInc; // find number to match the increment

        // check new value is within range
        if( OffsetYvalue < iImageOffsetYMin)
            OffsetYvalue = iImageOffsetYMin;

        if( OffsetYvalue > iImageOffsetYMax)
            OffsetYvalue = iImageOffsetYMax;
        
        pDevice->GetRemoteNode("OffsetY")->SetInt(OffsetYvalue);

        //recheck new OffsetY is set
        std::cout << "          set value to:           " << pDevice->GetRemoteNode("OffsetY")->GetInt() << " is half value of the sensor height: " << pDevice->GetRemoteNode("SensorHeight")->GetInt() << std::endl << std::endl;

    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
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
            std::cout << "  5.2.4   DataStream ID:          " << dstIterator->first << std::endl << std::endl;
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

        // 4 buffers using external buffer mode
        std::cout << "5.4.2   External buffers" << std::endl;
        bo_uint64 payloadsize = pDataStream->GetDefinesPayloadSize() ? pDataStream->GetPayloadSize() : pDevice->GetPayloadSize();
        std::cout << "         payloadsize required for external buffer [bytes]: " << payloadsize << std::endl;
        for(int i=0; i<4; i++)
        {
            pMemoryBlock[i] = (char*)new char[(unsigned int)payloadsize];
            pBuffer = new BGAPI2::Buffer(pMemoryBlock[i], payloadsize, NULL);
            bufferList->Add(pBuffer);
            std::cout << "         add external buffer [" << i << "]" << std::endl;
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

    //CAPTURE 20 IMAGES
    std::cout << " " << std::endl;
    std::cout << "CAPTURE 20 IMAGES BY IMAGE POLLING USING EXTERNAL BUFFERS" << std::endl;
    std::cout << "#########################################################" << std::endl << std::endl;

    BGAPI2::Buffer * pBufferFilled = NULL;
    try
    {
        for(int i = 0; i < 20; i++)
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
                std::cout << " Image " << std::setw(5) << pBufferFilled->GetFrameID();
                std::cout << " received in memory address " << std::hex << pBufferFilled->GetMemPtr() << std::dec;
                std::cout << " size " << pBufferFilled->GetMemSize() << " [bytes]" << std::endl;
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

    BGAPI2::String sExposureNodeName = "";
    if (pDevice->GetRemoteNodeList()->GetNodePresent("ExposureTime")) {
        sExposureNodeName = "ExposureTime";
    } else if (pDevice->GetRemoteNodeList()->GetNodePresent("ExposureTimeAbs")) {
        sExposureNodeName = "ExposureTimeAbs";
    }
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

        for(int i=0; i<4; i++)
        {
            if (pMemoryBlock[i])
                delete [] pMemoryBlock[i];
        }
        std::cout << "5.4.2    memory blocks of external buffers deleted" << std::endl;
        pDataStream->Close();
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    //DEVICE PARAMETER RESET
    try
    {
        std::cout << "5.3.1    Reset ROI parameters" << std::endl;
        pDevice->GetRemoteNode("OffsetY")->SetInt(pDevice->GetRemoteNode("OffsetY")->GetIntMin());
        pDevice->GetRemoteNode("Height")->SetInt(pDevice->GetRemoteNode("Height")->GetIntMax());
        pDevice->GetRemoteNode("OffsetX")->SetInt(pDevice->GetRemoteNode("OffsetX")->GetIntMin());
        pDevice->GetRemoteNode("Width")->SetInt(pDevice->GetRemoteNode("Width")->GetIntMax());
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
        pDevice->Close();
        pInterface->Close();
        pSystem->Close();
        BGAPI2::SystemList::ReleaseInstance();
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
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
