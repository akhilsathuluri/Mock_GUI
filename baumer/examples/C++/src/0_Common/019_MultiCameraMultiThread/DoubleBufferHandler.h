/* Copyright 2019-2020 Baumer Optronic */
#pragma once

#include <thread>
#include <vector>
#include <mutex>
#include <list>

#include "bgapi2_genicam/bgapi2_genicam.hpp"

class DoubleBufferHandler{
 public:
    DoubleBufferHandler();
    DoubleBufferHandler(const DoubleBufferHandler &instance);
    virtual ~DoubleBufferHandler();
    virtual void PushBuffer(BGAPI2::Buffer *buffer);
    virtual BGAPI2::Buffer* PullBuffer();
    virtual void FreeBuffer(BGAPI2::Buffer *buffer);
    virtual void Init();
    virtual bool HasNewData();
    DoubleBufferHandler & operator =(const DoubleBufferHandler &instance);

 private:
    BGAPI2::Buffer * buffer_read_;
    BGAPI2::Buffer * buffer_write_;
    std::mutex buffer_exchange_lock_;
    bool new_data;
};
