#include "screenshotwindow.h"
#include "ui_screenshotwindow.h"

ScreenshotWindow::ScreenshotWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScreenshotWindow)
{
    ui->setupUi(this);
}

ScreenshotWindow::~ScreenshotWindow()
{
    delete ui;
}
