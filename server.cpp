#include "server.h"
#include <QTcpSocket>
#include <windows.h>

Server::Server(MainWindow* pServer, QObject *parent) : QTcpServer(parent)
{
    m_pWindow = pServer;
}

void Server::incomingConnection(int socketfd)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketfd);
    clients.insert(client);

    m_pWindow->addMessage("Controller is connected to drone. ");

    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void Server::readyRead()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    QString line = QString::fromUtf8(client->readLine()).trimmed();
   if (!line.contains("Error:")){
        QChar number = line[line.length() - 1];
        line = line.left(line.length() - 1);
        QString message = "The drone has moved " + line + "for " + number + " seconds.";
        m_pWindow->addMessage(message);
    }
    else
        m_pWindow->addMessage(line);
}

void Server::disconnected()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    qDebug() << "Client disconnected:" << client->peerAddress().toString();

    clients.remove(client);

}
Server::~Server(){ }
