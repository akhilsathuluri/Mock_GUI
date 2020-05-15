/* Copyright 2019-2020 Baumer Optronic */
#pragma once

#include <thread>
#include <vector>
#include <mutex>
#include <list>
#include <string>

#include "bgapi2_genicam/bgapi2_genicam.hpp"
#include "Camera.h"
#include "DoubleBufferHandler.h"
#include "TimingStatistic.h"

// the class encapsulate camera search, capture command and feature command
// it also implements the worker threads
class CameraManager {
// ----
// public functions which are used in the context of the main thread and application threads
// to control the main settings from the user application
// ----
 public:
    // construction / destruction
    CameraManager();
    virtual ~CameraManager();

    // this function starts the init thread to search for connected cameras
    // it also passed the buffer handler
    void Start(DoubleBufferHandler buffer_handler);

    // close all connected cameras and frees all BGAPI resources
    // this function doesn't run in a separate thread
    void Stop();

    // this function initialize the buffer management of all connected cameras
    // it also creates the struct BufferInformation for every camera buffer
    // all cameras use 10 buffers to store the image data
    // the memory for the camera buffers will be allocated inside BGAPI
    // the size of the memory to allocated will determined from the maximum image size
    // this function also starts the worker threads
    void StartCameras();

    // this function stops the worker threads
    // and deinitialize the BGAPI
    void StopCameras();

    // function to check for an initialisation error
    bool IsInitOK() const;

    // returns the number of connected cameras
    size_t NumberOfCameras() const;

    // this function returns the number of images taken by the camera which took the least number of images
    unsigned int CapturedImages();

    // function to return the camera vector
    const std::vector<Camera*>* GetCameraVector() const { return &cameras_; }

    // this function returns all logging messages as one string and clears the message list
    std::string LoggingMessages();

    // returns true, if at least one camera supports the resend statistic
    bool ResendStatisticSupported();

// ----
// public functions which are partly used in the worker threads
// to handle and process the code regarding to main settings
// ----

 public:
    // this function initializes the Baumer GAPI SDK and
    // searches for connected cameras
    // all found cameras will be opened to work with
    void InitializeBGAPI();

    // this function deinitialises the Baumer GAPI SDK and
    // close all connected cameras
    void DeinitializeBGAPI();

    // function to signal an initialisation error while start up
    void SetInitError();

    // function to add a string with logging information to the logging string list
    void AddLoggingMessage(std::string logging_message);

    // this function adds a camera object to the camera vector
    void AddCamera(BGAPI2::Device* camera_pointer);

    // this function set a flag to signal the worker thread to exit
    const bool* FinishWorkerThreads() const { return &finish_worker_threads_; }

 private:
    // pointer to the buffer handler
    DoubleBufferHandler buffer_handler_;
    // flag to signal the worker threads to exit themselves
    bool finish_worker_threads_;
    // initialisation thread
    std::thread init_thread;
    // camera vector holds information regarding synchronisation and
    // the BGAPI camera object
    std::vector<Camera*> cameras_;
    // flag is set for initialisation errors
    bool init_ok_;
    // a list which holds logging strings
    std::list<std::string> logging_list_;
    // a mutex to synchronize the access to the logging list
    std::mutex logging_list_lock_;
};
