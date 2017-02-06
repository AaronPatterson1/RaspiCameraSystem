#include "camerawindow.h"
#include "ui_camerawindow.h"

CameraWindow::CameraWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CameraWindow)
{
    ui->setupUi(this);
}

CameraWindow::~CameraWindow()
{
    delete ui;
}

void CameraWindow::on_comboBox_activated(const QString &arg1)
{

}
