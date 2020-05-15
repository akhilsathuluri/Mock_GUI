// HelperFunctions.h
#ifndef SHARED_HELPERFUNCTIONS_H_
#define SHARED_HELPERFUNCTIONS_H_

#include <string>
#include <vector>
#if USE_OPENCV
#   include "opencv2/opencv.hpp"
#   include "opencv2/highgui/highgui.hpp"

#if     USE_OCL_COMPONENT == 3
#           include "opencv2/core/ocl.hpp"
#elif   USE_OCL_COMPONENT == 2
#           include "opencv2/ocl/ocl.hpp"
#endif  // USE_OCL_COMPONENT
#endif  // USE_OPENCV

#if defined(__cplusplus) && __cplusplus >= 199711L && (!_MSC_VER || _MSC_VER >= 1600) // C++11 and greater 201103L?
#   define WITH_REGEX
#endif

// ---------------------------------------------------------------------------------------------------------------------
typedef std::vector<std::string> stringvec;

/** Write a Title line in (std)-output with one marker line below */
extern void writeHeadLine(const std::string& sHeadLine, const char c_cChar = '*');
/** Write a Title line in (std)-output with one marker line below and one above */
extern void writeHeader1(const std::string& sHeadLine, const char c_cChar = '#');

/** read all filenames from a given directory in a string list - with an optional filter */
extern void readDirectory(const std::string& sDirName, const std::string& sFilter, stringvec& v);

// ---------------------------------------------------------------------------------------------------------------------
#if USE_OPENCV
/** Creates a single colored File 'ExampleXXX.jpg' in a given directory with size frameSize and even color with
    Scalar color and number iNumber*/
extern void createColorImage(const std::string& sPathName, const cv::Size& frameSize, const cv::Scalar& color,
                      const int& iNumber);
/** Creates a some colored Files with function createColorImage in a given directory with size frameSize with changing
    color */
extern int createExampleImages(const std::string& sPathName, const cv::Size& frameSize);
#endif  // USE_OPENCV

#endif  // SHARED_HELPERFUNCTIONS_H_
