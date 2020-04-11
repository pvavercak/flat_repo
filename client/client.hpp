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
#include <QFileInfo>
#include <QDir>
#include <QImageReader>
#include <QRegularExpression>
#include <QHostAddress>

#include <memory>

#include "user.hpp"
#include "UFScanner.h"

class Client : public QObject
{
  Q_OBJECT
private:
  std::shared_ptr<QSslSocket> m_socket;
  std::shared_ptr<User> m_user;
  HUFScanner m_scanner;

  //private methods
  bool checkIp(const QString &receivedIp);
  QImage readFingerFromScanner();
  QImage readFingerFromImage(const QString& imagePath);
  quint8 readFingersFromDirectory(const QFileInfo &fileInfo, QVector<QImage> *outImages);
  bool isImageFormatSupported(const QString& format);
  void UFS_CHECK_ERROR(const UFS_STATUS& err);
  bool sendProbeUser(int operation);

public:
  explicit Client(QObject *parent = nullptr);
  ~Client();
  //public methods
  bool connectionInit(const QString &addr, const quint16& port);
  void makeScan();
  bool deleteCurrentlyEnrollingUser();
  bool addFingerFromScanner();
  bool addFingerFromFile(const QString& imagePath);
  void newUser();
  bool enrollUser();
  bool identifyUser();
  void disconnectFromHost();
signals:
  void connected();
  void loggedIn();
  void disconnected();
  void error(QAbstractSocket::SocketError socketError);
  void attemptConnect();
  void attemptToWrite();
  void updateLog(QString);
private slots:
  void onServerResponseRead();
  void onConnectedSlot();
  void onStateChanged(QAbstractSocket::SocketState state);
  void disconnectedClient();
  void encryptedSlot();
  void onEncryptedSlot();
  void onSslErrorSlot(const QList<QSslError> &errorList);
};
#endif // CLIENT_HPP
