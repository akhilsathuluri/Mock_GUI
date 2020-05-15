// Shared.h
#ifndef SHARED_SHARED_H_
#define SHARED_SHARED_H_

#include <string>
#include "bgapi2_genicam/bgapi2_genicam.hpp"

typedef std::vector<std::string> stringvec;

// extern void read_directory(const std::string& name, const std::string& filter, stringvec& v);
extern int initTest(BGAPI2::Device** ppDevice, BGAPI2::DataStream** ppDataStream, std::string sSystemList = "GEV");
extern int exitTest();
extern BGAPI2::Node* nodeEntry(BGAPI2::NodeMap* const pNodeMap, const char* const pName, const bool bRequired,
                               unsigned int* const pMissing = NULL);
#endif  // SHARED_SHARED_H_
