/********************************************************************************
** Form generated from reading UI file 'camera.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAMERA_H
#define UI_CAMERA_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>

QT_BEGIN_NAMESPACE

class Ui_Camera
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *cam_data;

    void setupUi(QDialog *Camera)
    {
        if (Camera->objectName().isEmpty())
            Camera->setObjectName(QString::fromUtf8("Camera"));
        Camera->resize(400, 300);
        buttonBox = new QDialogButtonBox(Camera);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        cam_data = new QLabel(Camera);
        cam_data->setObjectName(QString::fromUtf8("cam_data"));
        cam_data->setGeometry(QRect(70, 30, 261, 181));

        retranslateUi(Camera);
        QObject::connect(buttonBox, SIGNAL(accepted()), Camera, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Camera, SLOT(reject()));

        QMetaObject::connectSlotsByName(Camera);
    } // setupUi

    void retranslateUi(QDialog *Camera)
    {
        Camera->setWindowTitle(QApplication::translate("Camera", "Dialog", 0, QApplication::UnicodeUTF8));
        cam_data->setText(QApplication::translate("Camera", "TextLabel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Camera: public Ui_Camera {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAMERA_H
