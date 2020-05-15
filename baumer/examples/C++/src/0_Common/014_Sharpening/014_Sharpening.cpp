#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <algorithm>

#include "bgapi2_genicam/bgapi2_genicam.hpp"

using namespace BGAPI2;


int Test()
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

    int returncode = 0;

    std::cout << std::endl;
    std::cout << "######################" << std::endl;
    std::cout << "# 014_Sharpening.cpp #" << std::endl;
    std::cout << "######################" << std::endl;
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
        return returncode;
    }


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
                            if (deviceList->size() == 0)
                            {
                                std::cout << "5.1.13   Close interface (" << deviceList->size() << " cameras found) " << std::endl << std::endl;
                                ifIterator->second->Close();
                            }
                            else
                            {
                                sInterfaceID = ifIterator->first;
                                std::cout << "   " << std::endl;
                                std::cout << "        Opened interface - NodeList Information" << std::endl;
                                if (ifIterator->second->GetTLType() == "GEV")
                                {
                                    std::cout << "          GevInterfaceSubnetIPAddress: " << ifIterator->second->GetNode("GevInterfaceSubnetIPAddress")->GetValue() << std::endl;
                                    std::cout << "          GevInterfaceSubnetMask:      " << ifIterator->second->GetNode("GevInterfaceSubnetMask")->GetValue() << std::endl;
                                }
                                if (ifIterator->second->GetTLType() == "U3V")
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
                    // returncode = (returncode == 0) ? 1 : returncode;
                    std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
                    std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
                    std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
                }


                //if a camera is connected to the system interface then leave the system loop
                if (sInterfaceID != "")
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

    if (sSystemID == "")
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
        std::cout << " No interface found " << std::endl;
        std::cout << std::endl << "End" << std::endl;
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
            BGAPI2::Device* const pCurrentDevice = devIterator->second;

            std::cout << "  5.2.3   Device DeviceID:        " << devIterator->first << std::endl;
            std::cout << "          Device Model:           " << pCurrentDevice->GetModel() << std::endl;
            std::cout << "          Device SerialNumber:    " << pCurrentDevice->GetSerialNumber() << std::endl;
            std::cout << "          Device Vendor:          " << pCurrentDevice->GetVendor() << std::endl;
            std::cout << "          Device TLType:          " << pCurrentDevice->GetTLType() << std::endl;
            std::cout << "          Device AccessStatus:    " << pCurrentDevice->GetAccessStatus() << std::endl;
            std::cout << "          Device UserID:          " << pCurrentDevice->GetDisplayName() << std::endl << std::endl;
        }
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
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
            BGAPI2::Device* const pCurrentDevice = devIterator->second;
            try
            {
                std::cout << "5.1.7   Open first device " << std::endl;
                std::cout << "          Device DeviceID:        " << devIterator->first << std::endl;
                std::cout << "          Device Model:           " << pCurrentDevice->GetModel() << std::endl;
                std::cout << "          Device SerialNumber:    " << pCurrentDevice->GetSerialNumber() << std::endl;
                std::cout << "          Device Vendor:          " << pCurrentDevice->GetVendor() << std::endl;
                std::cout << "          Device TLType:          " << pCurrentDevice->GetTLType() << std::endl;
                std::cout << "          Device AccessStatus:    " << pCurrentDevice->GetAccessStatus() << std::endl;
                std::cout << "          Device UserID:          " << pCurrentDevice->GetDisplayName() << std::endl << std::endl;

                pCurrentDevice->Open();
                BGAPI2::NodeMap* const pDeviceRemoteNodeList = pCurrentDevice->GetRemoteNodeList();

                sDeviceID = devIterator->first;
                std::cout << "        Opened device - RemoteNodeList Information " << std::endl;
                std::cout << "          Device AccessStatus:    " << pCurrentDevice->GetAccessStatus() << std::endl;

                //SERIAL NUMBER
                if (pDeviceRemoteNodeList->GetNodePresent(SFNC_DEVICE_SERIALNUMBER))
                    std::cout << "          DeviceSerialNumber:     " << pCurrentDevice->GetRemoteNode(SFNC_DEVICE_SERIALNUMBER)->GetValue() << std::endl;
                else if (pDeviceRemoteNodeList->GetNodePresent(SFNC_DEVICE_ID))
                    std::cout << "          DeviceID (SN):          " << pCurrentDevice->GetRemoteNode(SFNC_DEVICE_ID)->GetValue() << std::endl;
                else
                    std::cout << "          SerialNumber:           Not Available " << std::endl;

                //DISPLAY DEVICEMANUFACTURERINFO
                if (pDeviceRemoteNodeList->GetNodePresent(SFNC_DEVICE_MANUFACTURERINFO))
                    std::cout << "          DeviceManufacturerInfo: " << pCurrentDevice->GetRemoteNode(SFNC_DEVICE_MANUFACTURERINFO)->GetValue() << std::endl;


                //DISPLAY DEVICEFIRMWAREVERSION OR DEVICEVERSION
                if (pDeviceRemoteNodeList->GetNodePresent(SFNC_DEVICE_FIRMWAREVERSION))
                    std::cout << "          DeviceFirmwareVersion:  " << pCurrentDevice->GetRemoteNode(SFNC_DEVICE_FIRMWAREVERSION)->GetValue() << std::endl;
                else if (pDeviceRemoteNodeList->GetNodePresent(SFNC_DEVICE_VERSION))
                    std::cout << "          DeviceVersion:          " << pCurrentDevice->GetRemoteNode(SFNC_DEVICE_VERSION)->GetValue() << std::endl;
                else
                    std::cout << "          DeviceVersion:          Not Available " << std::endl;

                if (pCurrentDevice->GetTLType() == "GEV")
                {
                    const bo_int64 iGevCurrentIpAddress = pCurrentDevice->GetRemoteNode(SFNC_GEV_CURRENTIPADDRESS)->GetInt();
                    const bo_int64 iGevCurrentSubnetMask = pCurrentDevice->GetRemoteNode(SFNC_GEV_CURRENTSUBNETMASK)->GetInt();
                    std::cout << "          GevCCP:                 " << pCurrentDevice->GetRemoteNode(SFNC_GEV_CCP)->GetValue() << std::endl;
                    std::cout << "          GevCurrentIPAddress:    " << ((iGevCurrentIpAddress & 0xff000000) >> 24) << "." << ((iGevCurrentIpAddress & 0x00ff0000) >> 16) << "." << ((iGevCurrentIpAddress & 0x0000ff00) >> 8) << "." << (iGevCurrentIpAddress & 0x0000ff) << std::endl;
                    std::cout << "          GevCurrentSubnetMask:   " << ((iGevCurrentSubnetMask & 0xff000000) >> 24) << "." << ((iGevCurrentSubnetMask & 0x00ff0000) >> 16) << "." << ((iGevCurrentSubnetMask & 0x0000ff00) >> 8) << "." << (iGevCurrentSubnetMask & 0x0000ff) << std::endl;
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
        std::cout << " No Device found " << std::endl;
        std::cout << std::endl << "End" << std::endl;
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
            pDataStream = dstIterator->second;

            std::cout << "        Opened datastream - NodeList Information " << std::endl;
            std::cout << "          StreamAnnounceBufferMinimum:  " << pDataStream->GetNode("StreamAnnounceBufferMinimum")->GetValue() << std::endl;
            if (dstIterator->second->GetTLType() == "GEV")
            {
                std::cout << "          StreamDriverModel:            " << pDataStream->GetNode("StreamDriverModel")->GetValue() << std::endl;
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
        for (int i = 0; i < 4; i++)
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

    std::cout << "SUPPORTED DEVICE PIXEL FORMAT" << std::endl;
    std::cout << "#############################" << std::endl << std::endl;
    std::set<BGAPI2::String> devicePixelFormat;
    BGAPI2::Node* const pDevicePixelFormat = pDevice->GetRemoteNode(SFNC_PIXELFORMAT);
    BGAPI2::NodeMap* pNodeMap = pDevicePixelFormat->GetEnumNodeList();
    bo_uint64 count = pNodeMap->GetNodeCount();
    for (bo_uint64 i = 0; i < count; i++) {
        try
        {
            BGAPI2::Node* pNode = pNodeMap->GetNodeByIndex(i);
            if ((pNode->GetImplemented()) && (pNode->GetAvailable())) {
                BGAPI2::String sPixelFormat = pNode->GetValue();
                devicePixelFormat.insert(sPixelFormat);
                std::cout << " " << sPixelFormat << std::endl;
            }
        }
        catch (BGAPI2::Exceptions::IException& ex)
        {
            returncode = (returncode == 0) ? 1 : returncode;
            std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
            std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
            std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
        }
    }
    std::cout << std::endl;

    BGAPI2::Node* pDemosaicingMethod = NULL;
    BGAPI2::Node* pSharpeningMode = NULL;
    BGAPI2::Node* pSharpeningFactor = NULL;
    BGAPI2::Node* pSharpeningSensitivityThreshold = NULL;
    BGAPI2::Node* pSharpeningSupportedPixelFormatIndex = NULL;
    BGAPI2::Node* pSharpeningSupportedPixelFormatValue = NULL;
    BGAPI2::Node* pSharpeningDemosaicingMethodIndex = NULL;
    BGAPI2::Node* pSharpeningDemosaicingMethodValue = NULL;

    BGAPI2::ImageProcessor* pImageProcessor = new BGAPI2::ImageProcessor();
    if (pImageProcessor != NULL) {
        try
        {
            BGAPI2::NodeMap* pNodeMap = pImageProcessor->GetNodeList();
            if (pNodeMap != NULL) {
                if (pNodeMap->GetNodePresent("DemosaicingMethod")) {
                    pDemosaicingMethod = pImageProcessor->GetNode("DemosaicingMethod");
                }

                if (pNodeMap->GetNodePresent("SharpeningMode")) {
                    pSharpeningMode = pNodeMap->GetNode("SharpeningMode");
                }

                if (pNodeMap->GetNodePresent("SharpeningFactor")) {
                    pSharpeningFactor = pNodeMap->GetNode("SharpeningFactor");
                }

                if (pNodeMap->GetNodePresent("SharpeningSensitivityThreshold")) {
                    pSharpeningSensitivityThreshold = pNodeMap->GetNode("SharpeningSensitivityThreshold");
                }

                if (pNodeMap->GetNodePresent("SharpeningSupportedPixelFormatIndex")) {
                    pSharpeningSupportedPixelFormatIndex = pNodeMap->GetNode("SharpeningSupportedPixelFormatIndex");
                }
                if (pNodeMap->GetNodePresent("SharpeningSupportedPixelFormatValue")) {
                    pSharpeningSupportedPixelFormatValue = pNodeMap->GetNode("SharpeningSupportedPixelFormatValue");
                }

                if (pNodeMap->GetNodePresent("SharpeningDemosaicingMethodIndex")) {
                    pSharpeningDemosaicingMethodIndex = pNodeMap->GetNode("SharpeningDemosaicingMethodIndex");
                }
                if (pNodeMap->GetNodePresent("SharpeningDemosaicingMethodValue")) {
                    pSharpeningDemosaicingMethodValue = pNodeMap->GetNode("SharpeningDemosaicingMethodValue");
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
    }

    BGAPI2::String sPixelFormatSrc = "";
    BGAPI2::String sPixelFormatDst = "";

    if ((pSharpeningSupportedPixelFormatIndex != NULL) && (pSharpeningSupportedPixelFormatValue != NULL)) {
        std::cout << "SUPPORTED SHARPENING PIXEL FORMAT" << std::endl;
        std::cout << "#################################" << std::endl << std::endl;
        try
        {
            BGAPI2::String sBayerFormat = "";

            bo_int64 iIndexMin = pSharpeningSupportedPixelFormatIndex->GetIntMin();
            bo_int64 iIndexMax = pSharpeningSupportedPixelFormatIndex->GetIntMax();
            for (bo_int64 iIndex = iIndexMin; iIndex <= iIndexMax; iIndex++) {
                pSharpeningSupportedPixelFormatIndex->SetInt(iIndex);
                BGAPI2::String sPixelFormat = pSharpeningSupportedPixelFormatValue->GetString();
                if (devicePixelFormat.find(sPixelFormat) != devicePixelFormat.end()) {
                    std::cout << " " << sPixelFormat << std::endl;
                    std::string sName = sPixelFormat.get();
                    if (sBayerFormat == "") {
                        if (sName.find("Bayer") != std::string::npos) {
                            sBayerFormat = sPixelFormat;
                        }
                        else if (sPixelFormatSrc == "") {
                            sPixelFormatSrc = sPixelFormat;
                            sPixelFormatDst = "Mono8";
                        }
                    }
                }
            }

            if (sBayerFormat != "") {
                sPixelFormatSrc = sBayerFormat;
                sPixelFormatDst = "BGR8";
            }
        }
        catch (BGAPI2::Exceptions::IException& ex)
        {
            returncode = (returncode == 0) ? 1 : returncode;
            std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
            std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
            std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
        }
        std::cout << std::endl;
    }

    BGAPI2::String sSharpeningDemosaicingMethod = "";

    if (pDemosaicingMethod != NULL) {
        BGAPI2::NodeMap* pNodeMap = pDemosaicingMethod->GetEnumNodeList();
        bo_uint64 count = pNodeMap->GetNodeCount();
        for (bo_uint64 i = 0; i < count; i++) {
            try
            {
                BGAPI2::Node* pNode = pNodeMap->GetNodeByIndex(i);
                if ((pNode->GetImplemented()) && (pNode->GetAvailable())) {
                    BGAPI2::String sPixelFormat = pNode->GetValue();
                    std::cout << " " << sPixelFormat << std::endl;
                }
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

    if ((pSharpeningDemosaicingMethodIndex != NULL) && (pSharpeningDemosaicingMethodValue != NULL)) {
        std::cout << "SHARPENING DEMOSAICING METHODS" << std::endl;
        std::cout << "##############################" << std::endl << std::endl;
        try
        {
            bo_int64 iIndexMin = pSharpeningDemosaicingMethodIndex->GetIntMin();
            bo_int64 iIndexMax = pSharpeningDemosaicingMethodIndex->GetIntMax();
            for (bo_int64 iIndex = iIndexMin; iIndex <= iIndexMax; iIndex++) {
                pSharpeningDemosaicingMethodIndex->SetInt(iIndex);
                BGAPI2::String sMethod = pSharpeningDemosaicingMethodValue->GetString();
                std::cout << " " << sMethod << std::endl;
                if (sSharpeningDemosaicingMethod == "") {
                    sSharpeningDemosaicingMethod = sMethod;
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
        std::cout << std::endl;
    }

    BGAPI2::String sSharpeningMode = "";
    std::vector<BGAPI2::String> sharpeningModes;
    if (pSharpeningMode != NULL) {
        std::cout << "SHARPENING MODES" << std::endl;
        std::cout << "################" << std::endl << std::endl;
        try
        {
            BGAPI2::NodeMap* pNodeMap = pSharpeningMode->GetEnumNodeList();
            bo_uint64 count = pNodeMap->GetNodeCount();
            for (bo_uint64 i = 0; i < count; i++) {
                BGAPI2::Node* pNode = pNodeMap->GetNodeByIndex(i);
                if ((pNode->GetImplemented()) && (pNode->GetAvailable())) {
                    BGAPI2::String sMode = pNode->GetValue();
                    std::cout << " " << sMode << std::endl;
                    sharpeningModes.push_back(sMode);
                    if (sSharpeningMode == "") {
                        sSharpeningMode = sMode;
                    }
                }
            }

            std::cout << " current: " << pSharpeningMode->GetValue() << std::endl;
        }
        catch (BGAPI2::Exceptions::IException& ex)
        {
            returncode = (returncode == 0) ? 1 : returncode;
            std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
            std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
            std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
        }
        std::cout << std::endl;
    }

    bo_int64 sharpeningFactorMin = 0;
    bo_int64 sharpeningFactorMax = 0;
    bo_int64 sharpeningFactor = 0;
    if (pSharpeningFactor != NULL) {
        std::cout << "SHARPENING FACTOR" << std::endl;
        std::cout << "#################" << std::endl << std::endl;
        try
        {
            sharpeningFactorMin = pSharpeningFactor->GetIntMin();
            sharpeningFactorMax = pSharpeningFactor->GetIntMax();
            sharpeningFactor = pSharpeningFactor->GetInt();

            std::cout << " min:     " << sharpeningFactorMin << std::endl;
            std::cout << " max:     " << sharpeningFactorMax << std::endl;
            std::cout << " current: " << sharpeningFactor << std::endl;
        }
        catch (BGAPI2::Exceptions::IException& ex)
        {
            returncode = (returncode == 0) ? 1 : returncode;
            std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
            std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
            std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
        }
        std::cout << std::endl;
    }

    bo_int64 sharpeningSensitivityThresholdMin = 0;
    bo_int64 sharpeningSensitivityThresholdMax = 0;
    bo_int64 sharpeningSensitivityThreshold = 0;
    if (pSharpeningSensitivityThreshold != NULL) {
        std::cout << "SHARPENING SENSITIVITY THRESHOLD" << std::endl;
        std::cout << "################################" << std::endl << std::endl;
        try
        {
            sharpeningSensitivityThresholdMin = pSharpeningSensitivityThreshold->GetIntMin();
            sharpeningSensitivityThresholdMax = pSharpeningSensitivityThreshold->GetIntMax();
            sharpeningSensitivityThreshold = pSharpeningSensitivityThreshold->GetInt();

            std::cout << " min:     " << sharpeningSensitivityThresholdMin << std::endl;
            std::cout << " max:     " << sharpeningSensitivityThresholdMax << std::endl;
            std::cout << " current: " << sharpeningSensitivityThreshold << std::endl;
        }
        catch (BGAPI2::Exceptions::IException& ex)
        {
            returncode = (returncode == 0) ? 1 : returncode;
            std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
            std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
            std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
        }
        std::cout << std::endl;
    }


    if (pDemosaicingMethod != NULL) {
        std::cout << "SET DEMOSAICING METHOD" << std::endl;
        std::cout << "#######################" << std::endl << std::endl;
        pDemosaicingMethod->SetString(sSharpeningDemosaicingMethod);
        std::cout << " " << pDemosaicingMethod->GetString() << std::endl;
        std::cout << std::endl;
    }

    const bo_uint64 uGetFilledBufferTimeout = 5000;

    //SEARCH FOR 'AcquisitionAbort'
    BGAPI2::Node* pDeviceAcquisitionAbort = NULL;
    if (pDevice->GetRemoteNodeList()->GetNodePresent(SFNC_ACQUISITION_ABORT))
    {
        pDeviceAcquisitionAbort = pDevice->GetRemoteNode(SFNC_ACQUISITION_ABORT);
    }

    BGAPI2::Node* const pDeviceAcquisitionStart = pDevice->GetRemoteNode(SFNC_ACQUISITION_START);
    BGAPI2::Node* const pDeviceAcquisitionStop = pDevice->GetRemoteNode(SFNC_ACQUISITION_STOP);

    // ENSURE CAMERA IS STOPPED TO SET PIXEL FORMAT
    try
    {
        if (pDeviceAcquisitionAbort)
        {
            pDeviceAcquisitionAbort->Execute();
            std::cout << "5.1.12   " << pDevice->GetModel() << " aborted " << std::endl;
        }

        pDeviceAcquisitionStop->Execute();
        std::cout << "5.1.12   " << pDevice->GetModel() << " stopped " << std::endl;
        std::cout << std::endl;
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }
    std::cout << std::endl;

    std::cout << "CAMERA START" << std::endl;
    std::cout << "############" << std::endl << std::endl;

    // SET PIXEL FORMAT
    std::cout << "         Set Pixel Format to " << sPixelFormatSrc << std::endl;
    pDevicePixelFormat->SetString(sPixelFormatSrc);
    std::cout << std::endl;

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
    std::cout << std::endl;

    //START CAMERA
    try
    {
        std::cout << "5.1.12   " << pDevice->GetModel() << " started " << std::endl << std::endl;
        pDeviceAcquisitionStart->Execute();
    }
    catch (BGAPI2::Exceptions::IException& ex)
    {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "ExceptionType:    " << ex.GetType() << std::endl;
        std::cout << "ErrorDescription: " << ex.GetErrorDescription() << std::endl;
        std::cout << "in function:      " << ex.GetFunctionName() << std::endl;
    }

    //CAPTURE <n> IMAGES
    std::cout << " " << std::endl;
    std::cout << "CAPTURE " << sharpeningModes.size() << " IMAGES" << std::endl;
    std::cout << "################" << std::endl << std::endl;

    try
    {
        size_t sharpeningMode = 0;
        BGAPI2::Image* pImageSrc = NULL;
        bo_uchar* pMemDst = NULL;
        bo_uint64 memSizeDst = 0;

        for (size_t i = 0; i < sharpeningModes.size(); i++)
        {
            //WAIT FOR IMAGE
            BGAPI2::Buffer* pBufferFilled = pDataStream->GetFilledBuffer(uGetFilledBufferTimeout); //timeout <n> msec
            if (pBufferFilled == NULL)
            {
                std::cout << "Error: Buffer Timeout after " << uGetFilledBufferTimeout << "msec" << std::endl;
            }
            else
            {
                if (pBufferFilled->GetIsIncomplete() == true)
                {
                    std::cout << "Error: Image is incomplete" << std::endl;
                }
                else
                {
                    const bo_uint w = static_cast<bo_uint>(pBufferFilled->GetWidth());
                    const bo_uint h = static_cast<bo_uint>(pBufferFilled->GetHeight());
                    void* const pMemSrc = pBufferFilled->GetMemPtr();
                    BGAPI2::String sPixelFormatSrc = pBufferFilled->GetPixelFormat();
                    const bo_uint64 memSizeSrc = pBufferFilled->GetMemSize();

                    std::cout << " Image " << std::setw(5) << pBufferFilled->GetFrameID() << " received in memory address " << std::hex << pMemSrc << std::dec
                        << " [" << sPixelFormatSrc << "]" << std::endl;

                    if (pImageSrc == NULL) {
                        pImageSrc = pImageProcessor->CreateImage(w, h, sPixelFormatSrc, pMemSrc, memSizeSrc);
                    }
                    else
                    {
                        pImageSrc->Init(w, h, sPixelFormatSrc, pMemSrc, memSizeSrc);
                    }

                    if (pImageSrc != NULL)
                    {
                        bo_uint64 bufferLength = pImageSrc->GetTransformBufferLength(sPixelFormatDst);

                        if (bufferLength > memSizeDst)
                        {
                            if (pMemDst != NULL) {
                                delete[] pMemDst;
                                pMemDst = NULL;
                                memSizeDst = 0;
                            }

                            pMemDst = new bo_uchar[static_cast<size_t>(bufferLength)];
                            memSizeDst = bufferLength;
                        }

                        if (sharpeningMode >= sharpeningModes.size())
                        {
                            sharpeningMode = 0;
                        }

                        std::cout << " convert to " << sPixelFormatDst << " - sharpening mode: " << sharpeningModes[sharpeningMode] << std::endl;
                        pSharpeningMode->SetValue(sharpeningModes[sharpeningMode]);
                        pImageProcessor->TransformImageToBuffer(pImageSrc, sPixelFormatDst, pMemDst, bufferLength);

                        sharpeningMode++;
                    }
                }

                // queue buffer again
                pBufferFilled->QueueBuffer();
            }
        }

        if (pImageSrc != NULL) {
            pImageSrc->Release();
        }

        if (pMemDst != NULL) {
            delete[] pMemDst;
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
        if (pDeviceAcquisitionAbort)
        {
            pDeviceAcquisitionAbort->Execute();
            std::cout << "5.1.12   " << pDevice->GetModel() << " aborted " << std::endl;
        }

        pDeviceAcquisitionStop->Execute();
        std::cout << "5.1.12   " << pDevice->GetModel() << " stopped " << std::endl;
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
        //DataStream Statistic
        BGAPI2::NodeMap* const pNodeList = pDataStream->GetNodeList();
        std::cout << "         DataStream Statistics " << std::endl;
        std::cout << "           GoodFrames:            " << pNodeList->GetNode("GoodFrames")->GetInt() << std::endl;
        std::cout << "           CorruptedFrames:       " << pNodeList->GetNode("CorruptedFrames")->GetInt() << std::endl;
        std::cout << "           LostFrames:            " << pNodeList->GetNode("LostFrames")->GetInt() << std::endl;
        if (pDataStream->GetTLType() == "GEV")
        {
            std::cout << "           ResendRequests:        " << pNodeList->GetNode("ResendRequests")->GetInt() << std::endl;
            std::cout << "           ResendPackets:         " << pNodeList->GetNode("ResendPackets")->GetInt() << std::endl;
            std::cout << "           LostPackets:           " << pNodeList->GetNode("LostPackets")->GetInt() << std::endl;
            std::cout << "           Bandwidth:             " << pNodeList->GetNode("Bandwidth")->GetInt() << std::endl;
        }
        std::cout << std::endl;

        pDataStream->StopAcquisition();
        std::cout << "5.1.12   DataStream stopped " << std::endl;

        bufferList->FlushAllToInputQueue();
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
        bufferList->DiscardAllBuffers();
        while (bufferList->size() > 0)
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

        if (pImageProcessor != NULL)
        {
            delete pImageProcessor;
        }
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
    return returncode;
}

int main()
{
    try {
        Test();
    }
    catch (BGAPI2::Exceptions::IException& e)
    {
        std::cout << "unhandled BGAPI exception \"" << e.GetType() << "\" " << e.GetFunctionName() << " " << e.GetErrorDescription() << "\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "unhandled exception: \"" << e.what() << "\"\n";
    }
    catch (...)
    {
        std::cout << "unhandled exception\n";
    }

    std::cout << "Input any number to close the program:";
    int endKey = 0;
    std::cin >> endKey;
    return 0;
}