#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "status.h"
#include "log.h"
#include "report.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Status_clicked()
{
    Status stat;
    stat.setModal(true);
    stat.exec();
}

void MainWindow::on_Log_clicked()
{
    Log log;
    log.setModal(true);
    log.exec();
}

void MainWindow::on_Report_clicked()
{
    report rep;
    rep.setModal(true);
    rep.exec();
}
