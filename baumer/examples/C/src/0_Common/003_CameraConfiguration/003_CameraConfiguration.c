/*
This example describes the FIRST STEPS of handling Baumer-GAPI SDK.
The given source code applies to handling one system, one camera and ten images.
Please see "Baumer-GAPI SDK Programmer's Guide" chapter 5.3
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

BGAPI2_RESULT captureFrames(BGAPI2_Device* pDevice, BGAPI2_DataStream* pStream) {
    int framecount = 0;
    BGAPI2_Node* pNode = NULL;
    char myString[1024] = "";
    bo_uint64  iStringlength = sizeof(myString);

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

BGAPI2_RESULT setDeviceParameter(BGAPI2_Device* pDevice) {
    BGAPI2_Node* pNode = NULL;
    char sEnumNodeValueCurrent[1024] = "";
    char userName[1024] = "";
    char myString[1024] = "";
    bo_uint64  iStringlength = sizeof(myString);
    bo_double fNodeValue = 0.0;
    bo_double fNodeValueMin = 0.0;
    bo_double fNodeValueMax = 0.0;
    bo_int64 iNodeValue = 0;
    bo_int64 iNodeValueMin = 0;
    bo_int64 iNodeValueMax = 0;
    bo_int64 iNodeValueInc = 0;
    BGAPI2_NodeMap* pNodeList = NULL;
    bo_uint64 iEnumNodeListCount = 0;
    bo_uint64 iIndexEnumNodeList = 0;
    bo_bool bNodePresent = 0;
    bo_bool bNodeValueBoolean = 0;

    printf("DEVICE PARAMETER SETUP\n");
    printf("######################\n\n");

    //EXAMPLE DEVICE REMOTE NODE INTERFACE TYPE -> "IFloat"
    //======================================================
    //EXPOSURE TIME
    printf("5.3.2   ExposureTime\n");
    if (BGAPI2_Device_GetRemoteNode(pDevice, "ExposureTime", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetDescription(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          description:            %s\n", myString);
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetInterface(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          interface type:         %s\n", myString);
    if (BGAPI2_Node_GetDouble(pNode, &fNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          current value:          %.0f \n", fNodeValue);
    if (BGAPI2_Node_GetDoubleMin(pNode, &fNodeValueMin) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetDoubleMax(pNode, &fNodeValueMax) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          possible value range:   %.0f to %.0f \n", fNodeValueMin, fNodeValueMax);
    //set new exposure value to 20000 usec
    fNodeValue = 20000;

    // check new value is within range
    if (fNodeValue < fNodeValueMin)
        fNodeValue = fNodeValueMin;
    if (fNodeValue > fNodeValueMax)
        fNodeValue = fNodeValueMax;

    if (BGAPI2_Node_SetDouble(pNode, fNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;

    //recheck new exposure is set
    if (BGAPI2_Node_GetDouble(pNode, &fNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          set value to:           %.0f \n\n", fNodeValue);

    //EXAMPLE DEVICE REMOTE NODE INTERFACE TYPE -> "IInteger"
    //=======================================================
    //IMAGE WIDTH
    printf("5.3.3   Width \n");
    if (BGAPI2_Device_GetRemoteNode(pDevice, "Width", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetDescription(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          description:            %s\n", myString);
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetInterface(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          interface type:         %s\n", myString);
    if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          current value:          %lld \n", (long long)iNodeValue);
    if (BGAPI2_Node_GetIntMin(pNode, &iNodeValueMin) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetIntMax(pNode, &iNodeValueMax) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetIntInc(pNode, &iNodeValueInc) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          possible value range:   %lld to %lld with increment of %lld \n", (long long)iNodeValueMin, (long long)iNodeValueMax, (long long)iNodeValueInc);

    //get height value
    if (BGAPI2_Device_GetRemoteNode(pDevice, "Height", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;

    // find number to match the increment
    iNodeValue = (iNodeValue - iNodeValueMin) / iNodeValueInc * iNodeValueInc + iNodeValueMin;

    // check new value is within range
    if (iNodeValue < iNodeValueMin)
        iNodeValue = iNodeValueMin;

    if (iNodeValue > iNodeValueMax)
        iNodeValue = iNodeValueMax;

    //set width value
    if (BGAPI2_Device_GetRemoteNode(pDevice, "Width", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_SetInt(pNode, iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;

    //recheck new width is set
    if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          set value to:           %lld  is about same as the height: ", (long long)iNodeValue);

    //get height value
    if (BGAPI2_Device_GetRemoteNode(pDevice, "Height", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetInt(pNode, &iNodeValue) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("%lld \n\n", (long long)iNodeValue);

    //EXAMPLE DEVICE REMOTE NODE INTERFACE TYPE -> "IEnumeration"
    //============================================================
    //TRIGGER SOURCE 
    printf("5.3.4   TriggerSource \n");
    if (BGAPI2_Device_GetRemoteNode(pDevice, "TriggerSource", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetDescription(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          description:            %s\n", myString);
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetInterface(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          interface type:         %s\n", myString);
    iStringlength = sizeof(sEnumNodeValueCurrent);
    if (BGAPI2_Node_GetValue(pNode, sEnumNodeValueCurrent, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;

    //NODE ENUMERATION COUNT
    if (BGAPI2_Node_GetEnumNodeList(pNode, &pNodeList) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_NodeMap_GetNodeCount(pNodeList, &iEnumNodeListCount) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          enumeration list count: %lld\n", (long long)iEnumNodeListCount);
    for (iIndexEnumNodeList = 0; iIndexEnumNodeList < iEnumNodeListCount; iIndexEnumNodeList++) {
        BGAPI2_Node * pEnumEntry = NULL;
        bo_bool bReadable;
        if (BGAPI2_NodeMap_GetNodeByIndex(pNodeList, iIndexEnumNodeList, &pEnumEntry) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_IsReadable(pEnumEntry, &bReadable) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (bReadable) {
            iStringlength = sizeof(myString);
            if (BGAPI2_Node_GetValue(pEnumEntry, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (strcmp(myString, sEnumNodeValueCurrent) == 0) {
                printf("                           [%2lld]: *%s\n", (long long)iIndexEnumNodeList, myString); //current value
            } else {
                printf("                           [%2lld]:  %s\n", (long long)iIndexEnumNodeList, myString);
            }
        }
    }

    //SET TRIGGER SOURCE "SOFTWARE"
    if (BGAPI2_Node_SetString(pNode, "Software") != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    iStringlength = sizeof(sEnumNodeValueCurrent);
    if (BGAPI2_Node_GetValue(pNode, sEnumNodeValueCurrent, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          set value to:           %s\n", sEnumNodeValueCurrent);

    //SET TRIGGER MODE ON
    printf("        TriggerMode \n");
    if (BGAPI2_Device_GetRemoteNode(pDevice, "TriggerMode", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetDescription(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          description:            %s\n", myString);
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetInterface(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          interface type:         %s\n", myString);
    iStringlength = sizeof(sEnumNodeValueCurrent);
    if (BGAPI2_Node_GetValue(pNode, sEnumNodeValueCurrent, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          current value:          %s\n", myString);
    if (BGAPI2_Node_SetString(pNode, "On") != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetValue(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          set value to:           %s\n", myString);

    //EXAMPLE DEVICE REMOTE NODE INTERFACE TYPE -> "IBoolean"
    //========================================================
    //SEARCH FOR 'LineInverter'
    if (BGAPI2_Device_GetRemoteNodeList(pDevice, &pNodeList) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_NodeMap_GetNodePresent(pNodeList, "LineInverter", &bNodePresent) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (bNodePresent == 1) {
        //select an I/O line first = Line0
        if (BGAPI2_Device_GetRemoteNode(pDevice, "LineSelector", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_SetString(pNode, "Line0") != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("5.3.5   LineInverter \n");
        if (BGAPI2_Device_GetRemoteNode(pDevice, "LineInverter", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        iStringlength = sizeof(myString);
        if (BGAPI2_Node_GetDescription(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("          description:            %s\n", myString);
        iStringlength = sizeof(myString);
        if (BGAPI2_Node_GetInterface(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("          interface type:         %s\n", myString);
        if (BGAPI2_Node_GetBool(pNode, &bNodeValueBoolean) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("          current value:          %d\n", bNodeValueBoolean);

        //LineStatus
        if (BGAPI2_Device_GetRemoteNode(pDevice, "LineStatus", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetBool(pNode, &bNodeValueBoolean) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("          Line0 LineStatus:       %d\n", bNodeValueBoolean);

        //set LineInverter of Line0 to true
        if (BGAPI2_Device_GetRemoteNode(pDevice, "LineInverter", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_SetBool(pNode, 1) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        //recheck new value
        if (BGAPI2_Node_GetBool(pNode, &bNodeValueBoolean) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("          set value to:           %d\n", bNodeValueBoolean);

        //LineStatus
        if (BGAPI2_Device_GetRemoteNode(pDevice, "LineStatus", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_GetBool(pNode, &bNodeValueBoolean) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("          Line0 LineStatus:       %d\n", bNodeValueBoolean);
        printf("\n");
    }

    //EXAMPLE DEVICE REMOTE NODE INTERFACE TYPE -> "IString"
    //=======================================================
    //DEVICE USER ID (LABEL)
    printf("5.3.6   DeviceUserID \n");
    if (BGAPI2_Device_GetRemoteNode(pDevice, "DeviceUserID", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetDescription(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          description:            %s\n", myString);
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetInterface(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          interface type:         %s\n", myString);
    iStringlength = sizeof(userName);
    if (BGAPI2_Node_GetValue(pNode, userName, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          current value:          %s\n", userName);

    //SET A NEW USER ID LIKE "Camera0"
    strcpy(myString, "Camera0");
    //CONFIRM STRING LENGTH IS MATCHING
    if (BGAPI2_Node_GetMaxStringLength(pNode, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (strlen(myString) < iStringlength) {
        if (BGAPI2_Node_SetString(pNode, myString) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    } else {
        printf("Error: string length of '%s' (%d) is longer than max string length (%lld)\n", myString, (int)strlen(myString), (long long)iStringlength);
    }
    //RECHECK CHANGES
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetValue(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("          set value to:           %s\n\n", myString);
    // RESTORE OLD NAME
    if (BGAPI2_Node_SetString(pNode, userName) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;

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
            res = captureFrames(pDevice, pStream);
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
