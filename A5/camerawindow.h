#ifndef CAMERAWINDOW_H
#define CAMERAWINDOW_H

#include <QWidget>

namespace Ui {
class CameraWindow;
}

class CameraWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CameraWindow(QWidget *parent = 0);
    ~CameraWindow();

private slots:
    void on_comboBox_activated(const QString &arg1);

private:
    Ui::CameraWindow *ui;
};

#endif // CAMERAWINDOW_H
