/* Copyright 2019-2020 Baumer Optronic */
#include <iostream>
#include <sstream>
#include <limits>
#include <iomanip>
#include <string>
#include "Camera.h"
#include "DoubleBufferHandler.h"
#include "BufferInformation.h"

Camera::Camera(BGAPI2::Device* bgapi_device)
: command_capture_(false)
, capture_active_(false)
, number_of_images_(0)
, number_of_captured_images_(0)
, number_of_incomplete_images_(0)
, command_feature_(false)
, exposure_time_(0)
, camera_pointer_(bgapi_device)
, datastream_pointer_(nullptr)
, chunk_was_active_(false)
, resend_statistic_supported_(false) {
    camera_pointer_->Open();
    Initialize();
}

Camera::~Camera() {
    Deinitialize();
    camera_pointer_->Close();
}

void Camera::Initialize() {
    try {
        camera_name_ = camera_pointer_->GetModel().get();
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        camera_name_ = "ModelNotAvail";
        AddLoggingMessage("Camera initialize finished unexpected!");
        std::stringstream strstream;
        strstream << "Error in function: " << ex.GetFunctionName() << std::endl
            << "Error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
    }
#ifdef _DEBUG
    try {
        if (camera_pointer_->GetRemoteNodeList()->GetNodePresent("DeviceLinkHeartbeatMode")) {
            camera_pointer_->GetRemoteNode("DeviceLinkHeartbeatMode")->SetValue("Off");
        }
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        AddLoggingMessage("Disabel heartbeat failed! Maybe possible connection loss to camera.");
        std::stringstream strstream;
        strstream << "error in function: " << ex.GetFunctionName() << std::endl
            << "error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
    }
#endif
    try {
        if (camera_pointer_->GetRemoteNodeList()->GetNodePresent(SFNC_CHUNKMODEACTIVE)) {
            chunk_was_active_ = camera_pointer_->GetRemoteNode(SFNC_CHUNKMODEACTIVE)->GetBool();
        }
        if (camera_pointer_->GetRemoteNodeList()->GetNodePresent(SFNC_CHUNKSELECTOR)) {
            for (BGAPI2::NodeMap::iterator iter =
                camera_pointer_->GetRemoteNode(SFNC_CHUNKSELECTOR)->GetEnumNodeList()->begin();
                iter != camera_pointer_->GetRemoteNode(SFNC_CHUNKSELECTOR)->GetEnumNodeList()->end();
                iter++) {
                BGAPI2::Node * chunknode_name = (*iter).second.second;
                std::string chunk_name = chunknode_name->GetValue().get();
                camera_pointer_->GetRemoteNode(SFNC_CHUNKSELECTOR)->SetValue(chunk_name.c_str());
                camera_pointer_->GetRemoteNode(SFNC_CHUNKENABLE)->SetBool(true);
            }
            if (!chunk_was_active_) {
                camera_pointer_->GetRemoteNode(SFNC_CHUNKMODEACTIVE)->SetBool(true);
            }
        }
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        AddLoggingMessage("Camera initialize finished unexpected!");
        std::stringstream strstream;
        strstream << "Error in function: " << ex.GetFunctionName() << std::endl
            << "Error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
    }

    try {
        BGAPI2::DataStreamList *datastream_list = camera_pointer_->GetDataStreams();
        datastream_list->Refresh();
        if (datastream_list->size() > 0) {
            datastream_pointer_ = datastream_list->begin()->second;
            datastream_pointer_->Open();
            if (datastream_pointer_->GetNodeList()->GetNodePresent("ResendRequests") == true) {
                if (datastream_pointer_->GetNode("ResendRequests")->IsReadable()) {
                    resend_statistic_supported_ = true;
                }
            }
            datastream_pointer_->Close();
        }
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        AddLoggingMessage("Camera initialize finished unexpected!");
        std::stringstream strstream;
        strstream << "Error in function: " << ex.GetFunctionName() << std::endl
            << "Error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
    }
}

