/*
This example describes the FIRST STEPS of handling Baumer-GAPI SDK.
The given source code applies to handling one system, one camera and twenty images.
Please see "Baumer-GAPI SDK Programmer's Guide" chapter 5.4
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

BGAPI2_RESULT setIntValue(BGAPI2_Node* pNode, bo_int64 iValue) {
    bo_int64 iMin = 0;
    bo_int64 iMax = 0;
    bo_int64 iInc = 0;
    bo_int64 iCurrent = 0;
    bo_int64 iRemainder = 0;

    if (BGAPI2_Node_GetInt(pNode, &iCurrent) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetIntMin(pNode, &iMin) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetIntMax(pNode, &iMax) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetIntInc(pNode, &iInc) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    // check new value is within range
    if (iValue < iMin)
        iValue = iMin;
    if (iValue > iMax)
        iValue = iMax;
    iRemainder = (iValue - iMin) % iInc;
    if (iRemainder) {
        if (iValue < iCurrent) {
            iValue = iValue - iRemainder;
        } else {
            iValue = iValue - iRemainder + iInc;
        }
    }

    return BGAPI2_Node_SetInt(pNode, iValue);
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

BGAPI2_RESULT resetDeviceParameter(BGAPI2_Device* pDevice) {
    BGAPI2_Node* pNode = NULL;
    bo_int64 iNodeValue = 0;
    printf("5.3.1    Reset ROI parameters \n");
    if (BGAPI2_Device_GetRemoteNode(pDevice, "OffsetY", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetIntMin(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (setIntValue(pNode, iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Device_GetRemoteNode(pDevice, "Height", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetIntMax(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (setIntValue(pNode, iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Device_GetRemoteNode(pDevice, "OffsetX", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetIntMin(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (setIntValue(pNode, iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Device_GetRemoteNode(pDevice, "Width", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetIntMax(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (setIntValue(pNode, iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;

    return BGAPI2_RESULT_SUCCESS;
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

BGAPI2_RESULT addBuffers(BGAPI2_DataStream* pDataStream) {
    bo_uint iIndexBuffers = 0;
    BGAPI2_Buffer* pBuffer = NULL;
    bo_uint64 uPayloadSize = 0;
    bo_bool bDataStreamDefinesPayloadSize = 0;
    bo_uint64 iNumAnnounced = 0;
    bo_uint64 iBufferSize = 0;

    printf("5.4.2   External buffers \n");
    if (BGAPI2_DataStream_GetDefinesPayloadSize(pDataStream, &bDataStreamDefinesPayloadSize) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (bDataStreamDefinesPayloadSize == 1) {
        if (BGAPI2_DataStream_GetPayloadSize(pDataStream, &uPayloadSize) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    } else {
        bo_int64 iNodeValue = 0;
        BGAPI2_Device* pDevice = NULL;
        BGAPI2_Node* pNode = NULL;

        if (BGAPI2_DataStream_GetParent(pDataStream, &pDevice) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Device_GetRemoteNode(pDevice, "PayloadSize", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        uPayloadSize = (bo_uint64)iNodeValue;
    }
    printf("         payloadsize required for external buffer [bytes]: %llu \n", (unsigned long long)uPayloadSize);

    for (iIndexBuffers = 0; iIndexBuffers < BUFFERCOUNT; iIndexBuffers++) {
        void* pMemory = malloc((unsigned int)uPayloadSize);
        if (BGAPI2_CreateBufferWithExternalMemory(&pBuffer, pMemory, uPayloadSize, pMemory) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_DataStream_AnnounceBuffer(pDataStream, pBuffer) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_DataStream_QueueBuffer(pDataStream, pBuffer) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    }

    //DATASTREAM ANNOUNCED COUNT
    if (BGAPI2_DataStream_GetNumAnnounced(pDataStream, &iNumAnnounced) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("5.1.10   Announced buffers:       %llu", (unsigned long long)iNumAnnounced);
    if (BGAPI2_Buffer_GetMemSize(pBuffer, &iBufferSize) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf(" using %llu [bytes]\n", (unsigned long long)iBufferSize*iNumAnnounced);

    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT removeBuffers(BGAPI2_DataStream* pDataStream) {
    bo_uint64 iNumAnnounced = 0;
    bo_uint64 iBufferIndex = 0;
    if (BGAPI2_DataStream_DiscardAllBuffers(pDataStream) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_DataStream_GetNumAnnounced(pDataStream, &iNumAnnounced) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    for (iBufferIndex = 0; iBufferIndex < iNumAnnounced; iBufferIndex++) {
        BGAPI2_Buffer* pBuffer = NULL;
        void* pMemory = NULL;
        if (BGAPI2_DataStream_GetBufferID(pDataStream, 0, &pBuffer) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_DataStream_RevokeBuffer(pDataStream, pBuffer, NULL) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_DeleteBuffer(pBuffer, &pMemory) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        free(pMemory);
    }
    printf("5.4.2    memory blocks of external buffers deleted\n");
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

BGAPI2_RESULT captureFrames(BGAPI2_DataStream* pStream) {
    int framecount = 0;

    printf("\nCAPTURE 20 IMAGES POLLING USING EXTERNAL BUFFERS\n");
    printf("#################################################\n\n");

    for (framecount = 0; framecount < 20; framecount++) {
        BGAPI2_Buffer* pBufferFilled = NULL;
        bo_bool bIsIncomplete;
        BGAPI2_RESULT res = BGAPI2_DataStream_GetFilledBuffer(pStream, &pBufferFilled, 1000);  // timeout 1000 msec
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
                bo_uint64 iBufferSize = 0;
                if (BGAPI2_Buffer_GetFrameID(pBufferFilled, &iFrameId) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf("    Image %lld received in buffer address ", (long long)iFrameId);
                if (BGAPI2_Buffer_GetMemPtr(pBufferFilled, &pBufferAddress) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf("%p ", pBufferAddress);
                if (BGAPI2_Buffer_GetMemSize(pBufferFilled, &iBufferSize) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf(" size %llu [bytes]\n", (unsigned long long)iBufferSize);
            }
            //QUEUE BUFFER
            if (BGAPI2_DataStream_QueueBuffer(pStream, pBufferFilled) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        }
    }  // Image Capture Loop end
    printf(" \n");
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT setDeviceParameter(BGAPI2_Device* pDevice) {
    BGAPI2_Node* pNode = NULL;
    BGAPI2_Node* pSensorNode = NULL;
    bo_int64 iNodeValue = 0;
    bo_int64 iSensorValue = 0;

    printf("DEVICE PARAMETER SETUP\n");
    printf("######################\n\n");

    if (BGAPI2_Device_GetRemoteNode(pDevice, "TriggerMode", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_SetString(pNode, "Off") != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    
    printf("    Set ROI parameters to the right bottom quarter of the sensor \n");

    //IMAGE WIDTH
    //===========
    if (BGAPI2_Device_GetRemoteNode(pDevice, "Width", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("        Width \n");
    if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          current value:          %lld \n", (long long)iNodeValue);
    //get sensor width
    if (BGAPI2_Device_GetRemoteNode(pDevice, "SensorWidth", &pSensorNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetInt(pSensorNode, &iSensorValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    //set new width value same to the half value of sensor width
    if (setIntValue(pNode, iSensorValue / 2) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    //recheck new width is set
    if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          set value to:           %lld is half value of the sensor width: %lld\n\n", (long long)iNodeValue, (long long)iSensorValue);

    //IMAGE OFFSET X
    //==============
    if (BGAPI2_Device_GetRemoteNode(pDevice, "OffsetX", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("        OffsetX \n");
    if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          current value:          %lld \n", (long long)iNodeValue);
    //set new width value same to the half value of sensor width
    if (setIntValue(pNode, iSensorValue / 2) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    //recheck new width is set
    if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          set value to:           %lld is half value of the sensor width: %lld\n\n", (long long)iNodeValue, (long long)iSensorValue);

    //IMAGE HEIGHT
    //============
    if (BGAPI2_Device_GetRemoteNode(pDevice, "Height", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("        Height \n");
    if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          current value:          %lld \n", (long long)iNodeValue);
    //get sensor height
    if (BGAPI2_Device_GetRemoteNode(pDevice, "SensorHeight", &pSensorNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetInt(pSensorNode, &iSensorValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    //set new width value same to the half value of sensor height
    if (setIntValue(pNode, iSensorValue / 2) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    //recheck new width is set
    if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          set value to:           %lld is half value of the sensor height: %lld\n\n", (long long)iNodeValue, (long long)iSensorValue);


    //IMAGE OFFSET Y
    //==============
    if (BGAPI2_Device_GetRemoteNode(pDevice, "OffsetY", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("        OffsetY \n");
    if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          current value:          %lld \n", (long long)iNodeValue);
    //set new width value same to the half value of sensor height
    if (setIntValue(pNode, iSensorValue / 2) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    //recheck new width is set
    if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          set value to:           %lld is half value of the sensor height: %lld\n\n", (long long)iNodeValue, (long long)iSensorValue);

    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT run() {
    BGAPI2_Device* pDevice = NULL;
    BGAPI2_DataStream* pStream = NULL;
    BGAPI2_RESULT res = BGAPI2_RESULT_SUCCESS;
    if (getFirstDevice(&pDevice) == BGAPI2_RESULT_SUCCESS) {
        res = setDeviceParameter(pDevice);
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = startStreaming(pDevice, &pStream);
        }
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = captureFrames(pStream);
        }
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = stopStreaming(pDevice, pStream);
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
