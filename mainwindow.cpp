#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_pBoxServer = new Server(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_portConnect_clicked()
{
   m_pBoxServer= new Server(this);

    bool success = m_pBoxServer->listen(QHostAddress::Any, quint16(ui->Port->toPlainText().toInt()));
    if(!success)
    {
        addMessage("Server failed...");

    }
    else
    {
        addMessage("Drone started. Waiting for controller...");
    }
}

void MainWindow::addMessage(QString Msg)
{

    ui->Status->setText(Msg);

}
