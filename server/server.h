#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <sstream>
#include <QSslSocket>
#include <QSslError>
#include <memory>
#include <vector>
#include <string>
#include "../extractor/extraction.h"
#include "../extractor/extraction_caffenetwork.h"
#include "databaseconnection.h"
#include "preprocessing.h"

#define HEADERSIZE 15 // in this application, all the fingerprints are sent in format header<0, 15) + rawData<15, end)

class Server : public QTcpServer
{
    Q_OBJECT
private:
    std::shared_ptr<QSslSocket> m_socket;
    std::unique_ptr<QTcpServer> m_server;        
    std::shared_ptr<DatabaseConnection> m_db;    
    QByteArray m_receivedTemplate;
    std::shared_ptr<Extraction> m_extractor;
    std::shared_ptr<Preprocessing> m_preprocessing;
    int m_messageCounter;
    int m_expectingSize;
    QString m_certificate;
    QString m_key;

    //private methods
    int size2int(QByteArray received);
    bool checkIp(QString &addr);
    void extraction();
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

    //public methods
    void initialize(QString &addr, quint16 &port);
    void terminate();    

private slots:
    void newConnectionSlot();
    void onReadyRead();
    void receivedMessage();
    void disconnectedClient();
    void connectedClient();
    void onStateChanged(QAbstractSocket::SocketState state);
    void onError(QAbstractSocket::SocketError error);    
    void onPreprocessingDoneSlot(PREPROCESSING_RESULTS preprocessingResults);
    void onPreprocessingErrorSlot(int error);
    void onExtractionDoneSlot(EXTRACTION_RESULTS extractionResults);
    void onExtractionErrorSlot(int error);
    void onExtractionProgressSlot(int progress);
    void onSslErrorSlot(const QList<QSslError> &errorList);
signals:
    void updateLog(QString log);
    void sendImage(QByteArray);

public slots:
};

#endif // SERVER_H
