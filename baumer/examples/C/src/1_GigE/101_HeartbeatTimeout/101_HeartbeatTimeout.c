/*
This example describes the FIRST STEPS of handling Baumer-GAPI SDK.
The given source code applies to handling one system, one camera and twelfe images.
Please see "Baumer-GAPI SDK Programmer's Guide" chapter 5.1 and chapter 5.2
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "bgapi2_genicam/bgapi2_genicam.h"

BGAPI2_RESULT printDeviceInfo(BGAPI2_Device* pDevice) {
    char sModel[1024] = "";
    bo_uint64  iModelLength = sizeof(sModel);
    char sSerial[1024] = "";
    bo_uint64  iSerialLength = sizeof(sSerial);
    if (BGAPI2_Device_GetModel(pDevice, sModel, &iModelLength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Device_GetSerialNumber(pDevice, sSerial, &iSerialLength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("%s(%s)\n", sModel, sSerial);
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT getFirstDevice(BGAPI2_Device** ppDevice) {
    bo_uint iNumTLProducer = 0;
    bo_uint iIndexTLProducer = 0; // Gige = 0, USB3 = 1
    if (BGAPI2_UpdateSystemList() != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_GetNumSystems(&iNumTLProducer) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    for (iIndexTLProducer = 0; iIndexTLProducer < iNumTLProducer; iIndexTLProducer++) {
        BGAPI2_System * pSystem = NULL;
        bo_bool bChanged = 0;
        bo_uint iNumInterfaces = 0;
        bo_uint iIndexInterface = 0;
        if (BGAPI2_GetSystem(iIndexTLProducer, &pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_System_Open(pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_System_UpdateInterfaceList(pSystem, &bChanged, 100) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_System_GetNumInterfaces(pSystem, &iNumInterfaces) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        for (iIndexInterface = 0; iIndexInterface < iNumInterfaces; iIndexInterface++) {
            BGAPI2_Interface* pInterface = NULL;
            bo_uint iNumDevices = 0;
            if (BGAPI2_System_GetInterface(pSystem, iIndexInterface, &pInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Interface_Open(pInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Interface_UpdateDeviceList(pInterface, &bChanged, 200) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Interface_GetNumDevices(pInterface, &iNumDevices) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (iNumDevices > 0) {
                BGAPI2_Device* pDevice = NULL;
                if (BGAPI2_Interface_GetDevice(pInterface, 0, &pDevice) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                if (BGAPI2_Device_Open(pDevice) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                if (printDeviceInfo(pDevice) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                *ppDevice = pDevice;
                break;
            }
            if (BGAPI2_Interface_Close(pInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        }
        if (*ppDevice) break;
        if (BGAPI2_System_Close(pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_ReleaseSystem(pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    }
    return (*ppDevice) ? BGAPI2_RESULT_SUCCESS : BGAPI2_RESULT_ERROR;
}

BGAPI2_RESULT releaseResources(BGAPI2_Device* pDevice) {
    BGAPI2_System* pSystem = NULL;
    BGAPI2_Interface* pInterface = NULL;
    BGAPI2_RESULT res = BGAPI2_RESULT_SUCCESS;
    BGAPI2_Device_Close(pDevice);
    res = BGAPI2_Device_GetParent(pDevice, &pInterface);
    if (res == BGAPI2_RESULT_SUCCESS) {
        BGAPI2_Interface_Close(pInterface);
        res = BGAPI2_Interface_GetParent(pInterface, &pSystem);
    }
    if (res == BGAPI2_RESULT_SUCCESS) {
        BGAPI2_System_Close(pSystem);
        res = BGAPI2_ReleaseSystem(pSystem);
    }
    return res;
}

BGAPI2_RESULT setDeviceParameter(BGAPI2_Device* pDevice) {
    BGAPI2_Node* pNode = NULL;

    printf("DEVICE PARAMETER SETUP\n");
    printf("######################\n\n");
    if (BGAPI2_Device_GetRemoteNode(pDevice, "DeviceLinkHeartbeatTimeout", &pNode) == BGAPI2_RESULT_SUCCESS) {
        char pString[1024] = "micro seconds";
        bo_uint64  iStringlength = sizeof(pString);
        bo_double fNodeValue = 0;
        printf("Set HeartbeatTimeout to longer time from 3 seconds to 1 minute during debugging\n");
        printf("to prevent the camera from auto-disconnection in breakpoints\n\n");
        // 1 minute = 60,000,000 usec
        if (BGAPI2_Node_SetDouble(pNode, 60000000.0) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetDouble(pNode, &fNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("    DeviceLinkHeartbeatTimeout: %0.f", fNodeValue);
        BGAPI2_Node_GetUnit(pNode, pString, &iStringlength);
        printf(" [%s]\n", pString);
    } else if (BGAPI2_Device_GetRemoteNode(pDevice, "DeviceLinkHeartbeatTimeout", &pNode) == BGAPI2_RESULT_SUCCESS) {
        char pString[1024] = "milli seconds";
        bo_uint64  iStringlength = sizeof(pString);
        bo_double fNodeValue = 0;
        printf("Set HeartbeatTimeout to longer time from 3 seconds to 1 minute during debugging\n");
        printf("to prevent the camera from auto-disconnection in breakpoints\n\n");
        // 1 minute = 60000 msec
        if (BGAPI2_Node_SetDouble(pNode, 60000000.0) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetDouble(pNode, &fNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("    GevHeartbeatTimeout: %0.f", fNodeValue);
        BGAPI2_Node_GetUnit(pNode, pString, &iStringlength);
        printf(" [%s]\n", pString);
    } else {
        printf("Device does not support HeartbeatTimeout\n");
    }
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT run() {
    BGAPI2_Device* pDevice = NULL;
    BGAPI2_DataStream* pStream = NULL;
    BGAPI2_DeviceEvent* pEvent = NULL;
    BGAPI2_RESULT res = BGAPI2_RESULT_SUCCESS;
    if (getFirstDevice(&pDevice) == BGAPI2_RESULT_SUCCESS) {
        res = setDeviceParameter(pDevice);
        // always release resources
        if (releaseResources(pDevice) != BGAPI2_RESULT_SUCCESS) {
            res = BGAPI2_RESULT_ERROR;
        }
    }
    return res;
}

int main() {
    int key = 0; // keyboard input number
    int result = 0;
    if (run() != BGAPI2_RESULT_SUCCESS) {
        BGAPI2_RESULT res;
        char pString[1024] = "";         // defines an char array
        bo_uint64  iStringlength = sizeof(pString);
        BGAPI2_GetLastError(&res, pString, &iStringlength);
        printf("\nError: %s", pString);
        result = 1;
    }
    printf("\nInput any number to close the program:");
    if(scanf("%d", &key)) {}  // use 'if' to avoid compiler warning of unused return value
    return result;
}
