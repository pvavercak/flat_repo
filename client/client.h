#ifndef CLIENT_H
#define CLIENT_H

#include <memory>
#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <fphandler.h>
class Client : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Client)
public:
    explicit Client(QObject *parent = nullptr);

    //public methods
    bool isSocketConnected();
    void writeTemplate();
    bool writeHeader(QByteArray& headerToWrite, int len);
    bool connectionInit(QString &addr, quint16& port);
    void makeScan();

signals:
    void connected();
    void loggedIn();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void attemptConnect();
    void attemptToWrite();
public slots:    
    void disconnectFromHost();
private slots:
        void onReadyRead();
        void onStateChanged(QAbstractSocket::SocketState state);
        void disconnectedClient();
private:
    std::unique_ptr<FpHandler> m_scanner;
    std::shared_ptr<QTcpSocket> m_socket;
    QByteArray m_scanData;

    //private methods
    bool checkIp(QString &receivedIp);
};

#endif // CLIENT_H
