#include "client.h"
#include <regex>
Client::Client(QObject *parent) :
    QObject(parent),
    m_socket(new QTcpSocket(this))

{
    connect(m_socket.get(), &QTcpSocket::connected, this, &Client::connected);
    //connect(m_socket.get(), &QTcpSocket::disconnected, this, &Client::disconnected);
    connect(m_socket.get(), &QTcpSocket::readyRead, this, &Client::onReadyRead);
}

void Client::connectToServer()
{
    m_socket->abort();
    m_socket->connectToHost(address, port);
}

bool Client::isSocketConnected(){
    if (m_socket->isOpen()){
        return true;
    }
    return false;
}

void Client::writeMessage(QByteArray &bytesToWrite)
{
    QByteArray enter{bytesToWrite};
    //enter.append(QString("\n").toUtf8());
    int written = (int)m_socket->write(enter);
    m_socket->waitForBytesWritten(3000);
    if(enter.size() == written){
        qDebug() << "all the data sent";
    } else {
        qDebug() << "not all the data were sent...";
    }
}

void Client::disconnectFromHost()
{
    m_socket->abort();

}

void Client::onReadyRead(){
    qDebug() << "on ready read";
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
