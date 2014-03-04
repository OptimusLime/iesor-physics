#include "QtWINSocket.h"
#include <QTcpSocket>
#include <QHostAddress>
#include <cstdio>

QtWINSocket::QtWINSocket(QObject *parent) :
    QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()),
            this, SLOT(on_connected()));

    connect(socket, SIGNAL(disconnected()),
    this, SLOT(on_disconnected()));
    connect(socket, SIGNAL(readyRead()),
    this, SLOT(on_readyRead()));

}

void QtWINSocket::on_connected()
{
    printf("Connection established.\n");
    this->send("Hello friend!");
}

void QtWINSocket::on_readyRead()
{
    while(socket->canReadLine())
    {
        QByteArray ba = socket->readLine();

        if(strcmp(ba.constData(), "!exit\n") == 0)
        {
            socket->disconnectFromHost();
            break;
        }
        printf(">> %s", ba.constData());
    }
}

void QtWINSocket::on_disconnected()
{
    printf("Connection disconnected.\n");
    disconnect(socket, SIGNAL(disconnected()));
    disconnect(socket, SIGNAL(readyRead()));
    disconnect(socket, SIGNAL(connected()));
    socket->deleteLater();
}

void QtWINSocket::connectToServer(int port)
{
    socket->connectToHost(QHostAddress::LocalHost, port);
}

void QtWINSocket::send(std::string socketMessage)
{
    socket->write(socketMessage.c_str(), socketMessage.length());
    socket->flush();
}
