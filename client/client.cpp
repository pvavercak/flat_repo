#include "client.h"
#include <regex>
#include <QDataStream>
#include <QImage>

Client::Client(QObject *parent) :
    QObject(parent),
    m_scanner(std::unique_ptr<FpHandler>(new FpHandler())),
    m_socket(std::shared_ptr<QTcpSocket>(new QTcpSocket()))
{                
}

bool Client::isSocketConnected()
{
    if (m_socket.get()->isOpen()) {
        return true;
    }
    return false;
}

void Client::writeTemplate()
{    
    if (m_scanData.size() != m_socket.get()->write(m_scanData)) {
        qDebug() << "Error sending data";
    }    
    QImage image((unsigned char*)m_scanData.data(), 320, 480, QImage::Format_Grayscale8);
    image.save("/home/pva/njdj.png");
}

bool Client::connectionInit(QString &addr, quint16 &port)
{
    if (checkIp(addr)) {
        m_socket.get()->connectToHost(addr, port);
    } else {
        qDebug() << "Provide a valid host address";
    }
    if (!m_socket.get()->isOpen()){
        qDebug() << "Could not connect to host";
        return false;
    }
    connect(m_socket.get(), &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(m_socket.get(), SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    connect(m_socket.get(), SIGNAL(disconnected()), this, SLOT(disconnectedClient()));
    return true;
}

void Client::makeScan()
{
    m_scanData.clear(); //clear old scan data
    m_scanner.get()->startScan();
    m_scanData = m_scanner.get()->getScanData();
}

void Client::disconnectFromHost()
{
    m_socket.get()->disconnectFromHost();
    m_socket.get()->close();
}

void Client::onReadyRead()
{

}

void Client::onStateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << "Client socket state changed to " << state;
}

void Client::disconnectedClient()
{
    qDebug() << "Client is disconnected from server";
}

bool Client::checkIp(QString &receivedIp)
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
