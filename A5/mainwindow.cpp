#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "landtakeoffwindow.h"

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

void MainWindow::on_pushButton_clicked()
{
    LandTakeoffWindow nLTW;

    //nLTW.setModal(true);
    //nLTW.exec();
}
