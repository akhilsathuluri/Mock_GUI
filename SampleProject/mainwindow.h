#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_Status_clicked();

    void on_Log_clicked();

    void on_Report_clicked();

    void on_Log_2_clicked();

    void on_camera_clicked();

    void on_Camera_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
