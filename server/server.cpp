#include "server.h"
#include <iostream>
#include <QDataStream>
#include <sstream>
#include <QAbstractSocket>
#include <regex>

Server::Server(QObject *parent) :
    QObject(parent),
    m_socket(std::shared_ptr<QTcpSocket>(new QTcpSocket())),
    m_server(std::unique_ptr<QTcpServer>(new QTcpServer())),
    m_db(std::shared_ptr<DatabaseConnection>(new DatabaseConnection())),
    m_messageCounter{0},
    m_expectingSize{0}
{

}

Server::~Server()
{
}

void Server::initialize(QString &addr, quint16 &port)
{
    if(!checkIp(addr)) {
        qDebug() << "Error - wrong IP address";
        return;
    }
    if(m_server.get()->isListening()){
        emit updateLog("server is already listening");
    } else if (m_server.get()->listen(QHostAddress(addr), port)) {
        emit updateLog("server runs now...");        
    } else {
        emit updateLog("could not start a server");
    }
    connect(m_server.get(), SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void Server::terminate()
{
    if (m_server.get()->isListening()){
        m_server.get()->close();
        updateLog("server terminated...");
    }
    else {
        updateLog("server is not running...");
    }
}

void Server::newConnection()
{
    if(m_server.get()->hasPendingConnections()){
        m_socket = std::shared_ptr<QTcpSocket>(m_server.get()->nextPendingConnection());
        qDebug() << m_socket.get()->state();
        connect(m_socket.get(), &QAbstractSocket::readyRead, this, &Server::receivedMessage);
        connect(m_socket.get(), &QAbstractSocket::connected, this, &Server::connectedClient);
        connect(m_socket.get(), SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
        connect(m_socket.get(), SIGNAL(disconnected()), this, SLOT(disconnectedClient()));
    }
    if(!m_socket.get()->open(QIODevice::ReadOnly)){
        emit updateLog("error during setting a readonly mode...");
    }
}

void Server::onReadyRead()
{
    QByteArray r = qobject_cast<QTcpSocket*>(sender())->readAll();
    qDebug() << r.size();
}

int Server::size2int(QByteArray received)
{
    std::stringstream ss;
    for (int i = 0; i < HEADERSIZE; ++i){
        if (static_cast<unsigned char>(62) != static_cast<unsigned char>(received.at(i))) {
            ss << static_cast<int>(received.at(i));
        }
    }
    return atoi(ss.str().c_str());
}

void Server::receivedMessage()
{
    QByteArray r = qobject_cast<QTcpSocket*>(sender())->readAll();
    if (1 == ++m_messageCounter) {
        m_expectingSize = size2int(r.mid(0, HEADERSIZE));
        m_receivedTemplate2.clear();
    }

    m_receivedTemplate2.append(r);
    if (m_expectingSize == m_receivedTemplate2.size()){
        m_expectingSize = 0; //get ready to receive a new fingerprint
        m_messageCounter = 0;
        qDebug() <<"done: " << m_receivedTemplate2.size();
    }
}

void Server::disconnectedClient()
{
    emit updateLog("Client disconnected");
}

void Server::connectedClient()
{
    qDebug() << m_socket.get()->peerAddress();
    qDebug() << m_socket.get()->peerPort();
}

void Server::onStateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << "Status: " << state;
}

void Server::onError(QAbstractSocket::SocketError error)
{
    qDebug() << "Error: " << error;
}

bool Server::checkIp(QString &receivedIp)
{
    std::string stdIp = receivedIp.toStdString();
    std::regex r{"^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$"};
    if(std::regex_match(stdIp.begin(), stdIp.end(), r)){
        return true;
    } else if(stdIp == "localhost"){
        return true;
    } else {
        return false;
    }
}
