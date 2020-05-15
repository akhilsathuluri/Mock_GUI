/* Copyright 2019-2020 Baumer Optronic */
#pragma once

#include <thread>
#include <vector>
#include <mutex>
#include <string>
#include <list>

#include "bgapi2_genicam/bgapi2_genicam.hpp"
#include "DoubleBufferHandler.h"
#include "BufferInformation.h"

// the class represents a BGAPI camera object and additional camera specific information
class Camera {
 public:
    explicit Camera(BGAPI2::Device* bgapi_device);
    virtual ~Camera();

    // this function activates the feature command
    // the feature command use the ExposureTime feature of the camera
    // the parameter passes the new exposure time to be written to the camera
    virtual void StartFeatureCommandExposure(double exposure_time);

    // this function activates the capture command
    // the parameter passes the number of images to be captured
    virtual void StartCaptureCommand(unsigned int images);

    // this function checks, if the capture command was activated
    // in this case the capture command is automatically reset
    virtual bool IsCommandCaptureActivatedAndReset();

    // this function checks, if the feature command was activated
    // in this case the feature command is automatically reset
    virtual bool IsCommandExposureActivatedAndReset();

    // this function writes the desired exposure time to the passed camera
    virtual bool ExposureFeatureBGAPI();

    // this function makes all necessary preparations on the buffer management to be able to capture images with BGAPI
    // it opens the data stream and creates the camera buffers
    virtual void InitializeBGAPIBufferManagement();

    // this function clears the buffer management and
    // deletes the camera buffers
    virtual void DeinitializeBGAPIBufferManagement();

    // this function capture the desired number of images from the passed camera
    // and store some additional information (BufferInformation struct)
    virtual bool CaptureBGAPIImages(const bool * abort_flag, unsigned int number_of_images);

    // this function starts the streaming for the passed camera
    virtual bool StartStreamingBGAPI();

    // this function stops the streaming for the passed camera
    virtual void StopStreamingBGAPI();

    // this function fill the BufferInformation struct and can be used for buffer specific actions
    virtual void BufferReceived(BGAPI2::DataStream* datastream_pointer, BGAPI2::Buffer *buffer);

    // function to add a string with logging information to the logging string list
    virtual void AddLoggingMessage(std::string logging_message);

    // this function returns all logging messages as one string and clears the message list
    virtual std::string LoggingMessages();

    // native camera pointer
    virtual BGAPI2::Device* GetBGAPIPointer() { return camera_pointer_; }

    // returns true if the camera is capturing images
    virtual bool CaptureActive() { return capture_active_; }

    // returns the number of captured images since start acquisition
    virtual uint64_t GetCapturedImages() { return number_of_captured_images_; }

    // returns the number of incomplete received images
    virtual uint64_t GetIncompleteImages() { return number_of_incomplete_images_; }

    // returns the camera name
    virtual std::string GetName();

    // returns the exposure time
    virtual bool GetExposureTime(double *exposure_time);

    // returns the roi offset in x direction
    virtual bool GetRoiOffsetX(uint64_t *roi);

    // returns the roi offset in y direction
    virtual bool GetRoiOffsetY(uint64_t *roi);

    // returns the width of the roi
    virtual bool GetRoiWidth(uint64_t *roi);

    // returns the height of the roi
    virtual bool GetRoiHeight(uint64_t *roi);

    // returns true if the resend statistic is supported
    virtual bool IsResendStatisticSupported() { return resend_statistic_supported_; }

    // returns the number of retransmitted image data blocks
    virtual uint64_t GetResendRequests();

    // returns the number of single retransmitted image data blocks
    virtual uint64_t GetResendSingleRequests();

    // returns the number of retransmitted image data blocks which were requested as range
    virtual uint64_t GetResendRangeRequests();

    // returns the buffer information of the latest received image
    virtual void GetLatestBufferInformation(BufferInformation* buffer_info);

 private:
    // this function makes some preparation steps
    // e.g. enable chunk mode, disable heartbeat for GEV cameras in debug mode
    virtual void Initialize();

    // this function reverts the preparation steps
    virtual void Deinitialize();

    // this function initialize the BufferInformation struct for each buffer object in the buffer list
    // used directly before the streaming starts
    virtual void ResetBufferInformation();

    // this function store the buffer information struc of the latest captured buffer
    virtual void SaveLatestBufferInformation(BufferInformation* buffer_info);

 public:
    // buffer exchange strategy (double buffer)
    DoubleBufferHandler buffer_handler_;
    // a worker thread that captures images from BGAPI
    std::thread capture_thread_;
    // a worker thread that control camera features from BGAPI
    std::thread feature_thread_;
    // a flag which controls the capture command
    bool command_capture_;
    // a flag which shows if the capture command is active
    bool capture_active_;
    // number of images to capture
    unsigned int number_of_images_;
    // number of currently captured images
    unsigned int number_of_captured_images_;
    // number of incomplete images
    unsigned int number_of_incomplete_images_;
    // a flag which controls the feature command
    bool command_feature_;
    // exposure time to be set
    double exposure_time_;

 private:
    // BGAPI device name
    std::string camera_name_;
    // BGAPI camera object
    BGAPI2::Device* camera_pointer_;
    // BGAPI data stream object
    BGAPI2::DataStream* datastream_pointer_;
    // mutex to synchronize the access to capture command flag of all cameras
    std::mutex capture_command_lock_;
    // mutex to synchronize the access to feature command flag of all cameras
    std::mutex feature_command_lock_;
    // a list which holds logging strings
    std::list<std::string> logging_list_;
    // a mutex to synchronize the access to the logging list
    std::mutex logging_list_lock_;
    // flag which indicated if the chunk mode was active at initialization
    bool chunk_was_active_;
    // flag which indicated if a resend statistic is supported, mainly used by GigE Vision cameras
    bool resend_statistic_supported_;
    // variable to save the latest BufferInformation, can be used if no more buffers are captured
    BufferInformation latest_buffer_information_;
    // lock the latest buffer information
    std::mutex buffer_information_lock_;
};
