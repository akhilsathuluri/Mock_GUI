/* Copyright 2019-2020 Baumer Optronic */
#pragma once

#include <thread>
#include <vector>
#include <mutex>
#include <list>

#include "bgapi2_genicam/bgapi2_genicam.hpp"
#include "Camera.h"
#include "DoubleBufferHandler.h"
#include "TimingStatistic.h"

// the class represents a BGAPI camera object and provide several timing information
class CameraTiming : public Camera {
 public:
    explicit CameraTiming(BGAPI2::Device* bgapi_device);
    virtual ~CameraTiming();
    // this function is responsible to reset stored timing information before the camera starts
    // capturing new images
    virtual bool CaptureBGAPIImages(const bool * abort_flag, unsigned int number_of_images);

    // this function elevates several timing information
    virtual void BufferReceived(BGAPI2::DataStream* datastream_pointer, BGAPI2::Buffer *buffer);

    // this function activated the chunk frameID if supported, before the camera starts streaming
    virtual bool StartStreamingBGAPI();

    // this function stops the streaming
    virtual void StopStreamingBGAPI();

 public:
    // flag to indicate if sensor id is supported as chunk block 'FrameId'
    bool supports_frameid_sensor_;

    // timing statistic for start streaming
    TimingStat tmr_start_;

    // timing statistic for stop streaming
    TimingStat tmr_stop_;

    // timing statistic for GetFilledBuffer
    TimingStat tmr_get_buffer_;

    // timing statistic from the camera timestamp
    TimingStat tmr_camera_ts_;

    // timing statistic from the host timestamp
    TimingStat tmr_host_ts_;

    // timestamp frequency of the camera timestamp
    uint64_t cam_ts_freq_;

    // timestamp from the camera of the last captured image
    uint64_t cam_buffer_ts_last_;

    // timestamp from the camera of latest captured image
    uint64_t cam_buffer_ts_;

    // timestamp from the host of the last captured image
    uint64_t host_buffer_ts_last_;

    // timestamp from the host of the latest captured image
    uint64_t host_buffer_ts_;

 private:
    // native BGAPI camera object
    BGAPI2::Device* camera_pointer_;
};
