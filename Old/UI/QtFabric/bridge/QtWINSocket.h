#ifndef QTWINSOCKET_H
#define QTWINSOCKET_H
 
#include <QObject>

class QTcpSocket;

class QtWINSocket : public QObject
{
    Q_OBJECT
public:
    explicit QtWINSocket(QObject *parent = 0);
    void connectToServer(int port);
    void send(std::string message);
signals:
    
public slots:
    void on_connected();
    void on_readyRead();
    void on_disconnected();
private:
    QTcpSocket* socket;
};
 
#endif // QTWINSOCKET_H