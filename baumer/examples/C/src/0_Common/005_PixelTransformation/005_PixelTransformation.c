/*
This example describes the FIRST STEPS of handling Baumer-GAPI SDK.
The given source code applies to handling one system, one camera and eight images.
Please see "Baumer-GAPI SDK Programmer's Guide" chapter 5.5
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bgapi2_genicam/bgapi2_genicam.h"

#define BUFFERCOUNT 4

// LOAD IMAGE PROCESSOR
BGAPI2_RESULT loadImageProcessor(BGAPI2_ImageProcessor** ppImgProc) {
    char myString[1024] = "";
    bo_uint64  iStringlength = sizeof(myString);
    BGAPI2_Node* pNode = NULL;

    printf("IMAGE PROCESSOR\n");
    if (BGAPI2_CreateImageProcessor(ppImgProc) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_ImageProcessor_GetVersion(*ppImgProc, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("ImageProcessor version:    %s\n", myString);
    if (BGAPI2_ImageProcessor_GetNode(*ppImgProc, "DemosaicingMethod", &pNode) == BGAPI2_RESULT_SUCCESS) {
        if (BGAPI2_Node_SetString(pNode, "NearestNeighbor") != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;  // NearestNeighbor, Bilinear3x3, Baumer5x5
        iStringlength = sizeof(myString);
        if (BGAPI2_Node_GetString(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("    Demosaicing method:    %s\n", myString);
    }
    return BGAPI2_RESULT_SUCCESS;
}

// RELEASE IMAGE PROCESSOR
BGAPI2_RESULT releaseImageProcessor(BGAPI2_ImageProcessor* pImgProc) {
    return BGAPI2_ReleaseImageProcessor(pImgProc);
}

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

// IMAGE
BGAPI2_RESULT handleImage(BGAPI2_Buffer* pBuffer, BGAPI2_ImageProcessor* pImgProc) {
    bo_uint64 iWidth = 0;
    bo_uint64 iHeight = 0;
    bo_uint iImageWidth = 0;  // Image uses 32 bit data type
    bo_uint iImageHeight = 0;  // Image uses 32 bit data type
    char sPixelFormat[1024] = "";
    bo_uint64 iStringlength = sizeof(sPixelFormat);
    void* pMemory = NULL;
    unsigned char* pImageBuffer = NULL;
    bo_uint64 iImageOffset = 0;
    bo_uint64 iBufferSize = 0;
    BGAPI2_Image* pImage = NULL;
    BGAPI2_Image* pTransformImage = NULL;
    BGAPI2_Node* pNode = NULL;
    bo_double fBytesPerPixel = 0.0;
    bo_uint i, j = 0;
    bo_uint64 iBufferFrameID = 0;

    if (BGAPI2_Buffer_GetWidth(pBuffer, &iWidth) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Buffer_GetHeight(pBuffer, &iHeight) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Buffer_GetPixelFormat(pBuffer, sPixelFormat, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Buffer_GetMemPtr(pBuffer, &pMemory) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Buffer_GetImageOffset(pBuffer, &iImageOffset) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Buffer_GetMemSize(pBuffer, &iBufferSize) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;

    // CREATE IMAGE FROM BUFFER
    if (BGAPI2_ImageProcessor_CreateImage(pImgProc, (bo_uint)iWidth, (bo_uint)iHeight, sPixelFormat,
        (void *)((char*)pMemory + iImageOffset), iBufferSize, &pImage) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;

    iStringlength = sizeof(sPixelFormat);
    if (BGAPI2_Image_GetPixelformat(pImage, sPixelFormat, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("  pImage.Pixelformat:             %s \n", sPixelFormat);
    if (BGAPI2_Image_GetWidth(pImage, &iImageWidth) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("  pImage.Width:                   %d \n", iImageWidth);
    if (BGAPI2_Image_GetHeight(pImage, &iImageHeight) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("  pImage.Height:                  %d \n", iImageHeight);
    if (BGAPI2_Image_GetBuffer(pImage, &pMemory) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("  pImage.Buffer:                  %p \n", pMemory);
    if (BGAPI2_ImageProcessor_GetNode(pImgProc, "PixelFormatInfoSelector", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_SetString(pNode, sPixelFormat) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_ImageProcessor_GetNode(pImgProc, "BytesPerPixel", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    BGAPI2_Node_GetDouble(pNode, &fBytesPerPixel);
    printf("  Bytes per image:                %lld\n", (long long)(iImageWidth * iImageHeight * fBytesPerPixel));
    printf("  Bytes per pixel:                %.1f\n", fBytesPerPixel);

    // display first 6 pixel values of first 6 lines of the image
    //========================================================================
    pImageBuffer = (unsigned char *)pMemory;
    printf("  Address\n");
    for (j = 0; j < 6; j++) //first 6 lines
    {
        pMemory = &pImageBuffer[(int)(iImageWidth*j*fBytesPerPixel)];
        printf("  %p", pMemory);
        for (i = 0; i < (bo_uint)(6 * fBytesPerPixel); i++) // bytes of first 6 pixels 
        {
            printf(" %02X", pImageBuffer[(int)(iImageWidth*j*fBytesPerPixel) + i]);
        }
        printf("  ... \n");
    }


    if (strncmp(sPixelFormat, "Mono", 4) == 0) {
        // if pixel format starts with "Mono"
        if (BGAPI2_ImageProcessor_CreateTransformedImage(pImgProc, pImage, "Mono8", &pTransformImage) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Buffer_GetFrameID(pBuffer, &iBufferFrameID) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("\n Image %5llu transformed to Mono8 \n", (unsigned long long)iBufferFrameID);
        iStringlength = sizeof(sPixelFormat);
        if (BGAPI2_Image_GetPixelformat(pTransformImage, sPixelFormat, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("  pImage.Pixelformat:             %s\n", sPixelFormat);
        if (BGAPI2_Image_GetWidth(pTransformImage, &iImageWidth) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("  pImage.Width:                   %d\n", iImageWidth);
        if (BGAPI2_Image_GetHeight(pTransformImage, &iImageHeight) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("  pImage.Height:                  %d\n", iImageHeight);
        if (BGAPI2_Image_GetBuffer(pTransformImage, &pMemory) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("  pImage.Buffer:                  %p\n", pMemory);
        if (BGAPI2_ImageProcessor_GetNode(pImgProc, "PixelFormatInfoSelector", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_SetString(pNode, sPixelFormat) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_ImageProcessor_GetNode(pImgProc, "BytesPerPixel", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        BGAPI2_Node_GetDouble(pNode, &fBytesPerPixel);
        printf("  Bytes per image:                %lld\n", (long long)(iImageWidth * iImageHeight * fBytesPerPixel));
        printf("  Bytes per pixel:                %.1f\n", fBytesPerPixel);

        // display first 6 pixel values of first 6 lines of the image
        //========================================================================
        pImageBuffer = (unsigned char *)pMemory;
        printf("  Address           Y  Y  Y  Y  Y  Y \n");
        for (j = 0; j < 6; j++) {
            printf("  %016llX", (unsigned long long)&pImageBuffer[(int)(iImageWidth*j)]);
            for (i = 0; i < 6; i++) {
                printf(" %02X", pImageBuffer[(int)(iImageWidth*j) + i]);
            }
            printf("  ... \n");
        }
    } else {
        // if color format
        if (BGAPI2_ImageProcessor_CreateTransformedImage(pImgProc, pImage, "BGR8", &pTransformImage) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Buffer_GetFrameID(pBuffer, &iBufferFrameID) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("\n Image %5llu transformed to BGR8 \n", (unsigned long long)iBufferFrameID);
        iStringlength = sizeof(sPixelFormat);
        if (BGAPI2_Image_GetPixelformat(pTransformImage, sPixelFormat, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("  pImage.Pixelformat:             %s\n", sPixelFormat);
        if (BGAPI2_Image_GetWidth(pTransformImage, &iImageWidth) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("  pImage.Width:                   %d\n", iImageWidth);
        if (BGAPI2_Image_GetHeight(pTransformImage, &iImageHeight) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("  pImage.Height:                  %d\n", iImageHeight);
        if (BGAPI2_Image_GetBuffer(pTransformImage, &pMemory) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf("  pImage.Buffer:                  %p\n", pMemory);
        if (BGAPI2_ImageProcessor_GetNode(pImgProc, "PixelFormatInfoSelector", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_Node_SetString(pNode, sPixelFormat) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_ImageProcessor_GetNode(pImgProc, "BytesPerPixel", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        BGAPI2_Node_GetDouble(pNode, &fBytesPerPixel);
        printf("  Bytes per image:                %lld\n", (long long)(iImageWidth * iImageHeight * fBytesPerPixel));
        printf("  Bytes per pixel:                %.1f\n", fBytesPerPixel);

        // display first 6 pixel values of first 6 lines of the image
        //========================================================================
        pImageBuffer = (unsigned char *)pMemory;
        printf("  Address           B  G  R  B  G  R  B  G  R  B  G  R  B  G  R  B  G  R \n");
        for (j = 0; j < 6; j++) {
            printf("  %016llX", (unsigned long long) &pImageBuffer[(int)(iImageWidth*j * 3)]);
            for (i = 0; i < 6; i++) {
                printf(" %02X", pImageBuffer[(int)(iImageWidth*j * 3) + i * 3 + 0]); // Blue value of pixel
                printf(" %02X", pImageBuffer[(int)(iImageWidth*j * 3) + i * 3 + 1]); // Green value of pixel
                printf(" %02X", pImageBuffer[(int)(iImageWidth*j * 3) + i * 3 + 2]); // Red value of pixel
            }
            printf("  ... \n");
        }
    }
    printf("\n");

    //RELEASE IMAGE
    if (BGAPI2_Image_Release(pImage) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Image_Release(pTransformImage) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT captureFrames(BGAPI2_DataStream* pStream, BGAPI2_ImageProcessor* pImgProc) {
    int framecount = 0;

    printf("\nCAPTURE 8 IMAGES BY IMAGE POLLING\n");
    printf("##################################\n\n");

    for (framecount = 0; framecount < 8; framecount++) {
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
                printf("Image %lld received in buffer address ", (long long)iFrameId);
                if (BGAPI2_Buffer_GetMemPtr(pBufferFilled, &pBufferAddress) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf("%p\n", pBufferAddress);
                if (handleImage(pBufferFilled, pImgProc) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
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
 
    if (BGAPI2_Device_GetRemoteNode(pDevice, "TriggerMode", &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_SetString(pNode, "Off") != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT run() {
    BGAPI2_Device* pDevice = NULL;
    BGAPI2_DataStream* pStream = NULL;
    BGAPI2_ImageProcessor* pImgProc = NULL;
    BGAPI2_RESULT res = BGAPI2_RESULT_SUCCESS;
    if (loadImageProcessor(&pImgProc) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (getFirstDevice(&pDevice) == BGAPI2_RESULT_SUCCESS) {
        res = setDeviceParameter(pDevice);
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = startStreaming(pDevice, &pStream);
        }
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = captureFrames(pStream, pImgProc);
        }
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = stopStreaming(pDevice, pStream);
        }
        // always release resources
        if (releaseResources(pDevice) != BGAPI2_RESULT_SUCCESS) {
            res = BGAPI2_RESULT_ERROR;
        }
    }
    res = releaseImageProcessor(pImgProc);
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
