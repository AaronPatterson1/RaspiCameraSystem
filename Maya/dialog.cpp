#include "dialog.h"
#include "ui_dialog.h"
#include <fstream>

#include <QMessageBox>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    m_pClientSocket = new QTcpSocket(this);
    ui->setupUi(this);
    isMoving = false;
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButtonConnect_clicked()
{
    m_pClientSocket->connectToHost(ui->textEditIP->toPlainText(),quint16(5002) );
    connect(m_pClientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));

}

void Dialog::on_pushButtonSend_clicked()
{
    ui->pushButtonSend->setEnabled(false);
    if (isMoving){
         m_pClientSocket->write(QString("Error: Movement command is already taking place.").toUtf8());
    }
    QString message = ui->textEditMessage->toPlainText().trimmed() + " " + QString::number(ui->numSeconds->value());
    if (numLines==0){
        if (!(message.contains("up"))){
            m_pClientSocket->write(QString("Error: Drone is grounded.").toUtf8());
        }
        else
        {
            m_pClientSocket->write(QString(message + " ").toUtf8());

            lines[numLines] = message;
            numLines++;
        }
    }
    else if (!(message.contains("up")) && !(message.contains("down")) && !(message.contains("right")) && !(message.contains("left")) && !(message.contains("forward")) && !(message.contains("backward")))
        m_pClientSocket->write(QString("Error: Movement command is invalid.").toUtf8());
    else
    {
        m_pClientSocket->write(QString(message + " ").toUtf8());

        lines[numLines] = message;
        numLines++;
    }
    ui->pushButtonSend->setEnabled(true);

    ui->textEditMessage->clear();

    ui->textEditMessage->setFocus();
}

void Dialog::on_pushButtonLOG_clicked()
{
    std::ofstream myfile ("history.log");
    if(myfile.is_open())
    {
        for(int i = 0; i < numLines; i++)
        {
           myfile << lines[i].toStdString() << '\n';
        }
    }
}


void Dialog::displayError ( QAbstractSocket::SocketError socketError )
{
    switch (socketError) {
         case QAbstractSocket::RemoteHostClosedError:
             break;
         case QAbstractSocket::HostNotFoundError:
             QMessageBox::information(this, tr("Fortune Client"),
                                      tr("The host was not found. Please check the "
                                         "host name and port settings."));
             break;
         case QAbstractSocket::ConnectionRefusedError:
             QMessageBox::information(this, tr("Fortune Client"),
                                      tr("The connection was refused by the peer. "
                                         "Make sure the fortune server is running, "
                                         "and check that the host name and port "
                                         "settings are correct."));
             break;
         default:
             QMessageBox::information(this, tr("Fortune Client"),
                                      tr("The following error occurred: %1.")
                                      .arg(m_pClientSocket->errorString()));
         }


}