// this function reverts the preparation steps
void Camera::Deinitialize() {
    if (!chunk_was_active_) {
        try {
            if (camera_pointer_->GetRemoteNodeList()->GetNodePresent(SFNC_CHUNKSELECTOR)) {
                for (BGAPI2::NodeMap::iterator iter =
                    camera_pointer_->GetRemoteNode(SFNC_CHUNKSELECTOR)->GetEnumNodeList()->begin();
                    iter != camera_pointer_->GetRemoteNode(SFNC_CHUNKSELECTOR)->GetEnumNodeList()->end();
                    iter++) {
                    BGAPI2::Node * chunk_name = (*iter).second.second;
                    camera_pointer_->GetRemoteNode(SFNC_CHUNKSELECTOR)->SetValue(chunk_name->GetValue());
                    camera_pointer_->GetRemoteNode(SFNC_CHUNKENABLE)->SetBool(false);
                }
                camera_pointer_->GetRemoteNode(SFNC_CHUNKMODEACTIVE)->SetBool(false);
            }
        }
        catch (BGAPI2::Exceptions::IException& ex) {
            AddLoggingMessage("Camera deinitialize finished unexpected!");
            std::stringstream strstream;
            strstream << "Error in function: " << ex.GetFunctionName() << std::endl
                << "Error description: " << ex.GetErrorDescription() << std::endl;
            AddLoggingMessage(strstream.str());
        }
    }
#ifdef _DEBUG
    try {
        if (camera_pointer_->GetRemoteNodeList()->GetNodePresent("DeviceLinkHeartbeatMode")) {
            camera_pointer_->GetRemoteNode("DeviceLinkHeartbeatMode")->SetValue("On");
        }
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        AddLoggingMessage("Reset heartbeat settings failed! Maybe possible connection loss to camera.");
        std::stringstream strstream;
        strstream << "error in function: " << ex.GetFunctionName() << std::endl
            << "error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
    }
#endif
}

// this function uses a mutex to protect message list for parallel access
bool Camera::ExposureFeatureBGAPI() {
    try {
        BGAPI2::Node* exposure_node = camera_pointer_->GetRemoteNode("ExposureTime");
        // check if the feature is writeable and write new value
        if (exposure_node->IsWriteable()) {
            exposure_node->SetDouble(exposure_time_);
        }
        return true;
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        AddLoggingMessage("Command thread finished unexpected for camera " + std::string(camera_name_) + "!");
        std::stringstream strstream;
        strstream << "Error in function: " << ex.GetFunctionName() << std::endl
            << "Error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
        return false;
    }
}

// the mutex protects the command_feature_ flag
// against parallel access from function CommandExposure
void Camera::StartFeatureCommandExposure(double exposure_time) {
    std::lock_guard<std::mutex> lock(feature_command_lock_);
    exposure_time_ = exposure_time;
    command_feature_ = true;
}

// this function works like a auto reset and deactivates the exposure command immediately
// the mutex protects the command_feature_ flag
// against parallel access from function StartFeatureCommandExposure
bool Camera::IsCommandExposureActivatedAndReset() {
    std::lock_guard<std::mutex> lock(feature_command_lock_);
    if (command_feature_) {
        command_feature_ = false;
        return true;
    } else {
        return false;
    }
}

// the mutex protects the command_capture_ flag
// against parallel access from function CommandCapture
void Camera::StartCaptureCommand(unsigned int images) {
    std::lock_guard<std::mutex> lock(capture_command_lock_);
    number_of_images_ = images;
    command_capture_ = true;
}

// this function works like a auto reset and deactivates the capture command immediately
// the mutex protects the command_capture_ flag
// against parallel access from function StartCaptureCommand
bool Camera::IsCommandCaptureActivatedAndReset() {
    std::lock_guard<std::mutex> lock(capture_command_lock_);
    if (command_capture_) {
        command_capture_ = false;
        return true;
    } else {
        return false;
    }
}

// this function opens always the first data stream,
// other possible data streams will be ignored,
// because they are not relevant
// this function creates the camera buffer objects and passes a new instance of BufferInformation structure,
// the camera buffers and BufferInformation structures will be deleted in function DeinitializeBGAPIBufferManagement
void Camera::InitializeBGAPIBufferManagement() {
    // at first a refresh of data stream is needed to fill the stream list
    camera_pointer_->GetDataStreams()->Refresh();
    // set buffer handler to every device
    buffer_handler_ = buffer_handler_;
    datastream_pointer_->Open();

    BGAPI2::BufferList *buffer_list = datastream_pointer_->GetBufferList();
    BGAPI2::Buffer *buffer = nullptr;
    for (int i = 0; i < 10; i++) {
        // create the buffer object and pass a new instance of BufferInformation,
        // which holds additional buffer information
        buffer = new BGAPI2::Buffer(new BufferInformation());
        // announce the camera buffer to the data streams buffer list
        // at this point the camera buffer will be allocated
        buffer_list->Add(buffer);
        // move the camera buffer into the input queue
        // this makes the buffer available for image capture
        buffer->QueueBuffer();
    }
}

