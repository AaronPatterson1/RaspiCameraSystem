#ifndef LANDTAKEOFFWINDOW_H
#define LANDTAKEOFFWINDOW_H

#include <QWidget>

namespace Ui {
class LandTakeoffWindow;
}

class LandTakeoffWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LandTakeoffWindow(QWidget *parent = 0);
    ~LandTakeoffWindow();

private:
    Ui::LandTakeoffWindow *ui;
};

#endif // LANDTAKEOFFWINDOW_H
