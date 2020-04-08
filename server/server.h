#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <sstream>
#include <QSslSocket>
#include <QSslError>
#include <QPixmap>
#include <memory>
#include <vector>
#include <string>
#include <QImage>
#include "../extractor/extraction.h"
#include "../extractor/extraction_caffenetwork.h"
#include "databaseconnection.h"
#include "preprocessing.h"

class Server : public QTcpServer
{
    Q_OBJECT
private:
    QVector<QSslSocket*> m_sockets;
    std::shared_ptr<DatabaseConnection> m_db;
    QByteArray m_receivedTemplate;
    std::shared_ptr<Extraction> m_extractor;
    std::shared_ptr<Preprocessing> m_preprocessing;
    int m_expectingSize;
    QString m_certificate;
    QString m_key;

    //private methods
    bool checkIp(QString &addr);
    void preprocessing();
//    void minutiaeVisualisation(QByteArray fingerprint, QVector<MINUTIA> minutiaeList);
    void deserializeCurrentlyReceivedUser(int* operation);
public:
    explicit Server(QObject *parent = nullptr);
    ~Server() override;

    //public methods
    void initialize(QString &addr, quint16 &port);
    void terminate();    

private slots:  
    void receivedMessage();
    void disconnectedClientSlot();
    void onStateChanged(QAbstractSocket::SocketState state);
    void onErrorSlot(QAbstractSocket::SocketError error);
    void onPreprocessingDoneSlot(PREPROCESSING_RESULTS preprocessingResults);
    void onPreprocessingErrorSlot(int error);
    void onExtractionDoneSlot(EXTRACTION_RESULTS extractionResults);
    void onExtractionErrorSlot(int error);    
    void onSslErrorSlot(const QList<QSslError> &errorList);
    void onEncryptedSlot();
    void incomingConnection(qintptr socketDescriptor) override;
signals:
    void updateLog(QString);
    void sendImage(QByteArray);
    void sendImage(QPixmap);
    void updateClientList(QVector<QSslSocket*>);

public slots:
};

#endif // SERVER_H
