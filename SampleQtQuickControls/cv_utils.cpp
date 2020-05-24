#include "cv_utils.h"
#include <memory>
#include <iostream>
#include <QDebug>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <math.h>
#include <iostream>

using namespace cv;

cvUtils::cvUtils(QObject *parent)
    : QObject(parent)
{

}

QString cvUtils::preprocess_image()
{
    QString msg;

//    Example to test openCV
    Mat output = Mat::zeros(120, 350, CV_8UC3);
    putText(output,
                "Hello World :)",
                Point(15,70),
                FONT_HERSHEY_PLAIN,
                3,
                Scalar(0,255,0),
                4);
    imshow("Output", output);
    waitKey(0);

    msg.append("Pre-process successful");
    return msg;
}