// the BufferInformation objects and the camera buffers created in StartCameras will be deleted in this function
void Camera::DeinitializeBGAPIBufferManagement() {
    BGAPI2::BufferList *buffer_list = datastream_pointer_->GetBufferList();
    // returns the buffer to the user application
    buffer_list->DiscardAllBuffers();
    // revoke and delete the image buffers
    while (buffer_list->size() > 0) {
        BGAPI2::Buffer* buffer = buffer_list->begin()->second;
        // delete buffer information
        delete reinterpret_cast<BufferInformation*>(buffer->GetUserObj());
        // revoke the buffer from the buffer list
        buffer_list->RevokeBuffer(buffer);
        // delete the buffer itself
        delete buffer;
    }
    // close the data stream
    datastream_pointer_->Close();
}

// this function starts the data stream on host and on camera
// and implements a loop to capture the desired number of images
// it also exchange the received buffer with the buffer handler
bool Camera::CaptureBGAPIImages(const bool * abort_flag, unsigned int number_of_images) {
    bool return_flag = true;
    try {
        number_of_images_ = number_of_images;
        return_flag = StartStreamingBGAPI();
        if (return_flag) {
            // loop over the number of images to capture
            do {
                // fetch the buffers
                BGAPI2::Buffer* buffer = datastream_pointer_->GetFilledBuffer(10);
                if (buffer) {
                    BufferReceived(datastream_pointer_, buffer);
                    SaveLatestBufferInformation(reinterpret_cast<BufferInformation*>(buffer->GetUserObj()));
                    number_of_captured_images_++;
                    buffer_handler_.PushBuffer(buffer);
                }
                // break the thread also at this point,
                // to avoid long shut down times in case you capture a huge number of images
                if (*abort_flag) {
                    return_flag = false;
                }
            } while (number_of_captured_images_ < number_of_images_ && return_flag);
        }
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        AddLoggingMessage("streaming thread finished unexpected for camera " + std::string(camera_name_) + "!");
        std::stringstream strstream;
        strstream << "error in function: " << ex.GetFunctionName() << std::endl
            << "error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
        return_flag = false;
    }
    try {
        // stop acquisition on host and device side
        StopStreamingBGAPI();
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        AddLoggingMessage("stop streaming failed" + std::string(camera_name_) + "!");
        std::stringstream strstream;
        strstream << "error in function: " << ex.GetFunctionName() << std::endl
            << "error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
        return_flag = false;
    }
    return return_flag;
}

// this function starts the streaming on host and on camera
// this function may throw an exception, the exception will be caught by the caller
bool Camera::StartStreamingBGAPI() {
    // initialize image counter
    number_of_captured_images_ = 0;
    // initialize incomplete image counter
    number_of_incomplete_images_ = 0;
    // initialize the buffer mode object
    buffer_handler_.Init();
    // at first make sure the buffer list is ok -> all buffers has to be in the input queue
    datastream_pointer_->GetBufferList()->DiscardAllBuffers();
    datastream_pointer_->GetBufferList()->FlushAllToInputQueue();
    // reset all relevant buffer information collected by a previous run
    ResetBufferInformation();
    // start the acquisition on the host side with the selected number of images
    datastream_pointer_->StartAcquisition(number_of_images_);
    // reset frame counter on device side, if supported
    const BGAPI2::String frame_counter_node = "FrameCounter";
    if (camera_pointer_->GetRemoteNodeList()->GetNodePresent(frame_counter_node)) {
        if (camera_pointer_->GetRemoteNode(frame_counter_node)->IsWriteable()) {
            camera_pointer_->GetRemoteNode(frame_counter_node)->SetInt(0);
        }
    }
    // start the acquisition on the device side
    // to work with this example the device must be in free running mode
    if (camera_pointer_->GetRemoteNode(SFNC_ACQUISITION_START)->IsWriteable()) {
        camera_pointer_->GetRemoteNode(SFNC_ACQUISITION_START)->Execute();
        capture_active_ = true;
        return true;
    } else {
        AddLoggingMessage(
            "Streaming Acquisition not started for camera " +
            std::string(camera_name_) +
            "! The feature AcquisitionStart is not writeable");
        return false;
    }
}

