#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "server.h"

namespace Ui {
class MainWindow;
}
class Server;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void addMessage(QString Msg);
    Server* m_pBoxServer;

private slots:
    void on_portConnect_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
