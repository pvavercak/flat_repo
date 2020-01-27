#ifndef CLIENT_H
#define CLIENT_H

#include <memory>
#include <QObject>
#include <QSslSocket>
#include <QHostAddress>
#include <QFile>
#include <fphandler.h>
#include <QList>
#include <QSslError>
class Client : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Client)
public:
    explicit Client(QObject *parent = nullptr);

    //public methods
    bool isSocketConnected();
    void writeTemplate();       
    bool connectionInit(QString &addr, quint16& port);
    void makeScan();

signals:
    void connected();
    void loggedIn();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void attemptConnect();
    void attemptToWrite();
    void updateLog(QString);
public slots:    
    void disconnectFromHost();
private slots:
        void onReadyRead();
        void onConnectedSlot();
        void onEncryptedSlot();
        void onStateChanged(QAbstractSocket::SocketState state);
        void disconnectedClient();
        void encryptedSlot();
        void onSslErrorSlot(const QList<QSslError> &errorList);
private:
    std::unique_ptr<FpHandler> m_scanner;
    std::shared_ptr<QSslSocket> m_socket;
    QByteArray m_scanData;

    //private methods
    bool checkIp(QString &receivedIp);
};

#endif // CLIENT_H
