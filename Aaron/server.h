#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include "mainwindow.h"
class MainWindow;

class Server : public QTcpServer
{
public:
    Server();
    ~Server();
    Q_OBJECT

public:
    Server(MainWindow *pServer, QObject *parent=0);
    MainWindow* m_pWindow;
private slots:
    void readyRead();
    void disconnected();

protected:
    void incomingConnection(int socketfd);
private:
    QSet<QTcpSocket*> clients;
};

#endif // SERVER_H
