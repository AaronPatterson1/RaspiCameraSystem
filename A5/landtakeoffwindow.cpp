#include "landtakeoffwindow.h"
#include "ui_landtakeoffwindow.h"

LandTakeoffWindow::LandTakeoffWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LandTakeoffWindow)
{
    ui->setupUi(this);
}

LandTakeoffWindow::~LandTakeoffWindow()
{
    delete ui;
}
