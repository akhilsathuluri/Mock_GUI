#include "camera_utils.h"
#include "bgapi2_genicam.hpp"
#include <QDebug>
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

camera_utils::camera_utils(QObject *parent) :
    QObject(parent)
{
}

int camera_utils::check_status()
{
    systemList = SystemList::GetInstance();
    systemList->Refresh();
    qDebug() << "Devices: " << systemList->size();
    for (SystemList::iterator sysIterator = systemList->begin(); sysIterator != systemList->end(); sysIterator++)
        {
//            qDebug() << "  5.2.1   System Name:     " << sysIterator->second->GetFileName();
//            qDebug() << "          System Type:     " << sysIterator->second->GetTLType();
//            qDebug() << "          System Version:  " << sysIterator->second->GetVersion();
//            qDebug() << "          System PathName: " << sysIterator->second->GetPathName();
        }
    return 0;
}

