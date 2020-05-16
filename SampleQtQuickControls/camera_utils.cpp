#include "camera_utils.h"
#include <QDebug>
#include "bgapi2_genicam.hpp"
#include <stdio.h>
#include <iostream>
#include <iomanip>
using namespace BGAPI2;

BGAPI2::SystemList* systemList = NULL;
BGAPI2::System* pSystem = NULL;
BGAPI2::String sSystemID;

BGAPI2::InterfaceList* interfaceList = NULL;
BGAPI2::Interface* pInterface = NULL;
BGAPI2::String sInterfaceID;

BGAPI2::DeviceList* deviceList = NULL;
BGAPI2::Device* pDevice = NULL;
BGAPI2::String sDeviceID;

BGAPI2::DataStreamList* datastreamList = NULL;
BGAPI2::DataStream* pDataStream = NULL;
BGAPI2::String sDataStreamID;

BGAPI2::BufferList* bufferList = NULL;
BGAPI2::Buffer* pBuffer = NULL;
BGAPI2::String sBufferID;
int returncode = 0;

cameraUtils::cameraUtils(QObject *parent)
    : QObject(parent)
{

}

//void cameraUtils::onButtonClicked()
//{
//    systemList = SystemList::GetInstance();
//    systemList->Refresh();
//    qDebug() << "Devices: " << systemList->size();
//    for (SystemList::iterator sysIterator = systemList->begin(); sysIterator != systemList->end(); sysIterator++)
//    {
//        qDebug() << "  5.2.1   System Name:     " << sysIterator->second->GetFileName();
//            qDebug() << "          System Type:     " << sysIterator->second->GetTLType();
//            qDebug() << "          System Version:  " << sysIterator->second->GetVersion();
//            qDebug() << "          System PathName: " << sysIterator->second->GetPathName();
//    }
//    emit echo();
//}
QString cameraUtils::result()
{
    systemList = SystemList::GetInstance();
    systemList->Refresh();
    char a[14] = "\n";
    QString y;
    y.append("Device List \n");
    for (SystemList::iterator sysIterator = systemList->begin(); sysIterator != systemList->end(); sysIterator++)
    { y.append((QString)sysIterator->second->GetPathName());
    y.append(a);
    }

    return y;

}
