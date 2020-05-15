/* Copyright 2019-2020 Baumer Optronic */
#include <iostream>
#include <sstream>
#include <limits>
#include <iomanip>
#include "CameraTiming.h"
#include "DoubleBufferHandler.h"
#include "BufferInformation.h"

CameraTiming::CameraTiming(BGAPI2::Device* bgapi_device)
: Camera(bgapi_device)
, cam_ts_freq_(0)
, cam_buffer_ts_last_(0)
, cam_buffer_ts_(0)
, host_buffer_ts_last_(0)
, host_buffer_ts_(0)
, camera_pointer_(bgapi_device) {
    const BGAPI2::String tick_frequency_node = "GevTimestampTickFrequency";
    if (bgapi_device->GetRemoteNodeList()->GetNodePresent(tick_frequency_node)) {
        cam_ts_freq_ = bgapi_device->GetRemoteNodeList()->GetNode(tick_frequency_node)->GetInt();
    } else {
        cam_ts_freq_ = 1000000000;
    }
}

CameraTiming::~CameraTiming() {
}

bool CameraTiming::CaptureBGAPIImages(const bool * abort_flag, unsigned int number_of_images) {
    tmr_start_.Reset();
    tmr_stop_.Reset();
    tmr_get_buffer_.Reset();
    tmr_camera_ts_.Reset();
    return Camera::CaptureBGAPIImages(abort_flag, number_of_images);
}

void CameraTiming::BufferReceived(BGAPI2::DataStream* datastream_pointer, BGAPI2::Buffer *buffer) {
    BufferInformation * buffer_information = reinterpret_cast<BufferInformation*>(buffer->GetUserObj());
    const BGAPI2::String chunk_frame_id_node = "ChunkFrameID";
    buffer_information->valid = true;
    buffer_information->supports_frameid_sensor = supports_frameid_sensor_;
    if (buffer->GetIsIncomplete() == false) {
        buffer_information->frameid = buffer->GetFrameID();
        if (buffer_information->supports_frameid_sensor) {
            if (buffer->GetContainsChunk()) {
                if (buffer->GetChunkNodeList()->GetNodePresent(chunk_frame_id_node)) {
                    buffer_information->frameid_sensor = static_cast<unsigned int>(
                        buffer->GetChunkNodeList()->GetNode(chunk_frame_id_node)->GetInt());
                }
            }
        }
    }
    // time intervals between GetFilledBuffer, collect interval stats
    uint64_t getfilled_buffer_timestamp = 0;
    if (number_of_captured_images_ > 1) {
        getfilled_buffer_timestamp = tmr_get_buffer_.Stop();
    }
    tmr_get_buffer_.Start();
    // get image's camera timestamp, collect interval stats
    buffer_information->camera_buffer_ts = buffer->GetTimestamp();
    cam_buffer_ts_ = buffer_information->camera_buffer_ts;
    buffer_information->camera_buffer_ts_freq = cam_ts_freq_;
    if (number_of_captured_images_ > 1) {
        double m_sec = (static_cast<double>(cam_buffer_ts_ - cam_buffer_ts_last_) / cam_ts_freq_) * 1000.0;
        tmr_camera_ts_.Add_ms(m_sec);
    }

    cam_buffer_ts_last_ = cam_buffer_ts_;
    if (buffer->GetNodeList()->GetNodePresent(GENTL_SFNC_BUFFER_CUSTOM_HOSTTIMESTAMP)) {
        host_buffer_ts_ = static_cast<bo_uint64>(
            buffer->GetNodeList()->GetNode(GENTL_SFNC_BUFFER_CUSTOM_HOSTTIMESTAMP)->GetInt());
    }
    if (number_of_captured_images_ > 1) {
        double msec = static_cast<double>(host_buffer_ts_ - host_buffer_ts_last_) / 1000000;
        tmr_host_ts_.Add_ms(msec);
    }
    host_buffer_ts_last_ = host_buffer_ts_;
    buffer_information->camera_buffer_ts_diff = tmr_camera_ts_.Last_ms();
    buffer_information->camera_buffer_ts_diff_min = tmr_camera_ts_.Min_ms();
    buffer_information->camera_buffer_ts_diff_ave = tmr_camera_ts_.Ave_ms();
    buffer_information->camera_buffer_ts_diff_max = tmr_camera_ts_.Max_ms();
    buffer_information->host_buffer_ts = host_buffer_ts_;
    buffer_information->host_buffer_ts_diff = tmr_host_ts_.Last_ms();
    buffer_information->host_buffer_ts_diff_min = tmr_host_ts_.Min_ms();
    buffer_information->host_buffer_ts_diff_ave = tmr_host_ts_.Ave_ms();
    buffer_information->host_buffer_ts_diff_max = tmr_host_ts_.Max_ms();
    buffer_information->getfilled_buffer_ts = (bo_int64)(getfilled_buffer_timestamp);
    buffer_information->getfilled_buffer_ts_diff = tmr_get_buffer_.Last_ms();
    buffer_information->getfilled_buffer_ts_diff_min = tmr_get_buffer_.Min_ms();
    buffer_information->getfilled_buffer_ts_diff_ave = tmr_get_buffer_.Ave_ms();
    buffer_information->getfilled_buffer_ts_diff_max = tmr_get_buffer_.Max_ms();
    Camera::BufferReceived(datastream_pointer, buffer);
}

bool CameraTiming::StartStreamingBGAPI() {
    // try to switch to chunk mode if chunk block FrameID (sendor frame id) supported
    const BGAPI2::String frame_id_node = "FrameID";
    if (camera_pointer_->GetRemoteNodeList()->GetNodePresent(SFNC_CHUNKSELECTOR)) {
        if (camera_pointer_->GetRemoteNode(SFNC_CHUNKSELECTOR)->GetEnumNodeList()->GetNodePresent(frame_id_node)) {
            supports_frameid_sensor_ = true;
        }
    }
    // reset streaming information
    cam_buffer_ts_last_ = 0;
    cam_buffer_ts_ = 0;
    host_buffer_ts_last_ = 0;
    host_buffer_ts_ = 0;
    tmr_get_buffer_.Reset();
    tmr_camera_ts_.Reset();
    tmr_host_ts_.Reset();

    bool result = false;
    tmr_start_.Start();
    result = Camera::StartStreamingBGAPI();
    tmr_start_.Stop();
    return result;
}

void CameraTiming::StopStreamingBGAPI() {
    tmr_stop_.Start();
    Camera::StopStreamingBGAPI();
    tmr_stop_.Stop();
}
