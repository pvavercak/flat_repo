#include "client.h"
#include <regex>
#include <QDataStream>

Client::Client(QObject *parent) :
    QObject(parent),
    m_socket(new QTcpSocket(this))

{
    connect(m_socket.get(), &QTcpSocket::connected, this, &Client::onConnectedToServer);
    //connect(m_socket.get(), &QTcpSocket::disconnected, this, &Client::disconnected);
    connect(m_socket.get(), &QTcpSocket::readyRead, this, &Client::onReadyRead);
}

void Client::connectToServer()
{
    //m_socket->abort();
    m_socket->connectToHost(address, port);
}

bool Client::isSocketConnected(){
    if (m_socket->isOpen()){
        return true;
    }
    return false;
}

void Client::writeTemplate(QByteArray &bytesToWrite)
{
    int repeat{(int) static_cast<double>(bytesToWrite.size())/SOCKETSIZE};
    qDebug() << bytesToWrite.toBase64();
    for(int i = 0; i != 1 + repeat; ++i){
        std::unique_ptr<QTcpSocket> temp{std::unique_ptr<QTcpSocket>(new QTcpSocket())};
        temp.get()->connectToHost(address, port);
        if((i + 1) * SOCKETSIZE < bytesToWrite.size()){
            //qDebug() << bytesToWrite.mid(i*SOCKETSIZE, (i +1) * SOCKETSIZE);
            qDebug() << "Bytes sent in " << i << " iteration: "<< temp.get()->write(bytesToWrite.mid(i*SOCKETSIZE, SOCKETSIZE));
            temp.get()->waitForBytesWritten(300000);
        } else {
            //qDebug() << bytesToWrite.mid(i*SOCKETSIZE, bytesToWrite.size() - i *SOCKETSIZE);
            qDebug() << "Bytes sent in " << i << " iteration: "<< temp.get()->write(bytesToWrite.mid(i*SOCKETSIZE, bytesToWrite.size() - i * SOCKETSIZE));
            temp.get()->waitForBytesWritten(300000);
        }
        temp.get()->close();
        if(!temp.get()->isOpen()){
            qDebug() << "Temporary socket disconnection successful";
        } else {
            qDebug() << "Error when disconnecting temporary socket";
        }
    }
}

bool Client::writeHeader(QByteArray &headerToWrite, int len)
{
    int writeLen = (int)(m_socket.get()->write(headerToWrite));
    m_socket.get()->waitForBytesWritten();
    if(len != writeLen){
        return false;
    }
    m_socket.get()->disconnectFromHost();
    m_socket.get()->close();
    if(!m_socket.get()->isOpen()){
        qDebug() << "Header sent and socket closed";
    } else {
        qDebug() << "Error in header sending";
    }
    return true;
}

void Client::disconnectFromHost()
{
    m_socket->abort();

}

void Client::onReadyRead(){
    qDebug() << "on ready read";
}

void Client::onConnectedToServer()
{
    qDebug() << "Client: connected";
    //connectToServer();
}

bool Client::checkIp(QString &receivedIp, quint16 receivedPort)
{
    std::string stdIp = receivedIp.toStdString();
    std::regex r{"^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$"};
    if(std::regex_match(stdIp.begin(), stdIp.end(), r)){
        this->address = QHostAddress(receivedIp);
        this->port = receivedPort;
        return true;
    } else if(stdIp == "localhost"){
        this->address = QHostAddress("127.0.0.1");
        this->port = receivedPort;
        return true;
    } else {
        return false;
    }

}
