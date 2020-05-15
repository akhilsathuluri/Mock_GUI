/*
This example describes the FIRST STEPS of handling Baumer-GAPI SDK.
The given source code applies to handling one system, one camera and twelfe images.
Please see "Baumer-GAPI SDK Programmer's Guide" chapter 5.1 and chapter 5.2
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bgapi2_genicam/bgapi2_genicam.h"

#define BUFFERCOUNT 4

BGAPI2_RESULT printLastError() {
    BGAPI2_RESULT res;
    char* pString = NULL;
    bo_uint64  iStringlength = 0;
    BGAPI2_GetLastError(&res, NULL, &iStringlength);
    pString = malloc((int)iStringlength);
    BGAPI2_GetLastError(&res, pString, &iStringlength);
    printf("Error: %s", pString);
    free(pString);
    return res;
}

// SYSTEM INFORMATION
BGAPI2_RESULT getSystemInfo(BGAPI2_System* pSystem) {
    char* pString = NULL;
    bo_uint64  iStringlength;

    iStringlength = 0;
    if (BGAPI2_System_GetFileName(pSystem, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pString = malloc((int)iStringlength);
    if (BGAPI2_System_GetFileName(pSystem, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("  5.2.1   System Name:     %s\n", pString);
    free(pString);

    iStringlength = 0;
    if (BGAPI2_System_GetTLType(pSystem, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pString = malloc((int)iStringlength);
    if (BGAPI2_System_GetTLType(pSystem, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          System Type:     %s\n", pString);
    free(pString);

    iStringlength = 0;
    if (BGAPI2_System_GetVersion(pSystem, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pString = malloc((int)iStringlength);
    if (BGAPI2_System_GetVersion(pSystem, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          System Version:  %s\n", pString);
    free(pString);

    iStringlength = 0;
    if (BGAPI2_System_GetPathName(pSystem, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pString = malloc((int)iStringlength);
    if (BGAPI2_System_GetPathName(pSystem, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          System PathName: %s\n", pString);
    free(pString);

    printf("\n");
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT updateSystems(bo_uint* pNumSystems) {
    bo_uint iIndexTLProducer = 0;
    printf("SYSTEM LIST\n");
    printf("###########\n\n");

    // COUNTING AVAILABLE SYSTEMS (TL producers)
    if (BGAPI2_UpdateSystemList() != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_GetNumSystems(pNumSystems) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("5.1.2   Detected systems:  %d\n", *pNumSystems);
    for (iIndexTLProducer = 0; iIndexTLProducer < *pNumSystems; iIndexTLProducer++) {
        BGAPI2_System * pSystem = NULL;
        // SYSTEM LOAD (TL producer)
        if (BGAPI2_GetSystem(iIndexTLProducer, &pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (getSystemInfo(pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_ReleaseSystem(pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    }
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT getInterfaceInfo(BGAPI2_Interface* pInterface) {
    char* pString = NULL;
    bo_uint64  iStringlength;

    iStringlength = 0;
    if (BGAPI2_Interface_GetDisplayName(pInterface, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pString = malloc((int)iStringlength);
    if (BGAPI2_Interface_GetDisplayName(pInterface, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("  5.2.1   Interface Name:     %s\n", pString);
    free(pString);

    iStringlength = 0;
    if (BGAPI2_Interface_GetTLType(pInterface, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pString = malloc((int)iStringlength);
    if (BGAPI2_Interface_GetTLType(pInterface, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          Interface Type:     %s\n", pString);
    free(pString);
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT updateInterfaces(BGAPI2_System * pSystem, bo_uint* pNumInterfaces) {
    bo_bool bChanged = 0;
    bo_uint iIndexInterface = 0;
    //COUNT AVAILABLE INTERFACES
    if (BGAPI2_System_UpdateInterfaceList(pSystem, &bChanged, 100) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_System_GetNumInterfaces(pSystem, pNumInterfaces) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("5.1.4   Detected interfaces: %d\n", *pNumInterfaces);
    for (iIndexInterface = 0; iIndexInterface < *pNumInterfaces; iIndexInterface++) {
        BGAPI2_Interface* pInterface = NULL;
        if (BGAPI2_System_GetInterface(pSystem, iIndexInterface, &pInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (getInterfaceInfo(pInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("\n");
    }
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT openInterface(BGAPI2_Interface* pInterface, bo_uint* pNumDevices) {
    bo_bool bChanged = 0;
    printf("5.1.5   Open interface\n");
    //INTERFACE OPEN
    if (BGAPI2_Interface_Open(pInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (getInterfaceInfo(pInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    //search for any camera is connected to this interface
    if (BGAPI2_Interface_UpdateDeviceList(pInterface, &bChanged, 100) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Interface_GetNumDevices(pInterface, pNumDevices) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (*pNumDevices == 0) {
        printf("5.1.13   Close interface (0 cameras found)\n\n");
        if (BGAPI2_Interface_Close(pInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    } else {
        BGAPI2_Node * nNode = NULL;
        char* pString = NULL;
        bo_uint64  iStringlength = 0;
        if (BGAPI2_Interface_GetTLType(pInterface, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        pString = malloc((int)iStringlength);
        if (BGAPI2_Interface_GetTLType(pInterface, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (strcmp(pString, "GEV") == 0) {
            free(pString);
            BGAPI2_Interface_GetNode(pInterface, "GevInterfaceSubnetIPAddress", &nNode);
            iStringlength = 0;
            if (BGAPI2_Node_GetValue(nNode, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            pString = malloc((int)iStringlength);
            if (BGAPI2_Node_GetValue(nNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf("          GevInterfaceSubnetIPAddress: %s\n", pString);
            free(pString);
            BGAPI2_Interface_GetNode(pInterface, "GevInterfaceSubnetMask", &nNode);
            iStringlength = 0;
            if (BGAPI2_Node_GetValue(nNode, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            pString = malloc((int)iStringlength);
            if (BGAPI2_Node_GetValue(nNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf("          GevInterfaceSubnetMask:      %s\n", pString);
        }
        free(pString);
    }
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT getFirstInterfaceWithDevice(bo_uint NumSystems, BGAPI2_Interface** ppInterface) {
    BGAPI2_RESULT res = BGAPI2_RESULT_SUCCESS;
    bo_uint iIndexSystem = 0;
    BGAPI2_System * pSystem = NULL;
    for (iIndexSystem = 0; iIndexSystem < NumSystems; iIndexSystem++) {
        BGAPI2_Node * nNode = NULL;
        bo_int64 iMajor = 0;
        bo_int64 iMinor = 0;
        bo_uint iNumInterfaces = 0;
        bo_uint iIndexInterface = 0;
        printf("SYSTEM\n");
        printf("######\n\n");
        // SYSTEM LOAD (TL producer)
        if (BGAPI2_GetSystem(iIndexSystem, &pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        // SYSTEM OPEN
        printf("5.1.3   Open next system \n");
        if (BGAPI2_System_Open(pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (getSystemInfo(pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("        Opened system - NodeList Information \n");
        if (BGAPI2_System_GetNode(pSystem, "GenTLVersionMajor", &nNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetInt(nNode, &iMajor) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_System_GetNode(pSystem, "GenTLVersionMinor", &nNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetInt(nNode, &iMinor) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("          GenTL Version:   %lld.%lld\n", (long long)iMajor, (long long)iMinor);
        printf("\nINTERFACE LIST\n");
        printf("##############\n\n");
        if (updateInterfaces(pSystem, &iNumInterfaces)) {
            res = printLastError();
        }
        printf("INTERFACE\n");
        printf("#########\n\n");
        for (iIndexInterface = 0; iIndexInterface < iNumInterfaces; iIndexInterface++) {
            bo_uint iNumDevices = 0;
            if (BGAPI2_System_GetInterface(pSystem, iIndexInterface, ppInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (openInterface(*ppInterface, &iNumDevices) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (iNumDevices) {
                break;
            }
            *ppInterface = NULL;
        }
        if (*ppInterface != NULL) {
            break;
        } else {
            if (BGAPI2_System_Close(pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_ReleaseSystem(pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        }
    }
    return res;
}
BGAPI2_RESULT getDeviceInfo(BGAPI2_Device* pDevice) {
    char* pString = NULL;
    bo_uint64  iStringlength = 0;
    if (BGAPI2_Device_GetModel(pDevice, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pString = malloc((int)iStringlength);
    if (BGAPI2_Device_GetModel(pDevice, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          Device Model:           %s\n", pString);
    free(pString);
    iStringlength = 0;
    if (BGAPI2_Device_GetSerialNumber(pDevice, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pString = malloc((int)iStringlength);
    if (BGAPI2_Device_GetSerialNumber(pDevice, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          Device SerialNumber:    %s\n", pString);
    free(pString);
    iStringlength = 0;
    if (BGAPI2_Device_GetVendor(pDevice, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pString = malloc((int)iStringlength);
    if (BGAPI2_Device_GetVendor(pDevice, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          Device Vendor:          %s\n", pString);
    free(pString);
    iStringlength = 0;
    if (BGAPI2_Device_GetTLType(pDevice, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pString = malloc((int)iStringlength);
    if (BGAPI2_Device_GetTLType(pDevice, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          Device TLType:          %s\n", pString);
    free(pString);
    iStringlength = 0;
    if (BGAPI2_Device_GetAccessStatus(pDevice, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pString = malloc((int)iStringlength);
    if (BGAPI2_Device_GetAccessStatus(pDevice, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          Device AccessStatus:    %s\n", pString);
    free(pString);
    iStringlength = 0;
    if (BGAPI2_Device_GetDisplayName(pDevice, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pString = malloc((int)iStringlength);
    if (BGAPI2_Device_GetDisplayName(pDevice, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          Device DisplayName:     %s\n", pString);
    free(pString);
    printf("\n");
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT updateDevices(BGAPI2_Interface* pInterface, bo_uint* pNumDevices) {
    bo_uint iNumDevices = 0;
    bo_uint iIndexDevice = 0;
    bo_bool bChanged = 0;
    printf("\nDEVICE LIST\n");
    printf("###########\n\n");

    // COUNT AVAILABLE DEVICES
    if (BGAPI2_Interface_UpdateDeviceList(pInterface, &bChanged, 100) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Interface_GetNumDevices(pInterface, &iNumDevices) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("5.1.6   Detected devices:         %d\n", iNumDevices);
    for (iIndexDevice = 0; iIndexDevice < iNumDevices; iIndexDevice++) {
        BGAPI2_Device* pDevice = NULL;
        if (BGAPI2_Interface_GetDevice(pInterface, iIndexDevice, &pDevice) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (getDeviceInfo(pDevice) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    }
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT getFirstDevice(BGAPI2_Interface* pInterface, BGAPI2_Device** ppDevice) {
    bo_uint iNumDevices = 0;
    bo_uint iIndexDevice = 0;
    bo_bool bChanged = 0;
    if (BGAPI2_Interface_UpdateDeviceList(pInterface, &bChanged, 100) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Interface_GetNumDevices(pInterface, &iNumDevices) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("5.1.6   Detected devices:         %d\n", iNumDevices);
    for (iIndexDevice = 0; iIndexDevice < iNumDevices; iIndexDevice++) {
        if (BGAPI2_Interface_GetDevice(pInterface, iIndexDevice, ppDevice) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (getDeviceInfo(*ppDevice) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Device_Open(*ppDevice) == BGAPI2_RESULT_SUCCESS) {
            char* pString = NULL;
            bo_uint64  iStringlength = 0;
            BGAPI2_NodeMap* pDeviceRemoteNodeList = NULL;
            bo_bool bNodePresent;
            BGAPI2_Node* pNode = NULL;
            printf("        Opened device - RemoteNodeList Information \n");
            if (BGAPI2_Device_GetAccessStatus(*ppDevice, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            pString = malloc((int)iStringlength);
            if (BGAPI2_Device_GetAccessStatus(*ppDevice, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf("          Device AccessStatus:        %s\n", pString);
            free(pString);
            if (BGAPI2_Device_GetRemoteNodeList(*ppDevice, &pDeviceRemoteNodeList) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            // DEVICE SERIAL NUMBER
            if (BGAPI2_NodeMap_GetNodePresent(pDeviceRemoteNodeList, "DeviceSerialNumber", &bNodePresent) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (bNodePresent == 1) {
                if (BGAPI2_Device_GetRemoteNode(*ppDevice, "DeviceSerialNumber", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                iStringlength = 0;
                if (BGAPI2_Node_GetValue(pNode, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                pString = malloc((int)iStringlength);
                if (BGAPI2_Node_GetValue(pNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf("          DeviceSerialNumber:     %s\n", pString);
                free(pString);
            } else {
                if (BGAPI2_NodeMap_GetNodePresent(pDeviceRemoteNodeList, "DeviceID", &bNodePresent) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                if (bNodePresent == 1) {
                    if (BGAPI2_Device_GetRemoteNode(*ppDevice, "DeviceID", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                    iStringlength = 0;
                    if (BGAPI2_Node_GetValue(pNode, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                    pString = malloc((int)iStringlength);
                    if (BGAPI2_Node_GetValue(pNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                    printf("          DeviceID (SN):          %s\n", pString);
                    free(pString);
                } else {
                    printf("          SerialNumber:           Not Available \n");
                }
            }
            // DEVICE MANUFACTURERINFO
            if (BGAPI2_NodeMap_GetNodePresent(pDeviceRemoteNodeList, "DeviceManufacturerInfo", &bNodePresent) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (bNodePresent == 1) {
                if (BGAPI2_Device_GetRemoteNode(*ppDevice, "DeviceManufacturerInfo", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                iStringlength = 0;
                if (BGAPI2_Node_GetValue(pNode, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                pString = malloc((int)iStringlength);
                if (BGAPI2_Node_GetValue(pNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf("          DeviceManufacturerInfo: %s\n", pString);
                free(pString);
            } else {
                printf("          DeviceManufacturerInfo: Not Available \n");
            }
            // DEVICE FIRMWAREVERSION OR VERSION
            if (BGAPI2_NodeMap_GetNodePresent(pDeviceRemoteNodeList, "DeviceFirmwareVersion", &bNodePresent) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (bNodePresent == 1) {
                if (BGAPI2_Device_GetRemoteNode(*ppDevice, "DeviceFirmwareVersion", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                iStringlength = 0;
                if (BGAPI2_Node_GetValue(pNode, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                pString = malloc((int)iStringlength);
                if (BGAPI2_Node_GetValue(pNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf("          DeviceFirmwareVersion:  %s\n", pString);
                free(pString);
            } else {
                if (BGAPI2_NodeMap_GetNodePresent(pDeviceRemoteNodeList, "DeviceVersion", &bNodePresent) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                if (bNodePresent == 1) {
                    if (BGAPI2_Device_GetRemoteNode(*ppDevice, "DeviceVersion", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                    iStringlength = 0;
                    if (BGAPI2_Node_GetValue(pNode, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                    pString = malloc((int)iStringlength);
                    if (BGAPI2_Node_GetValue(pNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                    printf("          DeviceVersion:          %s\n", pString);
                    free(pString);
                } else {
                    printf("          DeviceVersion:          Not Available \n");
                }
            }

            if (BGAPI2_Device_GetTLType(*ppDevice, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            pString = malloc((int)iStringlength);
            if (BGAPI2_Device_GetTLType(*ppDevice, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (strcmp(pString, "GEV") == 0) {
                free(pString);
                BGAPI2_Device_GetRemoteNode(*ppDevice, "GevCCP", &pNode);
                iStringlength = 0;
                if (BGAPI2_Node_GetValue(pNode, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                pString = malloc((int)iStringlength);
                if (BGAPI2_Node_GetValue(pNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf("          GevCCP:                 %s\n", pString);
                free(pString);
                BGAPI2_Device_GetRemoteNode(*ppDevice, "GevCurrentIPAddress", &pNode);
                iStringlength = 0;
                if (BGAPI2_Node_GetValue(pNode, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                pString = malloc((int)iStringlength);
                if (BGAPI2_Node_GetValue(pNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf("          GevCurrentIPAddress:    %s\n", pString);
                free(pString);
                BGAPI2_Device_GetRemoteNode(*ppDevice, "GevCurrentSubnetMask", &pNode);
                iStringlength = 0;
                if (BGAPI2_Node_GetValue(pNode, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                pString = malloc((int)iStringlength);
                if (BGAPI2_Node_GetValue(pNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf("          GevCurrentSubnetMask:   %s\n", pString);
            }
            free(pString);
            printf("\n");
            break;
        }
        *ppDevice = NULL;
    }
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT setDeviceParameter(BGAPI2_Device* pDevice) {
    BGAPI2_Node* pNode = NULL;
    char* pString = NULL;
    bo_uint64  iStringlength = 0;
    printf("DEVICE PARAMETER SETUP\n");
    printf("######################\n\n");
    // TRIGGER MODE OFF - SET
    if (BGAPI2_Device_GetRemoteNode(pDevice, "TriggerMode", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_SetString(pNode, "Off") != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetValue(pNode, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pString = malloc((int)iStringlength);
    if (BGAPI2_Node_GetValue(pNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          TriggerMode:          %s\n", pString);
    free(pString);
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT getDataStreamCount(BGAPI2_Device* pDevice, bo_uint* pNumDataStreams) {
    printf("\nDATA STREAM LIST\n");
    printf("################\n\n");
    // COUNTING AVAILABLE DATA STREAMS
    if (BGAPI2_Device_GetNumDataStreams(pDevice, pNumDataStreams) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("5.1.8   Detected datastreams:     %d\n", *pNumDataStreams);
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT getFirstDataStream(BGAPI2_Device* pDevice, BGAPI2_DataStream** ppDataStream) {
    bo_uint iNumDataStreams = 0;
    bo_uint iIndexDataStreams = 0;
    printf("\nDATA STREAM\n");
    printf("###########\n\n");
    if (BGAPI2_Device_GetNumDataStreams(pDevice, &iNumDataStreams) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    for (iIndexDataStreams = 0; iIndexDataStreams < iNumDataStreams; iIndexDataStreams++) {
        printf("5.1.9   Open first datastream \n");
        if (BGAPI2_Device_GetDataStream(pDevice, iIndexDataStreams, ppDataStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_DataStream_Open(*ppDataStream) == BGAPI2_RESULT_SUCCESS) {
            BGAPI2_Node* pNode = NULL;
            bo_int64 iNodeValue = 0;
            char* pString = NULL;
            bo_uint64  iStringlength = 0;
            printf("        Opened datastream - NodeList Information \n");
            if (BGAPI2_DataStream_GetNode(*ppDataStream, "StreamAnnounceBufferMinimum", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf("          StreamAnnounceBufferMinimum:  %lld\n", (long long)iNodeValue);

            if (BGAPI2_DataStream_GetTLType(*ppDataStream, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            pString = malloc((int)iStringlength);
            if (BGAPI2_DataStream_GetTLType(*ppDataStream, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (strcmp(pString, "GEV") == 0) {
                free(pString);
                if (BGAPI2_DataStream_GetNode(*ppDataStream, "StreamDriverModel", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                iStringlength = 0;
                if (BGAPI2_Node_GetValue(pNode, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                pString = malloc((int)iStringlength);
                if (BGAPI2_Node_GetValue(pNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf("          StreamDriverModel: %s\n", pString);
            }
            free(pString);
            printf("\n");
            break;
        }
        *ppDataStream = NULL;
    }
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT addBuffers(BGAPI2_DataStream* pDataStream) {
    bo_uint iIndexBuffers = 0;
    BGAPI2_Buffer* pBuffer = NULL;
    printf("BUFFER LIST\n");
    printf("###########\n\n");
    // DATASTREAM ALLOC_AND_ANNOUNCE BUFFERS
    for (iIndexBuffers = 0; iIndexBuffers < BUFFERCOUNT; iIndexBuffers++) {
        if (BGAPI2_CreateBuffer(&pBuffer) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_DataStream_AnnounceBuffer(pDataStream, pBuffer) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_DataStream_QueueBuffer(pDataStream, pBuffer) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    }
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT startStreaming(BGAPI2_Device* pDevice, BGAPI2_DataStream* pDataStream) {
    BGAPI2_Node* pNode = NULL;
    char* pString = NULL;
    bo_uint64  iStringlength = 0;
    printf("CAMERA START\n");
    printf("############\n\n");
    // DATASTREAM START
    if (BGAPI2_DataStream_StartAcquisitionContinuous(pDataStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("5.1.12   DataStream started \n");
    if (BGAPI2_Device_GetRemoteNode(pDevice, "AcquisitionStart", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_Execute(pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Device_GetModel(pDevice, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pString = malloc((int)iStringlength);
    if (BGAPI2_Device_GetModel(pDevice, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("5.1.12   %s started \n", pString);
    free(pString);
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT captureFrames(BGAPI2_DataStream* pDataStream) {
    int framecount = 0;
    printf("\nCAPTURE 12 IMAGES BY IMAGE POLLING\n");
    printf("##################################\n\n");
    for (framecount = 0; framecount < 12; framecount++) {
        BGAPI2_Buffer* pBufferFilled = NULL;
        bo_bool bIsIncomplete;
        BGAPI2_RESULT res = BGAPI2_DataStream_GetFilledBuffer(pDataStream, &pBufferFilled, 1000);  // timeout 1000 msec
        if (res != BGAPI2_RESULT_SUCCESS) {
            if (res == BGAPI2_RESULT_TIMEOUT) {
                printf("Error: Buffer Timeout after 1000 msec\n");
            } else {
                printf("BGAPI2_DataStream_GetFilledBuffer errorcode: %d\n", res);
            }
        } else {
            if (BGAPI2_Buffer_GetIsIncomplete(pBufferFilled, &bIsIncomplete) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (bIsIncomplete == 1) {
                printf("Error: Image is incomplete\n");
            } else {
                bo_uint64 iFrameId = 0;
                void* pBufferAddress = NULL;
                if (BGAPI2_Buffer_GetFrameID(pBufferFilled, &iFrameId) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf("    Image %lld received in buffer address ", (long long)iFrameId);
                if (BGAPI2_Buffer_GetMemPtr(pBufferFilled, &pBufferAddress) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf("%p \n", pBufferAddress);
            }
            //QUEUE BUFFER
            if (BGAPI2_DataStream_QueueBuffer(pDataStream, pBufferFilled) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        }
    }  // Image Capture Loop end
    printf(" \n");
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT getStreamRelatedInformations(BGAPI2_Device* pDevice, BGAPI2_DataStream* pDataStream) {
    BGAPI2_Node* pNode = NULL;
    bo_double fNodeValue = 0.0;
    char* pString = NULL;
    bo_uint64  iStringlength = 0;
    bo_int64 iNodeValue = 0;

    // EXPOSURE TIME
    if (BGAPI2_Device_GetRemoteNode(pDevice, "ExposureTime", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetDouble(pNode, &fNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetUnit(pNode, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pString = malloc((int)iStringlength);
    if (BGAPI2_Node_GetUnit(pNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("         ExposureTime:                   %.0f [%s]\n", fNodeValue, pString);
    free(pString);
    iStringlength = 0;
    if (BGAPI2_DataStream_GetTLType(pDataStream, NULL, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pString = malloc((int)iStringlength);
    if (BGAPI2_DataStream_GetTLType(pDataStream, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (strcmp(pString, "GEV") == 0) {
        BGAPI2_NodeMap* pDeviceRemoteNodeList = NULL;
        bo_bool bNodePresent = 0;
        if (BGAPI2_Device_GetRemoteNodeList(pDevice, &pDeviceRemoteNodeList) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_NodeMap_GetNodePresent(pDeviceRemoteNodeList, "DeviceStreamChannelPacketSize", &bNodePresent) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (bNodePresent == 1) {
            if (BGAPI2_Device_GetRemoteNode(pDevice, "DeviceStreamChannelPacketSize", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf("         DeviceStreamChannelPacketSize:  %lld [bytes]\n", (long long)iNodeValue);
        } else {
            if (BGAPI2_Device_GetRemoteNode(pDevice, "GevSCPSPacketSize", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf("         GevSCPSPacketSize:  %lld [bytes]\n", (long long)iNodeValue);
        }
        if (BGAPI2_Device_GetRemoteNode(pDevice, "GevSCPD", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("         GevSCPD (PacketDelay):          %lld [tics]\n", (long long)iNodeValue);
    }
    printf("\n");

    // DATASTREAM STATISTICS
    if (strcmp(pString, "GEV") == 0) {
        printf("         DataStream Statistics \n");
        if (BGAPI2_DataStream_GetNode(pDataStream, "DataBlockComplete", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("           DataBlockComplete:              %lld\n", (long long)iNodeValue);
        if (BGAPI2_DataStream_GetNode(pDataStream, "DataBlockInComplete", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("           DataBlockInComplete:            %lld\n", (long long)iNodeValue);
        if (BGAPI2_DataStream_GetNode(pDataStream, "DataBlockMissing", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("           DataBlockMissing:               %lld\n", (long long)iNodeValue);
        if (BGAPI2_DataStream_GetNode(pDataStream, "PacketResendRequestSingle", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("           PacketResendRequestSingle:      %lld\n", (long long)iNodeValue);
        if (BGAPI2_DataStream_GetNode(pDataStream, "PacketResendRequestRange", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("           PacketResendRequestRange:       %lld\n", (long long)iNodeValue);
        if (BGAPI2_DataStream_GetNode(pDataStream, "PacketResendReceive", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("           PacketResendReceive:            %lld\n", (long long)iNodeValue);
        if (BGAPI2_DataStream_GetNode(pDataStream, "DataBlockDroppedBufferUnderrun", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("           DataBlockDroppedBufferUnderrun: %lld\n", (long long)iNodeValue);
        if (BGAPI2_DataStream_GetNode(pDataStream, "Bitrate", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetDouble(pNode, &fNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("           Bitrate:                        %.1f\n", fNodeValue);
        if (BGAPI2_DataStream_GetNode(pDataStream, "Throughput", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetDouble(pNode, &fNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("           Throughput:                     %.1f\n", fNodeValue);
    } else if (strcmp(pString, "U3V") == 0) {
        printf("         DataStream Statistics \n");
        if (BGAPI2_DataStream_GetNode(pDataStream, "GoodFrames", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("           GoodFrames:              %lld\n", (long long)iNodeValue);
        if (BGAPI2_DataStream_GetNode(pDataStream, "CorruptedFrames", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("           CorruptedFrames:         %lld\n", (long long)iNodeValue);
        if (BGAPI2_DataStream_GetNode(pDataStream, "LostFrames", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("           LostFrames:              %lld\n", (long long)iNodeValue);
    }
    free(pString);
    // BUFFERLIST INFORMATION
    printf("\n         BufferList Information \n");
    if (BGAPI2_DataStream_GetNumDelivered(pDataStream, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("           DeliveredCount:          %llu\n", (unsigned long long)iNodeValue);
    if (BGAPI2_DataStream_GetNumUnderrun(pDataStream, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("           UnderrunCount:           %llu\n", (unsigned long long)iNodeValue);
    printf("\n");
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT stopStreaming(BGAPI2_Device* pDevice, BGAPI2_DataStream* pDataStream) {
    BGAPI2_NodeMap* mDeviceRemoteNodeList = NULL;
    BGAPI2_Node* pNode = NULL;
    char* pModelName = NULL;
    bo_uint64  iStringlength = 0;
    bo_bool bNodePresent = 0;
    printf("\nCAMERA STOP\n");
    printf("###########\n\n");
    // DEVICE ACQUISITION ABORT
    if (BGAPI2_Device_GetModel(pDevice, pModelName, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    pModelName = malloc((int)iStringlength);
    if (BGAPI2_Device_GetModel(pDevice, pModelName, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Device_GetRemoteNodeList(pDevice, &mDeviceRemoteNodeList) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_NodeMap_GetNodePresent(mDeviceRemoteNodeList, "AcquisitionAbort", &bNodePresent) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (bNodePresent == 1) {
        if (BGAPI2_Device_GetRemoteNode(pDevice, "AcquisitionAbort", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_Execute(pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("5.1.12   %s aborted \n", pModelName);
    }
    if (BGAPI2_Device_GetRemoteNode(pDevice, "AcquisitionStop", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_Execute(pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("5.1.12   %s stopped \n", pModelName);
    free(pModelName);
    if (getStreamRelatedInformations(pDevice, pDataStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_DataStream_StopAcquisition(pDataStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("5.1.12   DataStream stopped \n");
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT removeBuffers(BGAPI2_DataStream* pDataStream) {
    bo_uint64 iNumAnnounced = 0;
    bo_uint64 iBufferIndex = 0;
    if (BGAPI2_DataStream_DiscardAllBuffers(pDataStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_DataStream_GetNumAnnounced(pDataStream, &iNumAnnounced) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("\nRELEASE\n");
    printf("#######\n\n");
    // REVOKE ALL BUFFERS
    printf("5.1.13   Releasing the resources \n");
    for (iBufferIndex = 0; iBufferIndex < iNumAnnounced; iBufferIndex++) {
        BGAPI2_Buffer* pBuffer = NULL;
        if (BGAPI2_DataStream_GetBufferID(pDataStream, 0, &pBuffer) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_DataStream_RevokeBuffer(pDataStream, pBuffer, NULL) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_DeleteBuffer(pBuffer, NULL) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    }
    printf("\n");
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT run() {
    bo_uint numSystems = 0;
    BGAPI2_System* pSystem = NULL;
    BGAPI2_Interface* pInterface = NULL;
    bo_uint numDevices = 0;
    BGAPI2_Device* pDevice = NULL;
    bo_uint numDataStreams = 0;
    BGAPI2_DataStream* pDataStream = NULL;
    BGAPI2_RESULT res = BGAPI2_RESULT_SUCCESS;
    printf("#############################################################\n");
    printf("# PROGRAMMER'S GUIDE Example 001_ImageCaptureMode_Polling.c #\n");
    printf("#############################################################\n");
    printf("\n\n");
    res = updateSystems(&numSystems);
    if (res == BGAPI2_RESULT_SUCCESS) {
        res = getFirstInterfaceWithDevice(numSystems, &pInterface);
    }
    if (!pInterface) {
        printf("  No camera found \n");
    } else {
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = updateDevices(pInterface, &numDevices);
        }
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = getFirstDevice(pInterface, &pDevice);
        }
    }
    if (pDevice) {
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = setDeviceParameter(pDevice);
        }
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = getDataStreamCount(pDevice, &numDataStreams);
        }
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = getFirstDataStream(pDevice, &pDataStream);
        }
    }
    if (pDataStream) {
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = addBuffers(pDataStream);
        }
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = startStreaming(pDevice, pDataStream);
        }
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = captureFrames(pDataStream);
        }
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = stopStreaming(pDevice, pDataStream);
        }
        // always release resources
        if (removeBuffers(pDataStream) != BGAPI2_RESULT_SUCCESS) {
            res = BGAPI2_RESULT_ERROR;
        }
        BGAPI2_DataStream_Close(pDataStream);
    }

    if (pDevice) {
        BGAPI2_Device_Close(pDevice);
    }
    if (pInterface) {
        BGAPI2_Interface_Close(pInterface);
        if (BGAPI2_Interface_GetParent(pInterface, &pSystem) == BGAPI2_RESULT_SUCCESS) {
            BGAPI2_System_Close(pSystem);
            BGAPI2_ReleaseSystem(pSystem);
        }
    }
    return res;
}

int main() {
    int key = 0;  // keyboard input number
    int result = 0;
    if (run()) {
        printLastError();
        result = 1;
    }
    printf("End\nInput any number to close the program:");
    if (scanf("%d", &key)) {}  // use 'if' to avoid compiler warning of unused return value
    return result;
}