// this function may throw an exception, the exception will be caught by the caller
void Camera::StopStreamingBGAPI() {
    capture_active_ = false;
    try {
        // stop acquisition on host and device side
        camera_pointer_->GetRemoteNode(SFNC_ACQUISITION_STOP)->Execute();
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        AddLoggingMessage("Execute AcquisitionStop faild. Maybe the connection to the camera "
            + std::string(camera_name_) + " is lost!");
        std::stringstream strstream;
        strstream << "error in function: " << ex.GetFunctionName() << std::endl
            << "error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
    }
    datastream_pointer_->StopAcquisition();
}

// this function may throw an exception, the exception will be caught by the caller
void Camera::BufferReceived(BGAPI2::DataStream* datastream_pointer, BGAPI2::Buffer *buffer) {
    BufferInformation * buffer_information = reinterpret_cast<BufferInformation*>(buffer->GetUserObj());
    buffer_information->valid = true;
    buffer_information->bufferid = buffer->GetID().get();
    buffer_information->is_imcomplete = buffer->GetIsIncomplete();
    if (buffer_information->is_imcomplete) {
        number_of_incomplete_images_++;
    }
    buffer_information->resend_valid = false;
    if (datastream_pointer->GetNodeList()->GetNodePresent("ResendRequests") == true) {
        if (datastream_pointer->GetNode("ResendRequests")->IsReadable()) {
            buffer_information->resend_requests = datastream_pointer->GetNode("ResendRequests")->GetInt();
            buffer_information->resend_valid = true;
        }
    }
    if (datastream_pointer->GetNodeList()->GetNodePresent("PacketResendRequestSingle") == true) {
        if (datastream_pointer->GetNode("PacketResendRequestSingle")->IsReadable()) {
            buffer_information->resend_requests_single =
                datastream_pointer->GetNode("PacketResendRequestSingle")->GetInt();
            buffer_information->resend_valid = true;
        }
    }
    if (datastream_pointer->GetNodeList()->GetNodePresent("PacketResendRequestRange") == true) {
        if (datastream_pointer->GetNode("PacketResendRequestRange")->IsReadable()) {
            buffer_information->resend_requests_range =
                datastream_pointer->GetNode("PacketResendRequestRange")->GetInt();
            buffer_information->resend_valid = true;
        }
    }
}

// this function protects the logging_list_ against parallel access from function LoggingString
void Camera::AddLoggingMessage(std::string logging_message) {
    std::lock_guard<std::mutex> lock(logging_list_lock_);
    if (logging_list_.size() == 0) {
        logging_list_.push_back(camera_name_ + ":");
    }
    logging_list_.push_back(logging_message);
}

// this function returns all logging messages as one string and clears the message list
std::string Camera::LoggingMessages() {
    std::lock_guard<std::mutex> lock(logging_list_lock_);
    std::stringstream logging_stream;
    while (logging_list_.size() > 0) {
        logging_stream << logging_list_.front() << std::endl;
        logging_list_.pop_front();
    }
    return logging_stream.str();
}

std::string Camera::GetName() {
    return camera_name_;
}

bool Camera::GetExposureTime(double *exposure_time) {
    try {
        BGAPI2::Node* exposure_node = camera_pointer_->GetRemoteNode(SFNC_EXPOSURETIME);
        // check if the feature is readable
        if (exposure_node->IsReadable()) {
            *exposure_time = exposure_node->GetDouble();
            return true;
        }
        return false;
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        std::stringstream strstream;
        strstream << "Read " << std::string(SFNC_EXPOSURETIME)
            << " on camera " << std::string(camera_name_) << std::endl
            << "Error in function: " << ex.GetFunctionName() << std::endl
            << "Error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
        return false;
    }
}

bool Camera::GetRoiOffsetX(uint64_t *roi) {
    try {
        BGAPI2::Node* offsetx_node = camera_pointer_->GetRemoteNode(SFNC_OFFSETX);
        // check if the features are readable
        if (offsetx_node->IsReadable()) {
            *roi = offsetx_node->GetInt();
            return true;
        }
        return false;
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        std::stringstream strstream;
        strstream << "Read ROI failed on camera " << std::string(camera_name_)
            << "Error in function: " << ex.GetFunctionName() << std::endl
            << "Error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
        return false;
    }
}

