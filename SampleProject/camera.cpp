#include "camera.h"
#include "ui_camera.h"
#include "bgapi2_genicam.hpp"

using namespace BGAPI2;

Camera::Camera(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Camera)
{
    ui->setupUi(this);
}

Camera::~Camera()
{
    delete ui;
}
