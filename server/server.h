#ifndef SERVER_H
#define SERVER_H
#include <QSslSocket>
#include <QTcpServer>
#include <memory>
#include <vector>
#include <string>
#include <QSqlDatabase>
#include "connection.h"
#include <QSqlQuery>
#define HEADERSIZE 15 // in this application, all the fingerprints are sent in format header<0, 15) + rawData<15, end)
class Server : public QObject
{
    Q_OBJECT
private:
    std::shared_ptr<QTcpSocket> m_socket;
    std::unique_ptr<QTcpServer> m_server;
    std::vector<QTcpSocket*> m_socket_list;    
    std::shared_ptr<QByteArray> m_receivedTemplate;
    QByteArray m_receivedTemplate2;
    std::shared_ptr<DatabaseConnection> m_db;
    int m_messageCounter;
    int m_expectingSize;

    //private methods
    int size2int(QByteArray received);
    bool checkIp(QString &addr);
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();

    //public methods
    void initialize(QString &addr, quint16 &port);
    void terminate();    

private slots:
    void newConnection();
    void onReadyRead();
    void receivedMessage();
    void disconnectedClient();
    void connectedClient();
    void onStateChanged(QAbstractSocket::SocketState state);
    void onError(QAbstractSocket::SocketError error);
signals:
    void updateLog(QString log);

public slots:
};

#endif // SERVER_H
