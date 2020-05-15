#ifndef CAMERA_H
#define CAMERA_H

#include <QDialog>

namespace Ui {
class Camera;
}

class Camera : public QDialog
{
    Q_OBJECT

public:
    explicit Camera(QWidget *parent = 0);
    ~Camera();

private:
    Ui::Camera *ui;
};

#endif // CAMERA_H
