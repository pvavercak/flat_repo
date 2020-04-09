#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <sstream>
#include <QSslSocket>
#include <QSslError>
#include <memory>
#include <regex>
#include <vector>
#include <string>
#include <QImage>

#include <databaseconnection.h>
#include <preprocessing.h>
#include <extraction.h>
#include <extraction_caffenetwork.h>
#include <matcher.h>

class Server : public QTcpServer
{
  Q_OBJECT
private:
  QVector<QSslSocket*> m_sockets;
  std::shared_ptr<DatabaseConnection> m_db;
  QByteArray m_receivedTemplate;
  std::shared_ptr<Extraction> m_extractor;
  std::shared_ptr<Preprocessing> m_preprocessing;
  std::shared_ptr<Matcher> m_matcher;
  int m_expectingSize;
  QString m_certificate;
  QString m_key;
  bool checkIp(QString &addr);
  void deserializeCurrentlyReceivedUser(int* operation);
public:
  explicit Server(QObject *parent = nullptr);
  ~Server() override;
  void initialize(QString &addr, quint16 &port);
  void terminate();
private slots:  
  void receivedMessage();
  void disconnectedClientSlot();
  void onStateChanged(QAbstractSocket::SocketState state);
  void onErrorSlot(QAbstractSocket::SocketError error);
  void onPreprocessingDoneSlot(QMap<QString, PREPROCESSING_RESULTS> preprocessingResults);
  void onPreprocessingErrorSlot(int error);
  void onExtractionDoneSlot(EXTRACTION_RESULTS extractionResults);
  void onExtractionErrorSlot(int error);
  void onExtractionSequenceDoneSlot(QMap<QString, EXTRACTION_RESULTS> resultMap);
  void onSslErrorSlot(const QList<QSslError> &errorList);
  void onEncryptedSlot();
  void incomingConnection(qintptr socketDescriptor) override;
signals:
  void updateLog(QString);
  void sendImage(QByteArray);
  void sendImage(QPixmap);
  void updateClientList(QVector<QSslSocket*>);
};
#endif // SERVER_H