bool Camera::GetRoiOffsetY(uint64_t *roi) {
    try {
        BGAPI2::Node* offsety_node = camera_pointer_->GetRemoteNode(SFNC_OFFSETY);
        // check if the features are readable
        if (offsety_node->IsReadable()) {
            *roi = offsety_node->GetInt();
            return true;
        }
        return false;
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        std::stringstream strstream;
        strstream << "Read ROI failed on camera " << std::string(camera_name_)
            << "Error in function: " << ex.GetFunctionName() << std::endl
            << "Error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
        return false;
    }
}

bool Camera::GetRoiWidth(uint64_t *roi) {
    try {
        BGAPI2::Node* width_node = camera_pointer_->GetRemoteNode(SFNC_WIDTH);
        // check if the features are readable
        if (width_node->IsReadable()) {
            *roi = width_node->GetInt();
            return true;
        }
        return false;
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        std::stringstream strstream;
        strstream << "Read ROI failed on camera " << std::string(camera_name_)
            << "Error in function: " << ex.GetFunctionName() << std::endl
            << "Error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
        return false;
    }
}

bool Camera::GetRoiHeight(uint64_t *roi) {
    try {
        BGAPI2::Node* height_node = camera_pointer_->GetRemoteNode(SFNC_HEIGHT);
        // check if the features are readable
        if (height_node->IsReadable()) {
            *roi = height_node->GetInt();
            return true;
        }
        return false;
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        std::stringstream strstream;
        strstream << "Read ROI failed on camera " << std::string(camera_name_)
            << "Error in function: " << ex.GetFunctionName() << std::endl
            << "Error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
        return false;
    }
}

uint64_t Camera::GetResendRequests() {
    try {
        if (datastream_pointer_->GetNodeList()->GetNodePresent("ResendRequests") == true) {
            if (datastream_pointer_->GetNode("ResendRequests")->IsReadable()) {
                return datastream_pointer_->GetNode("ResendRequests")->GetInt();
            }
        }
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        AddLoggingMessage("GetResendRequests failed.");
        std::stringstream strstream;
        strstream << "error in function: " << ex.GetFunctionName() << std::endl
            << "error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
    }
    return 0;
}

uint64_t Camera::GetResendSingleRequests() {
    try {
        if (datastream_pointer_->GetNodeList()->GetNodePresent("PacketResendRequestSingle") == true) {
            if (datastream_pointer_->GetNode("PacketResendRequestSingle")->IsReadable()) {
                return datastream_pointer_->GetNode("PacketResendRequestSingle")->GetInt();
            }
        }
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        AddLoggingMessage("GetResendSingleRequests failed.");
        std::stringstream strstream;
        strstream << "error in function: " << ex.GetFunctionName() << std::endl
            << "error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
    }
    return 0;
}

uint64_t Camera::GetResendRangeRequests() {
    try {
        if (datastream_pointer_->GetNodeList()->GetNodePresent("PacketResendRequestRange") == true) {
            if (datastream_pointer_->GetNode("PacketResendRequestRange")->IsReadable()) {
                return datastream_pointer_->GetNode("PacketResendRequestRange")->GetInt();
            }
        }
    }
    catch (BGAPI2::Exceptions::IException& ex) {
        AddLoggingMessage("GetResendRangeRequests failed.");
        std::stringstream strstream;
        strstream << "error in function: " << ex.GetFunctionName() << std::endl
            << "error description: " << ex.GetErrorDescription() << std::endl;
        AddLoggingMessage(strstream.str());
    }
    return 0;
}

void Camera::ResetBufferInformation() {
    latest_buffer_information_.Reset();
    BGAPI2::BufferList *buffer_list = datastream_pointer_->GetBufferList();
    for (BGAPI2::BufferList::iterator buf_iter = buffer_list->begin();
        buf_iter != buffer_list->begin();
        buf_iter++) {
        BufferInformation * buf_info = reinterpret_cast<BufferInformation*>((*buf_iter)->second->GetUserObj());
        buf_info->Reset();
    }
}

void Camera::SaveLatestBufferInformation(BufferInformation* buffer_info) {
    std::lock_guard<std::mutex> lock(buffer_information_lock_);
    latest_buffer_information_ = *buffer_info;
}

void Camera::GetLatestBufferInformation(BufferInformation* buffer_info) {
    std::lock_guard<std::mutex> lock(buffer_information_lock_);
    *buffer_info = latest_buffer_information_;
}
