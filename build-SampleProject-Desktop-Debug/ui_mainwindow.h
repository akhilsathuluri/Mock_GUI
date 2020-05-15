/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QPushButton *Status;
    QPushButton *Report;
    QPushButton *Settings;
    QPushButton *Log;
    QPushButton *Camera;
    QMenuBar *menuBar;
    QMenu *menuMain_menu;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(400, 300);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        Status = new QPushButton(centralWidget);
        Status->setObjectName(QString::fromUtf8("Status"));
        Status->setGeometry(QRect(20, 70, 99, 27));
        Report = new QPushButton(centralWidget);
        Report->setObjectName(QString::fromUtf8("Report"));
        Report->setGeometry(QRect(150, 70, 99, 27));
        Settings = new QPushButton(centralWidget);
        Settings->setObjectName(QString::fromUtf8("Settings"));
        Settings->setGeometry(QRect(270, 70, 111, 27));
        Log = new QPushButton(centralWidget);
        Log->setObjectName(QString::fromUtf8("Log"));
        Log->setGeometry(QRect(20, 130, 99, 27));
        Camera = new QPushButton(centralWidget);
        Camera->setObjectName(QString::fromUtf8("Camera"));
        Camera->setGeometry(QRect(150, 130, 99, 27));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 400, 25));
        menuMain_menu = new QMenu(menuBar);
        menuMain_menu->setObjectName(QString::fromUtf8("menuMain_menu"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuMain_menu->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        Status->setText(QApplication::translate("MainWindow", "Status (\340\244\270\340\245\215\340\244\237\340\245\207\340\244\237\340\244\270)", 0, QApplication::UnicodeUTF8));
        Report->setText(QApplication::translate("MainWindow", "Report (\340\244\260\340\244\277\340\244\252\340\245\213\340\244\260\340\245\215\340\244\237)", 0, QApplication::UnicodeUTF8));
        Settings->setText(QApplication::translate("MainWindow", "Settings(\340\244\270\340\245\207\340\244\237\340\245\215\340\244\207\340\244\250\340\245\215\340\244\227)", 0, QApplication::UnicodeUTF8));
        Log->setText(QApplication::translate("MainWindow", "Log (\340\244\262\340\245\211\340\244\227)", 0, QApplication::UnicodeUTF8));
        Camera->setText(QApplication::translate("MainWindow", "Camera", 0, QApplication::UnicodeUTF8));
        menuMain_menu->setTitle(QApplication::translate("MainWindow", "Main menu", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
