#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bgapi2_genicam/bgapi2_genicam.h"

#define BUFFERCOUNT 4

BGAPI2_RESULT run(BGAPI2_System * pSystem, bo_bool* pCamfound) {
    bo_bool bChanged = 0;
    bo_uint iNumInterfaces = 0;
    bo_uint iIndexInterface = 0;
    if (BGAPI2_System_Open(pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_System_UpdateInterfaceList(pSystem, &bChanged, 100) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_System_GetNumInterfaces(pSystem, &iNumInterfaces) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    for (iIndexInterface = 0; iIndexInterface < iNumInterfaces && !*pCamfound; iIndexInterface++) {
        BGAPI2_Interface* pInterface = NULL;
        bo_uint iNumDevices = 0;
        if (BGAPI2_System_GetInterface(pSystem, iIndexInterface, &pInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Interface_Open(pInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Interface_UpdateDeviceList(pInterface, &bChanged, 200) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Interface_GetNumDevices(pInterface, &iNumDevices) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (iNumDevices > 0) {
            BGAPI2_Device* pDevice = NULL;
            char sModel[1024] = "";
            bo_uint64  iModelLength = sizeof(sModel);
            char sSerial[1024] = "";
            bo_uint64  iSerialLength = sizeof(sSerial);
            bo_bool bNodePresent = 0;
            BGAPI2_Node* pNode = NULL;
            BGAPI2_NodeMap* pNodeList = NULL;
            bo_uint iFramecount = 0;
            BGAPI2_Buffer * pOutBuffer[BUFFERCOUNT];
            bo_uint iNumDataStreams = 0;
            int iIndex = 0;
            BGAPI2_DataStream* pDataStream = NULL;
            if (BGAPI2_Interface_GetDevice(pInterface, 0, &pDevice) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Device_Open(pDevice) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Device_GetModel(pDevice, sModel, &iModelLength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Device_GetSerialNumber(pDevice, sSerial, &iSerialLength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf("%s(%s)\n", sModel, sSerial);
            if (BGAPI2_Device_GetRemoteNode(pDevice, "TriggerMode", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Node_SetString(pNode, "Off") != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Device_GetRemoteNodeList(pDevice, &pNodeList) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_NodeMap_GetNodePresent(pNodeList, "ExposureTime", &bNodePresent) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (bNodePresent) {
                if (BGAPI2_Device_GetRemoteNode(pDevice, "ExposureTime", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            } else {
                if (BGAPI2_Device_GetRemoteNode(pDevice, "ExposureTimeAbs", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            }
            if (BGAPI2_Node_SetDouble(pNode, 10000.0) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Device_GetNumDataStreams(pDevice, &iNumDataStreams) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Device_GetDataStream(pDevice, 0, &pDataStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_DataStream_Open(pDataStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            for (iIndex = 0; iIndex < BUFFERCOUNT; iIndex++) {
                if (BGAPI2_CreateBuffer(&pOutBuffer[iIndex]) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                if (BGAPI2_DataStream_AnnounceBuffer(pDataStream, pOutBuffer[iIndex]) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                if (BGAPI2_DataStream_QueueBuffer(pDataStream, pOutBuffer[iIndex]) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            }
            if (BGAPI2_DataStream_StartAcquisitionContinuous(pDataStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Device_GetRemoteNode(pDevice, "AcquisitionStart", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Node_Execute(pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            for (iFramecount = 0; iFramecount < 12; iFramecount++) {
                BGAPI2_Buffer* pBufferFilled = NULL;
                BGAPI2_RESULT res = BGAPI2_DataStream_GetFilledBuffer(pDataStream, &pBufferFilled, 1000);
                if (res != BGAPI2_RESULT_SUCCESS) {
                    if (res == BGAPI2_RESULT_TIMEOUT) {
                        printf("Error: Buffer Timeout after 1000 msec\n");
                    } else {
                        printf("BGAPI2_DataStream_GetFilledBuffer errorcode: %d\n", res);
                    }
                } else {
                    bo_bool bIsIncomplete = 0;
                    if (BGAPI2_Buffer_GetIsIncomplete(pBufferFilled, &bIsIncomplete) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                    if (bIsIncomplete == 1) {
                        printf("Error: Image is incomplete\n");
                    } else {
                        bo_uint64 frameId = 0;
                        if (BGAPI2_Buffer_GetFrameID(pBufferFilled, &frameId) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                        printf("Image %lld received.\n", (long long)frameId);
                    }
                    if (BGAPI2_DataStream_QueueBuffer(pDataStream, pBufferFilled) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                }
            } // Image Capture Loop end
            if (BGAPI2_NodeMap_GetNodePresent(pNodeList, "AcquisitionAbort", &bNodePresent) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (bNodePresent == 1) {
                if (BGAPI2_Device_GetRemoteNode(pDevice, "AcquisitionAbort", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                if (BGAPI2_Node_Execute(pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            }
            if (BGAPI2_Device_GetRemoteNode(pDevice, "AcquisitionStop", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Node_Execute(pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_DataStream_StopAcquisition(pDataStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_DataStream_DiscardAllBuffers(pDataStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            for (iIndex = 0; iIndex < BUFFERCOUNT; iIndex++) {
                if (BGAPI2_DataStream_RevokeBuffer(pDataStream, pOutBuffer[iIndex], NULL) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                if (BGAPI2_DeleteBuffer(pOutBuffer[iIndex], NULL) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            }
            if (BGAPI2_DataStream_Close(pDataStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Device_Close(pDevice) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            *pCamfound = 1;
        }
        if (BGAPI2_Interface_Close(pInterface) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    }
    if (BGAPI2_System_Close(pSystem) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    return BGAPI2_RESULT_SUCCESS;
}

int main() {
    int key = 0; // keyboard input number
    int result = 0;
    bo_bool camfound = 0;
    bo_uint iNumTLProducer = 0;
    bo_uint iIndexTLProducer = 0; // Gige = 0, USB3 = 1
    BGAPI2_RESULT res = BGAPI2_RESULT_SUCCESS;
    res = BGAPI2_UpdateSystemList();
    if (res == BGAPI2_RESULT_SUCCESS) {
        res = BGAPI2_GetNumSystems(&iNumTLProducer);
    }
    for (iIndexTLProducer = 0; iIndexTLProducer < iNumTLProducer && !camfound; iIndexTLProducer++) {
        BGAPI2_System * pSystem = NULL;
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = BGAPI2_GetSystem(iIndexTLProducer, &pSystem);
        }
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = run(pSystem, &camfound);
        }
        if (pSystem && BGAPI2_ReleaseSystem(pSystem) != BGAPI2_RESULT_SUCCESS) {
            res = BGAPI2_RESULT_ERROR;
        }
    }
    if (!camfound) {
        printf("no camera found on any system and interface.\n");
    }
    if (res != BGAPI2_RESULT_SUCCESS) {
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
