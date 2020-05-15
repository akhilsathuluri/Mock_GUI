/*
This example describes the FIRST STEPS of handling Baumer-GAPI SDK.
The given source code applies to handling one system, one camera and display the list of features.
Please see "Baumer-GAPI SDK Programmer's Guide" chapter 5.3
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

BGAPI2_RESULT printNodeRecursive(BGAPI2_Node * pNode, int level) {
    char myString[1024] = "";         // defines an char array
    char myNodeInterface[1024] = "";         // defines an char array
    bo_uint64  iStringlength = sizeof(myNodeInterface);
    int i = 0;

    for (i = 0; i < level * 7 + 1; i++) {
        printf(" ");
    }
    if (BGAPI2_Node_GetInterface(pNode, myNodeInterface, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("[%-12s]", myNodeInterface);
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetName(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;

    if (strcmp(myNodeInterface, "ICategory") == 0) {
        BGAPI2_NodeMap * pNodeTree = NULL;
        bo_uint64 iNodeCount = 0;
        bo_uint64 iIndexNode = 0;
        BGAPI2_Node * nSubNode = NULL;
        printf(" %s\n", myString);
        if (BGAPI2_Node_GetNodeTree(pNode, &pNodeTree) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_NodeMap_GetNodeCount(pNodeTree, &iNodeCount) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        for (iIndexNode = 0; iIndexNode < iNodeCount; iIndexNode++) {
            if (BGAPI2_NodeMap_GetNodeByIndex(pNodeTree, iIndexNode, &nSubNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printNodeRecursive(nSubNode, level + 1);
        }
    } else {
        bo_bool bReadable;
        printf(" %-44s", myString);
        if (BGAPI2_Node_IsReadable(pNode, &bReadable) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (bReadable) {
            iStringlength = sizeof(myString);
            if (BGAPI2_Node_GetValue(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(": %s", myString);
        }
    }
    printf("\n");
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT printDeviceRemoteNodeInformation(BGAPI2_Device * pDevice, char* sNodeName) {
    BGAPI2_NodeMap * mDeviceRemoteNodeList = NULL;
    bo_bool bNodePresent = 0;
    BGAPI2_Node* pNode = NULL;
    char myString[1024] = "";
    char sNodeInterface[1024] = "";
    bo_uint64  iStringlength = sizeof(sNodeInterface);
    bo_bool bCurNodeAvailable = 0;
    bo_bool bCurNodeImplemented = 0;
    bo_bool bCurNodeIsSelector = 0;
    bo_bool bCurNodeReadable = 0;
    printf("printDeviceRemoteNodeInformation: '%s'\n", sNodeName);
    if (BGAPI2_Device_GetRemoteNodeList(pDevice, &mDeviceRemoteNodeList) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_NodeMap_GetNodePresent(mDeviceRemoteNodeList, sNodeName, &bNodePresent) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (bNodePresent == 0) {
        printf("ERROR - Node '%s' does not exist.\n\n", sNodeName);
        return BGAPI2_RESULT_ERROR;
    }
    if (BGAPI2_Device_GetRemoteNode(pDevice, sNodeName, &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_Node_GetInterface(pNode, sNodeInterface, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf(" Node Interface:           %s\n", sNodeInterface);
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetName(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf(" Node Name:                %s\n", myString);
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetDisplayname(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf(" Node Display Name:        %s\n", myString);
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetDescription(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf(" Node Description:         %s\n", myString);
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetToolTip(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf(" Node Tool Tip:            %s\n", myString);
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetVisibility(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf(" Node Visibility:          %s\n", myString);
    if (BGAPI2_Node_GetImplemented(pNode, &bCurNodeImplemented) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf(" Node Is Implemented:      %d\n", bCurNodeImplemented);
    if (bCurNodeImplemented == 1) {
        if (BGAPI2_Node_GetAvailable(pNode, &bCurNodeAvailable) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf(" Node Is Available:        %d\n", bCurNodeAvailable);
    }
    iStringlength = sizeof(myString);
    if (BGAPI2_Node_GetCurrentAccessMode(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf(" Node Current Access Mode: %s\n", myString);
    if (BGAPI2_Node_IsSelector(pNode, &bCurNodeIsSelector) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf(" Node Is Selector:         %d\n", bCurNodeIsSelector);
    if (BGAPI2_Node_IsReadable(pNode, &bCurNodeReadable) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf(" Node Is Readable:         %d\n", bCurNodeReadable);
    if (bCurNodeReadable) {
        if (strcmp(sNodeInterface, "IBoolean") == 0) {
            bo_bool bNodeValueBoolean = 0;
            iStringlength = sizeof(myString);
            if (BGAPI2_Node_GetValue(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node Value:               %s\n", myString);
            if (BGAPI2_Node_GetBool(pNode, &bNodeValueBoolean) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node Value Boolean:       %d\n", bNodeValueBoolean);
        } else if (strcmp(sNodeInterface, "ICommand") == 0) {
            bo_bool bNodeValueBoolean = 0;
            if (BGAPI2_Node_IsDone(pNode, &bNodeValueBoolean) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node isDone:              %d\n", bNodeValueBoolean);
        } else if (strcmp(sNodeInterface, "IEnumeration") == 0) {
            bo_int64 iNodeValueInteger = 0;
            bo_uint64 iEnumNodeListCount = 0;
            BGAPI2_NodeMap * pEnumNodeList = NULL;
            bo_uint64 iIndexEnumNodeList = 0;
            iStringlength = sizeof(myString);
            if (BGAPI2_Node_GetValue(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node Value:               %s\n", myString);
            if (BGAPI2_Node_GetInt(pNode, &iNodeValueInteger) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node Value (Integer):     %lld\n", (long long)iNodeValueInteger);
            if (BGAPI2_Node_GetEnumNodeList(pNode, &pEnumNodeList) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (BGAPI2_NodeMap_GetNodeCount(pEnumNodeList, &iEnumNodeListCount) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node Enumeration Count:   %lld\n", (long long)iEnumNodeListCount);
            for (iIndexEnumNodeList = 0; iIndexEnumNodeList < iEnumNodeListCount; iIndexEnumNodeList++) {
                BGAPI2_Node * pEnumEntry = NULL;
                if (BGAPI2_NodeMap_GetNodeByIndex(pEnumNodeList, iIndexEnumNodeList, &pEnumEntry) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                if (BGAPI2_Node_IsReadable(pEnumEntry, &bCurNodeReadable) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                if (bCurNodeReadable) {
                    iStringlength = sizeof(myString);
                    if (BGAPI2_Node_GetValue(pEnumEntry, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                    printf("                     [%2lld]: %s\n", (long long)iIndexEnumNodeList, myString);
                }
            }
        } else if (strcmp(sNodeInterface, "IFloat") == 0) {
            bo_double fNodeValueDouble = 0.0;
            bo_bool bNodeHasUnit = 0;
            iStringlength = sizeof(myString);
            if (BGAPI2_Node_GetValue(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node Value:               %s\n", myString);
            if (BGAPI2_Node_GetDouble(pNode, &fNodeValueDouble) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node Double:              %.3f\n", fNodeValueDouble);
            if (BGAPI2_Node_GetDoubleMin(pNode, &fNodeValueDouble) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node DoubleMin:           %.3f\n", fNodeValueDouble);
            if (BGAPI2_Node_GetDoubleMax(pNode, &fNodeValueDouble) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node DoubleMax:           %.3f\n", fNodeValueDouble);
            if (BGAPI2_Node_HasUnit(pNode, &bNodeHasUnit) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (bNodeHasUnit == 1) {
                iStringlength = sizeof(myString);
                if (BGAPI2_Node_GetUnit(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf(" Node Unit:                %s\n", myString);
            }
        } else if (strcmp(sNodeInterface, "IInteger") == 0) {
            bo_int64 iNodeValueInteger = 0;
            bo_bool bNodeHasUnit = 0;
            iStringlength = sizeof(myString);
            if (BGAPI2_Node_GetValue(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node Value:               %s\n", myString);
            if (BGAPI2_Node_GetInt(pNode, &iNodeValueInteger) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node Value (Integer):     %lld\n", (long long)iNodeValueInteger);
            if (BGAPI2_Node_GetIntMin(pNode, &iNodeValueInteger) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node IntegerMin:          %lld\n", (long long)iNodeValueInteger);
            if (BGAPI2_Node_GetIntMax(pNode, &iNodeValueInteger) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node IntegerMax:          %lld\n", (long long)iNodeValueInteger);
            if (BGAPI2_Node_GetIntInc(pNode, &iNodeValueInteger) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node IntegerInc:          %lld\n", (long long)iNodeValueInteger);
            if (BGAPI2_Node_HasUnit(pNode, &bNodeHasUnit) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            if (bNodeHasUnit == 1) {
                iStringlength = sizeof(myString);
                if (BGAPI2_Node_GetUnit(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
                printf(" Node Unit:                %s\n", myString);
            }
        } else if (strcmp(sNodeInterface, "IString") == 0) {
            bo_int64 iNodeMaxStringLength = 0;
            iStringlength = sizeof(myString);
            if (BGAPI2_Node_GetValue(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node Value:               %s\n", myString);
            iStringlength = sizeof(myString);
            if (BGAPI2_Node_GetString(pNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node Value:               %s\n", myString);
            if (BGAPI2_Node_GetMaxStringLength(pNode, &iNodeMaxStringLength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node MaxStringLength:     %lld\n", (long long)iNodeMaxStringLength);
        } else if (strcmp(sNodeInterface, "IRegister") == 0) {
            bo_int64 iNodeLength = 0;
            bo_int64 iAddress = 0;
            void* pBuffer = NULL;
            bo_int64 iIndex = 0;
            if (BGAPI2_Node_GetLength(pNode, &iNodeLength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node MaxLength:           %lld\n", (long long)iNodeLength);
            if (BGAPI2_Node_GetAddress(pNode, &iAddress) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf(" Node address:             %llx\n", iAddress);
            pBuffer = malloc((int)iNodeLength);
            if (BGAPI2_Node_Get(pNode, pBuffer, iNodeLength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            for (iIndex = 0; iIndex < iNodeLength; iIndex++) {
                printf(" %02X ", ((unsigned char*)pBuffer)[(int)iIndex]);
            }
            free(pBuffer);
        }
    }

    if (bCurNodeIsSelector == 1) {
        BGAPI2_NodeMap * pSelectedFeaturesNodeList = NULL;
        bo_uint64 iSelectedFeaturesNodeListCount = 0;
        bo_uint64 iIndexSelectedFeatures = 0;
        BGAPI2_Node * pSelectedNode = NULL;
        if (BGAPI2_Node_GetSelectedFeatures(pNode, &pSelectedFeaturesNodeList) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        if (BGAPI2_NodeMap_GetNodeCount(pSelectedFeaturesNodeList, &iSelectedFeaturesNodeListCount) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printf(" SelectedNodeList Count:   %lld\n", (long long)iSelectedFeaturesNodeListCount);
        for (iIndexSelectedFeatures = 0; iIndexSelectedFeatures < iSelectedFeaturesNodeListCount; iIndexSelectedFeatures++) {
            if (BGAPI2_NodeMap_GetNodeByIndex(pSelectedFeaturesNodeList, iIndexSelectedFeatures, &pSelectedNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            iStringlength = sizeof(myString);
            if (BGAPI2_Node_GetName(pSelectedNode, myString, &iStringlength) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
            printf("                           %s\n", myString);
        }
    }
    return BGAPI2_RESULT_SUCCESS;
}

// SHOW TREE OF DEVICE NODES
BGAPI2_RESULT printDeviceNodeTree(BGAPI2_Device* pDevice) {
    BGAPI2_NodeMap* mDeviceRemoteNodeTree = NULL;
    BGAPI2_Node* pNode = NULL;
    bo_uint64 iDeviceRemoteNodeTreeNodeCount = 0;
    bo_uint64 iIndexNode = 0;
    printf("DEVICE NODE TREE\n");
    printf("################\n");
    printf("\n");
    if (BGAPI2_Device_GetRemoteNodeTree(pDevice, &mDeviceRemoteNodeTree) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    if (BGAPI2_NodeMap_GetNodeCount(mDeviceRemoteNodeTree, &iDeviceRemoteNodeTreeNodeCount) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
    printf("5.3.1   NodeTree Count: %lld\n\n", (long long)iDeviceRemoteNodeTreeNodeCount);
    for (iIndexNode = 0; iIndexNode < iDeviceRemoteNodeTreeNodeCount; iIndexNode++) {
        if (BGAPI2_NodeMap_GetNodeByIndex(mDeviceRemoteNodeTree, iIndexNode, &pNode) != BGAPI2_RESULT_SUCCESS) return BGAPI2_RESULT_ERROR;
        printNodeRecursive(pNode, 0);
    }
    return BGAPI2_RESULT_SUCCESS;
}

//DEVICE REMOTE NODE INFORMATION
BGAPI2_RESULT printDeviceNode(BGAPI2_Device* pDevice) {
    char myInputString[1024] = "";
    printf("\nDEVICE REMOTE NODE INFORMATION\n");
    printf("##############################\n");

    strcpy(myInputString, "TriggerSource");  // Example input for Node information
    do {
        printf("\n");
        printDeviceRemoteNodeInformation(pDevice, myInputString);
        printf("input name of the camera feature like 'ExposureTime' or 'exit' to stop: \n");
        if(scanf("%s", myInputString)) {}  // use 'if' to avoid compiler warning of unused return value
    }
    while (strcmp(myInputString, "exit") != 0);
    return BGAPI2_RESULT_SUCCESS;
}

BGAPI2_RESULT run() {
    BGAPI2_Device* pDevice = NULL;
    BGAPI2_RESULT res = BGAPI2_RESULT_SUCCESS;
    if (getFirstDevice(&pDevice) == BGAPI2_RESULT_SUCCESS) {
        res = printDeviceNodeTree(pDevice);
        if (res == BGAPI2_RESULT_SUCCESS) {
            res = printDeviceNode(pDevice);
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
