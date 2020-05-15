#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "bgapi2_genicam/bgapi2_genicam.hpp"
using namespace BGAPI2;

Device *getDeviceByPortID(BGAPI2::String strPortID, DeviceList *deviceList) {

    for (DeviceList::iterator it = deviceList->begin(); it != deviceList->end(); it++) {
        Device *pDevice = (*it)->second;
        NodeMap* pDeviceNodeMap = pDevice->GetNodeList();
        if (pDeviceNodeMap->GetNodePresent("USBPortID")) {
            Node* pNode = pDeviceNodeMap->GetNode("USBPortID");
            if (pNode->GetValue() == strPortID.get()) {
                return pDevice;
            }
        }
    }
    return NULL;
}

int main() {
    int returncode = 0;
    try {
        std::cout << "# Example 301_PortIdents.cpp #" << std::endl;

        SystemList *systemList = SystemList::GetInstance();
        systemList->Refresh();

        System *pSystem = NULL;
        for (SystemList::iterator sysIterator = systemList->begin(); sysIterator != systemList->end(); sysIterator++) {
            // select U3V TL
            if (sysIterator->second->GetTLType() == "U3V") {
                pSystem = sysIterator->second;
                break;
            }
        }
        if (!pSystem) {
            std::cout << "error : U3V TL not found!" << std::endl;
        } else {
            std::cout << std::endl;

            pSystem->Open();

            NodeMap* pRootNodeMap = pSystem->GetNodeList();
            if (pRootNodeMap &&
                pRootNodeMap->GetNodePresent("USBPortSelector") &&
                pRootNodeMap->GetNodePresent("USBPortID") &&
                pRootNodeMap->GetNodePresent("USBPortLocationPath")) {

                // list all available PortID on USB TL
                Node* pSelector = pRootNodeMap->GetNode("USBPortSelector");
                if (pSelector->GetCurrentAccessMode() != "RW") {
                    std::cout << "error : no Port Idents found!" << std::endl;
                } else {
                    bo_int64 nMax = pSelector->GetIntMax();
                    std::cout << nMax + 1 << " Port Idents with devices found!" << std::endl << std::endl;
                    for (bo_int64 i = 0; i <= nMax; i++) {
                        pSelector->SetInt(i);
                        BGAPI2::String strPortID = pRootNodeMap->GetNode("USBPortID")->GetValue();
                        BGAPI2::String strLocPath = pRootNodeMap->GetNode("USBPortLocationPath")->GetValue();
                        std::cout << std::left << std::setw(2) << i + 1 << ".  " << std::setw(10) << strPortID << \
                            std::setw(0) << "    " << strLocPath << std::endl;
                    }
                }
                std::cout << std::endl;

                InterfaceList *interfaceList = pSystem->GetInterfaces();
                interfaceList->Refresh(100);
                Interface *pInterface = interfaceList->begin()->second;
                pInterface->Open();

                // search a (first) portident and device ident with camera on interface
                BGAPI2::String strFirstDeviceID;
                BGAPI2::String strFirstPortID;
                {
                    DeviceList *deviceList = pInterface->GetDevices();
                    deviceList->Refresh(100);  // collect all devices on interface
                    if (deviceList->size() > 0) {
                        Device *pDevice = deviceList->begin()->second;  // first cam on interface
                        NodeMap* pDeviceNodeMap = pDevice->GetNodeList();
                        if (pDeviceNodeMap->GetNodePresent("DeviceID")) {
                            Node* pNode = pDeviceNodeMap->GetNode("DeviceID");
                            strFirstDeviceID = pNode->GetValue();
                        }
                        if (pDeviceNodeMap->GetNodePresent("USBPortID")) {
                            Node* pNode = pDeviceNodeMap->GetNode("USBPortID");
                            strFirstPortID = pNode->GetValue();
                        }
                        std::cout << std::endl;
                    } else {
                        std::cout << "no camera found on u3v system and first interface." << std::endl;
                    }
                }
                // open camera by (known) PortID
                {
                    std::cout << "open camera by PortID: " << strFirstPortID << std::endl;

                    DeviceList *deviceList = pInterface->GetDevices();
                    deviceList->Refresh(100);
                    if (deviceList->size() > 0) {
                        Device *pDevice = getDeviceByPortID(strFirstPortID, deviceList);
                        if (pDevice) {
                            BGAPI2::String strID = pDevice->GetID();
                            std::cout << "    " << std::left << std::setw(18) << "DeviceID: " << strID << std::endl << std::endl;

                            NodeMap* pDeviceNodeMap = pDevice->GetNodeList();

                            if (pDeviceNodeMap->GetNodePresent("DeviceID")) {
                                Node* pNode = pDeviceNodeMap->GetNode("DeviceID");
                                std::cout << "    " << std::left << std::setw(18) << "DeviceID: " << pNode->GetValue() << std::endl;
                            }
                            if (pDeviceNodeMap->GetNodePresent("USBPortID")) {
                                Node* pNode = pDeviceNodeMap->GetNode("USBPortID");
                                std::cout << "    " << std::left << std::setw(18) << "USBPortID: " << pNode->GetValue() << std::endl;
                            }
                            if (pDeviceNodeMap->GetNodePresent("USB3VisionGUID")) {
                                Node* pNode = pDeviceNodeMap->GetNode("USB3VisionGUID");
                                std::cout << "    " << std::left << std::setw(18) << "USB3VisionGUID: " << pNode->GetValue() << std::endl;
                            }
                            std::cout << std::endl;
                            pDevice->Open();
                            std::cout << "    " << pDevice->GetModel() << "(" << pDevice->GetSerialNumber() << ")" << std::endl;
                            pDevice->Close();
                            std::cout << std::endl;
                        }
                    } else {
                        std::cout << "no camera found on u3v system and first interface." << std::endl;
                    }
                    std::cout << std::endl;
                }
                // open camera by DeviceID
                {
                    std::cout << "open camera by DeviceID: " << strFirstDeviceID << std::endl;

                    DeviceList *deviceList = pInterface->GetDevices();
                    Device *pDevice = (*deviceList)[strFirstDeviceID];
                    if (pDevice) {
                        std::cout << std::endl;
                        pDevice->Open();
                        std::cout << "    " << pDevice->GetModel() << "(" << pDevice->GetSerialNumber() << ")" << std::endl;
                        pDevice->Close();
                    } else {
                        std::cout << "no camera found on u3v system and first interface." << std::endl;
                    }
                    std::cout << std::endl;
                }

                pInterface->Close();

            }
            pSystem->Close();
        }
        BGAPI2::SystemList::ReleaseInstance();
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        returncode = (returncode == 0) ? 1 : returncode;
        std::cout << "Error in function: " << ex.GetFunctionName() << std::endl << "Error description: " << ex.GetErrorDescription() << std::endl << std::endl;
        BGAPI2::SystemList::ReleaseInstance();
    }
    std::cout << "Input any number to close the program:";
    int endKey = 0;
    std::cin >> endKey;
    return returncode;
}