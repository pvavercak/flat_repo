#include "server.h"
#include <iostream>
#include <QDataStream>
#include <sstream>

Server::Server(QObject *parent) :
    QObject(parent),
    m_expectedSize(-1)
{
    m_server = std::unique_ptr<QTcpServer>(new QTcpServer());
    m_socket = std::shared_ptr<QTcpSocket>(new QTcpSocket());
    m_db = std::shared_ptr<DatabaseConnection>(new DatabaseConnection());
    m_receivedTemplate = std::shared_ptr<QByteArray>(new QByteArray);
}

Server::~Server()
{

}

void Server::initialize(QHostAddress address, quint16 port)
{
    if(this->m_server.get()->isListening()){
        emit updateLog("server is already listening");
    } else if (this->m_server.get()->listen(address, port)) {
        m_socket.get()->abort();
        //this->m_socket.get()->connectToHost(address, port);
        connect(this->m_socket.get(), SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        connect(this->m_socket.get(), &QTcpSocket::disconnected, this, &Server::disconnectedClient);
        emit updateLog("server runs now...");
        if (m_db.get()->setDb()){
            emit updateLog("database connected");
        } else {
            emit updateLog("Warning: database not connected!");
        }
    } else {
        emit updateLog("could not start a server");
    }
    connect(this->m_server.get(), SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void Server::terminate()
{
    if (this->m_server.get()->isListening()){
        this->m_server.get()->close();        
        updateLog("server terminated...");
    }
    else {
        updateLog("server is not running...");
    }
}

void Server::newConnection()
{
    if(this->m_server.get()->hasPendingConnections()){
        this->m_socket = std::shared_ptr<QTcpSocket>(this->m_server.get()->nextPendingConnection());
        connect(this->m_socket.get(), &QAbstractSocket::readyRead, this, &Server::receivedMessage);
    }
    if(!this->m_socket.get()->open(QIODevice::ReadOnly)){
        emit updateLog("error during setting a readonly mode...");
    }
}

void Server::onReadyRead()
{
    QByteArray r = qobject_cast<QTcpSocket*>(sender())->readAll();
}

int Server::processHeader(QByteArray& header)
{
    std::stringstream expect;
    for(int i = 0; i != 6; ++i){
        expect << header[i];
    }
    std::stringstream sizeOfTemplate;
    if(expect.str() == "expect"){
        for(int i = 6; i < header.size(); ++i){
            sizeOfTemplate << header[i];
        }
        return atoi(sizeOfTemplate.str().c_str());
    }
    return -1;
}

void Server::clearAndSaveTemplate()
{
    if(m_expectedSize == m_receivedTemplate.get()->size()){
        m_expectedSize = 0;
        qDebug() << m_receivedTemplate.get()->toBase64();
        qDebug() << m_db.get()->writeTemplate(m_receivedTemplate.get()->toBase64());
        m_receivedTemplate.get()->clear();
    }
}

void Server::receivedMessage()
{
    QByteArray r = qobject_cast<QTcpSocket*>(sender())->readAll();
    if(processHeader(r) != -1) this->m_expectedSize = processHeader(r);
    else{
        m_receivedTemplate.get()->append(r);
        //qDebug() << r;
    }

    clearAndSaveTemplate();
//    QByteArray r = qobject_cast<QTcpSocket*>(sender())->readAll();
//    QDataStream rData(r);
//    qDebug() << r.size() << "bytes";
//    emit updateLog("template received");
}

void Server::disconnectedClient()
{
    emit updateLog("Client disconnected");
}
