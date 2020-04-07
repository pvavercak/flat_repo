#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <QObject>
#include <QSslSocket>
#include <QList>
#include <QSslError>
#include <QDebug>
#include <QVector>
#include <QString>
#include <QByteArray>
#include <QFile>
#include <QImageReader>

#include <regex>
#include <memory>

#include "user.hpp"
#include "UFScanner.h"

class Client : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Client)
public:
    explicit Client(QObject *parent = nullptr);
    ~Client();
    //public methods
    bool isSocketConnected();
    bool connectionInit(const QString &addr, const quint16& port);
    void makeScan();
    bool deleteCurrentlyEnrollingUser();
    bool addFingerFromScanner();
    bool addFingerFromFile(const QString& imagePath);
    void newUser();
    void enrollUser();
    void identifyUser();
    bool processInputFinger();
    bool sendProbeUser(int operation);
    void sendBadMessage();
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
    std::shared_ptr<QSslSocket> m_socket;
    User m_user;
    HUFScanner m_scanner;

    //private methods
    bool checkIp(const QString &receivedIp);
    finger readFingerFromScanner();
    finger readFingerFromImage(QString imagePath);
    void CHECK_ERROR(UFS_STATUS err);
};

#endif // CLIENT_HPP
