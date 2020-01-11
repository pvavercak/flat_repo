#ifndef CLIENT_H
#define CLIENT_H

#include <memory>
#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#define SOCKETSIZE 65000.0
class Client : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Client)
public:
    explicit Client(QObject *parent = nullptr);
    bool checkIp(QString &receivedIp, quint16 receivedPort);
    bool isSocketConnected();
    void writeTemplate(QByteArray& bytesToWrite);
    bool writeHeader(QByteArray& headerToWrite, int len);

signals:
    void connected();
    void loggedIn();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void attemptConnect();
    void attemptToWrite();
public slots:
    void connectToServer();
    void disconnectFromHost();
private slots:
        void onReadyRead();
private:
    QHostAddress address;
    quint16 port;
    std::shared_ptr<QTcpSocket> m_socket;
};

#endif // CLIENT_H
