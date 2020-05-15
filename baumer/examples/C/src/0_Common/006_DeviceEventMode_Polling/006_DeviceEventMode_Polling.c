/*
This example describes the FIRST STEPS of handling Baumer-GAPI SDK.
The given source code applies to handling one system, one camera and ten images.
Please see "Baumer-GAPI SDK Programmer's Guide" chapter 5.6
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bgapi2_genicam/bgapi2_genicam.h"

#define BUFFERCOUNT 4

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
                BGAPI2_Node* pNode = NULL;
                if (BGAPI2_Interface_GetDevice(pInterface, 0, &pDevice) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                if (BGAPI2_Device_Open(pDevice) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                if (printDeviceInfo(pDevice) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                if (BGAPI2_Device_GetRemoteNode(pDevice, "EventSelector", &pNode) == BGAPI2_RESULT_SUCCESS) {
                    *ppDevice = pDevice;
                    break;
                } else {
                    printf("Device did not support Events!\n");
                    if (BGAPI2_Device_Close(pDevice) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                }
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
    BGAPI2_Node* pNode = NULL;
    BGAPI2_RESULT res = BGAPI2_RESULT_SUCCESS;
    // switch 'TriggerMode' off
    if (BGAPI2_Device_GetRemoteNode(pDevice, "TriggerMode", &pNode) == BGAPI2_RESULT_SUCCESS) {
        BGAPI2_Node_SetString(pNode, "Off");
    }
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

BGAPI2_RESULT addBuffers(BGAPI2_DataStream* pDataStream) {
    bo_uint iIndexBuffers = 0;
    BGAPI2_Buffer* pBuffer = NULL;
    for (iIndexBuffers = 0; iIndexBuffers < BUFFERCOUNT; iIndexBuffers++) {
        if (BGAPI2_CreateBuffer(&pBuffer) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_DataStream_AnnounceBuffer(pDataStream, pBuffer) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_DataStream_QueueBuffer(pDataStream, pBuffer) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    }
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT removeBuffers(BGAPI2_DataStream* pDataStream) {
    bo_uint64 iNumAnnounced = 0;
    bo_uint64 iBufferIndex = 0;
    if (BGAPI2_DataStream_DiscardAllBuffers(pDataStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_DataStream_GetNumAnnounced(pDataStream, &iNumAnnounced) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    for (iBufferIndex = 0; iBufferIndex < iNumAnnounced; iBufferIndex++) {
        BGAPI2_Buffer* pBuffer = NULL;
        if (BGAPI2_DataStream_GetBufferID(pDataStream, 0, &pBuffer) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_DataStream_RevokeBuffer(pDataStream, pBuffer, NULL) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_DeleteBuffer(pBuffer, NULL) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    }
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT startStreaming(BGAPI2_Device* pDevice, BGAPI2_DataStream** ppStream) {
    BGAPI2_Node* pNode = NULL;

    if (BGAPI2_Device_GetDataStream(pDevice, 0, ppStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_DataStream_Open(*ppStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (addBuffers(*ppStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_DataStream_StartAcquisitionContinuous(*ppStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Device_GetRemoteNode(pDevice, "AcquisitionStart", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_Execute(pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT stopStreaming(BGAPI2_Device* pDevice, BGAPI2_DataStream* pStream) {
    BGAPI2_Node* pNode = NULL;
    if (BGAPI2_Device_GetRemoteNode(pDevice, "AcquisitionAbort", &pNode) == BGAPI2_RESULT_SUCCESS) {
        if (BGAPI2_Node_Execute(pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    }
    if (BGAPI2_Device_GetRemoteNode(pDevice, "AcquisitionStop", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_Execute(pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_DataStream_StopAcquisition(pStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (removeBuffers(pStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_DataStream_Close(pStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT captureFrames(BGAPI2_Device* pDevice, BGAPI2_DataStream* pStream, BGAPI2_DeviceEvent* pEvent, int iEventCount) {
    int framecount = 0;
    int iEventIndex = 0;
    BGAPI2_Node* pNode = NULL;
    char myString[1024] = "";
    bo_uint64  iStringlength = sizeof(myString);
    bo_uint64 iTimestamp = 0;
    BGAPI2_RESULT res;

    printf("\nCAPTURE 10 IMAGES BY SOFTWARE TRIGGER\n");
    printf("######################################\n\n");
    BGAPI2_Device_GetRemoteNode(pDevice, "TriggerSoftware", &pNode);
    if (BGAPI2_Node_GetInterface(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;

    for (framecount = 0; framecount < 10; framecount++) {
        BGAPI2_Buffer* pBufferFilled = NULL;
        bo_bool bIsIncomplete;
        //EXAMPLE DEVICE REMOTE NODE INTERFACE TYPE -> "ICommand"
        //=======================================================
        //SOFTWARE TRIGGER
        printf("Execute TriggerSoftware %d is interface type %s\n", framecount + 1, myString);
        BGAPI2_Node_Execute(pNode);

        for (iEventIndex = 0; iEventIndex < iEventCount; iEventIndex++) {
            res = BGAPI2_Device_GetDeviceEvent(pDevice, pEvent, 1000);
            if (res != BGAPI2_RESULT_SUCCESS) {
                printf("BGAPI2_GetCurrentEvent errorcode: %d\n", res);
            } else {
                printf("    Event received. \n");
                iStringlength = sizeof(myString);
                if (BGAPI2_DeviceEvent_GetID(pEvent, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf("    Event.Id:            %s \n", myString);
                iStringlength = sizeof(myString);
                if (BGAPI2_DeviceEvent_GetName(pEvent, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf("    Event.Name:          %s \n", myString);
                if (BGAPI2_DeviceEvent_GetTimeStamp(pEvent, &iTimestamp) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf("    Event.Timestamp:     %llu \n", (unsigned long long)iTimestamp);
            }
        }

        res = BGAPI2_DataStream_GetFilledBuffer(pStream, &pBufferFilled, 1000);  // timeout 1000 msec
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
            if (BGAPI2_DataStream_QueueBuffer(pStream, pBufferFilled) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        }
    }  // Image Capture Loop end
    printf(" \n");
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT activateDeviceEvent(BGAPI2_Device* pDevice, const char* pName) {
    BGAPI2_Node* pNode = NULL;
    char myString[1024] = "";
    bo_uint64  iStringlength = sizeof(myString);
    if (BGAPI2_Device_GetRemoteNode(pDevice, "EventSelector", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_SetString(pNode, pName) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetValue(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("        EventSelector:            %s\n", myString);
    if (BGAPI2_Device_GetRemoteNode(pDevice, "EventNotification", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    // standard: MXG, VisiLine,...
    if (BGAPI2_Node_SetString(pNode, "On") != BGAPI2_RESULT_SUCCESS) {
        // TXG, SXG
        if (BGAPI2_Node_SetString(pNode, "GigEVisionEvent") != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    }
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetValue(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("        EventNotification:        %s\n", myString);
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT setDeviceParameter(BGAPI2_Device* pDevice, int* pEventsRegistered) {
    BGAPI2_Node* pNode = NULL;
    BGAPI2_Node* pSelectorNode = NULL;
    BGAPI2_NodeMap* pNodeList = NULL;
    char myString[1024] = "";
    bo_uint64  iStringlength = sizeof(myString);
    bo_uint64 iNodeCount = 0;
    bo_uint64 iNodeIndex = 0;


    printf("DEVICE PARAMETER SETUP\n");
    printf("######################\n\n");
    if (BGAPI2_Device_GetRemoteNode(pDevice, "TriggerSource", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_SetString(pNode, "Software") != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Device_GetRemoteNode(pDevice, "TriggerMode", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_SetString(pNode, "On") != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetValue(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          TriggerMode:          %s\n", myString);

    *pEventsRegistered = 0;
    printf("5.6.1   Device Events\n\n");
    if (activateDeviceEvent(pDevice, "ExposureStart") == BGAPI2_RESULT_SUCCESS) {
        *pEventsRegistered += 1;
    }
    if (activateDeviceEvent(pDevice, "ExposureEnd") == BGAPI2_RESULT_SUCCESS) {
        *pEventsRegistered += 1;
    }
    if (BGAPI2_Device_GetRemoteNode(pDevice, "EventSelector", &pSelectorNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetEnumNodeList(pSelectorNode, &pNodeList) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_NodeMap_GetNodeCount(pNodeList, &iNodeCount) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("        EventSelector list count: %lld\n", (long long)iNodeCount);
    for (iNodeIndex = 0; iNodeIndex < iNodeCount; iNodeIndex++) {
        bo_bool bReadable = 0;
        BGAPI2_NodeMap_GetNodeByIndex(pNodeList, iNodeIndex, &pNode);
        BGAPI2_Node_IsReadable(pNode, &bReadable);
        if (bReadable) {
            iStringlength = sizeof(myString);
            if (BGAPI2_Node_GetValue(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf("                             %2lld:  %-28s ", (long long)iNodeIndex, myString);
            if (BGAPI2_Node_SetString(pSelectorNode, myString) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_Device_GetRemoteNode(pDevice, "EventNotification", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            iStringlength = sizeof(myString);
            if (BGAPI2_Node_GetValue(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf("%s\n", myString);
        }
    }
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT enableDeviceEvents(BGAPI2_Device* pDevice, BGAPI2_DeviceEvent** ppEvent) {
    if (BGAPI2_Device_SetDeviceEventMode(pDevice, EVENTMODE_POLLING) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_CreateDeviceEvent(ppEvent) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT disableDeviceEvents(BGAPI2_Device* pDevice, BGAPI2_DeviceEvent* pEvent) {
    if (BGAPI2_ReleaseDeviceEvent(pEvent) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Device_SetDeviceEventMode(pDevice, EVENTMODE_UNREGISTERED) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT run() {
    BGAPI2_Device* pDevice = NULL;
    BGAPI2_DataStream* pStream = NULL;
    BGAPI2_DeviceEvent* pEvent = NULL;
    int iEventCount = 0;
    BGAPI2_RESULT res = BGAPI2_RESULT_SUCCESS;
    if (getFirstDevice(&pDevice) == BGAPI2_RESULT_SUCCESS) {
        res = enableDeviceEvents(pDevice, &pEvent);
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = setDeviceParameter(pDevice, &iEventCount);
        }
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = startStreaming(pDevice, &pStream);
        }
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = captureFrames(pDevice, pStream, pEvent, iEventCount);
        }
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = stopStreaming(pDevice, pStream);
        }
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = disableDeviceEvents(pDevice, pEvent);
        }
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
