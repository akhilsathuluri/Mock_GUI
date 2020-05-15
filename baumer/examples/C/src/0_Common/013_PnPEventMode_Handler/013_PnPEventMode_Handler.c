#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "bgapi2_genicam/bgapi2_genicam.h"

BGAPI2_RESULT printDeviceTree(bo_uint iIndex) {
    bo_uint iNumInterfaces = 0;
    bo_uint iIndexInterface = 0;
    char pString[1024] = "";
    bo_uint64 iStringlength = sizeof(pString);
    BGAPI2_System * pSystem = NULL;

    if (BGAPI2_GetSystem(iIndex, &pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_System_GetDisplayName(pSystem, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("%s\n", pString);
    if (BGAPI2_System_GetNumInterfaces(pSystem, &iNumInterfaces) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    for (iIndexInterface = 0; iIndexInterface < iNumInterfaces; iIndexInterface++) {
        BGAPI2_Interface* pInterface = NULL;
        bo_uint iNumDevices = 0;
        bo_uint iIndexDevice = 0;
        bo_bool bChanged = 0;
        BGAPI2_Node* pNode = NULL;
        if (BGAPI2_System_GetInterface(pSystem, iIndexInterface, &pInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        iStringlength = sizeof(pString);
        if (BGAPI2_Interface_GetDisplayName(pInterface, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf(" |\n +-- %s", pString);
        if (BGAPI2_Interface_UpdateDeviceList(pInterface, &bChanged, 200) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Interface_GetNumDevices(pInterface, &iNumDevices) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("(devices: %d)", iNumDevices);
        if (BGAPI2_Interface_GetNode(pInterface, "GevInterfaceSubnetIPAddress", &pNode) == BGAPI2_RESULT_SUCCESS) {
            iStringlength = sizeof(pString);
            if (BGAPI2_Node_GetValue(pNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" IP %s", pString);
        }
        if (BGAPI2_Interface_GetNode(pInterface, "GevInterfaceSubnetMask", &pNode) == BGAPI2_RESULT_SUCCESS) {
            iStringlength = sizeof(pString);
            if (BGAPI2_Node_GetValue(pNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Mask %s", pString);
        }
        printf("\n");
        for (iIndexDevice = 0; iIndexDevice < iNumDevices; iIndexDevice++) {
            BGAPI2_Device* pDevice = NULL;
            if (BGAPI2_Interface_GetDevice(pInterface, iIndexDevice, &pDevice) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (iIndexInterface < iNumInterfaces - 1) {
                printf(" |    |\n |    +-- ");
            } else {
                printf("      |\n      +-- ");
            }
            iStringlength = sizeof(pString);
            if (BGAPI2_Device_GetModel(pDevice, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf("%s", pString);
            iStringlength = sizeof(pString);
            if (BGAPI2_Device_GetSerialNumber(pDevice, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" (%s)", pString);
            iStringlength = sizeof(pString);
            if (BGAPI2_Device_GetAccessStatus(pDevice, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Access %s", pString);
            if (BGAPI2_Device_GetNode(pDevice, "GevDeviceIPAddress", &pNode) == BGAPI2_RESULT_SUCCESS) {
                iStringlength = sizeof(pString);
                if (BGAPI2_Node_GetValue(pNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS)
                    return BGAPI2_RESULT_ERROR;
                printf(" IP %s", pString);
            }
            if (BGAPI2_Device_GetNode(pDevice, "GevDeviceSubnetMask", &pNode) == BGAPI2_RESULT_SUCCESS) {
                iStringlength = sizeof(pString);
                if (BGAPI2_Node_GetValue(pNode, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS)
                    return BGAPI2_RESULT_ERROR;
                printf(" Mask %s", pString);
            }
            printf("\n");
        }
    }
    printf("\n");
    return BGAPI2_RESULT_SUCCESS;
}

//CALLBACK PNP EVENTS
void BGAPI2CALL PnPEventHandler(void * callbackOwner, BGAPI2_PnPEvent * pPnPEvent) {
    char pString[1024] = "";
    bo_uint64 iStringlength = sizeof(pString);
    bo_uint64 iPNPType = 0;
    bo_uint iNumTLProducer = 0;
    bo_uint iIndexTLProducer = 0;
    BGAPI2_Interface* pInterface = ((BGAPI2_Interface *)callbackOwner);

    if (BGAPI2_Interface_GetDisplayName(pInterface, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) strcpy(pString, "ERROR");
    printf("\n\n %s PnPEvent!\n", pString);
    iStringlength = sizeof(pString);
    if (BGAPI2_PnPEvent_GetID(pPnPEvent, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) strcpy(pString, "ERROR");
    printf("   EventID %s", pString);
    if (BGAPI2_PnPEvent_GetPnPType(pPnPEvent, &iPNPType) != BGAPI2_RESULT_SUCCESS) {
        strcpy(pString, "ERROR");
    } else {
        strcpy(pString, ((iPNPType == 0) ? "removed" : "added  "));
    }
    printf(" PnPType: %s", pString);
    iStringlength = sizeof(pString);
    if (BGAPI2_PnPEvent_GetSerialNumber(pPnPEvent, pString, &iStringlength) != BGAPI2_RESULT_SUCCESS) strcpy(pString, "ERROR");
    printf(" SerialNumber %s\n\n", pString);
    
    BGAPI2_GetNumSystems(&iNumTLProducer);
    for (iIndexTLProducer = 0; iIndexTLProducer < iNumTLProducer; iIndexTLProducer++) {
        printDeviceTree(iIndexTLProducer);
    }
    printf("\nInput any value to stop.\n");
    return;
}

BGAPI2_RESULT init(bo_uint iIndex) {
    BGAPI2_System * pSystem = NULL;
    bo_bool bChanged = 0;
    bo_uint iNumInterfaces = 0;
    bo_uint iIndexInterface = 0;
    size_t iIndexEvent = 0;

    if (BGAPI2_GetSystem(iIndex, &pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_System_Open(pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_System_UpdateInterfaceList(pSystem, &bChanged, 100) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_System_GetNumInterfaces(pSystem, &iNumInterfaces) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    for (iIndexInterface = 0; iIndexInterface < iNumInterfaces; iIndexInterface++) {
        BGAPI2_Interface* pInterface = NULL;
        bo_uint iNumDevices = 0;
        if (BGAPI2_System_GetInterface(pSystem, iIndexInterface, &pInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Interface_Open(pInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Interface_UpdateDeviceList(pInterface, &bChanged, 200) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Interface_RegisterPnPEventHandler(pInterface, pInterface, &PnPEventHandler) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Interface_SetPnPEventMode(pInterface, EVENTMODE_EVENT_HANDLER) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    }
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT deinit(bo_uint iIndex) {
    BGAPI2_System * pSystem = NULL;
    bo_uint iNumInterfaces = 0;
    bo_uint iIndexInterface = 0;
    size_t iIndexEvent = 0;

    if (BGAPI2_GetSystem(iIndex, &pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_System_GetNumInterfaces(pSystem, &iNumInterfaces) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    for (iIndexInterface = 0; iIndexInterface < iNumInterfaces; iIndexInterface++) {
        BGAPI2_Interface* pInterface = NULL;
        if (BGAPI2_System_GetInterface(pSystem, iIndexInterface, &pInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Interface_SetPnPEventMode(pInterface, EVENTMODE_UNREGISTERED) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Interface_Close(pInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    }
    if (BGAPI2_System_Close(pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_ReleaseSystem(pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT run() {
    bo_uint iNumTLProducer = 0;
    bo_uint iIndexTLProducer = 0;
    char key = 0;
    BGAPI2_RESULT res = BGAPI2_RESULT_SUCCESS;

    res = BGAPI2_UpdateSystemList();
    if (res == BGAPI2_RESULT_SUCCESS) {
        res = BGAPI2_GetNumSystems(&iNumTLProducer);
    }

    for (iIndexTLProducer = 0; iIndexTLProducer < iNumTLProducer; iIndexTLProducer++) {
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = init(iIndexTLProducer);
        }
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = printDeviceTree(iIndexTLProducer);
        }
    }
    if (res == BGAPI2_RESULT_SUCCESS) {
        printf("\nInput any value to stop.\n");
        if (scanf(" %c", &key)) {}  // use 'if' to avoid compiler warning of unused return value
    }
    for (iIndexTLProducer = 0; iIndexTLProducer < iNumTLProducer; iIndexTLProducer++) {
        if (deinit(iIndexTLProducer) != BGAPI2_RESULT_SUCCESS) {
            res = BGAPI2_RESULT_ERROR;
        }
    }

    return res;
}

int main(int argc, char* argv[]) {
    int key = 0; // keyboard input number
    int result = 0;

    if (run() != BGAPI2_RESULT_SUCCESS) {
        BGAPI2_RESULT res;
        char myString[1024] = "";         // defines an char array
        bo_uint64  iStringlength = sizeof(myString);
        BGAPI2_GetLastError(&res, myString, &iStringlength);
        printf("\nError: %s", myString);
        result = 1;
    }
    printf("\nInput any number to close the program:");
    if (scanf("%d", &key)) {}  // use 'if' to avoid compiler warning of unused return value
    return result;
}
