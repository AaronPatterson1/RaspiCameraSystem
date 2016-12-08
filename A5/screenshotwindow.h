#ifndef SCREENSHOTWINDOW_H
#define SCREENSHOTWINDOW_H

#include <QWidget>

namespace Ui {
class ScreenshotWindow;
}

class ScreenshotWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenshotWindow(QWidget *parent = 0);
    ~ScreenshotWindow();

private:
    Ui::ScreenshotWindow *ui;
};

#endif // SCREENSHOTWINDOW_H
