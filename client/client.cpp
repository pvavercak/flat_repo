#include "client.h"
#include <regex>
#include <QDataStream>
#include <QImage>

Client::Client(QObject *parent) :
    QObject(parent),
    m_scanner(std::unique_ptr<FpHandler>(new FpHandler())),
    m_socket(std::shared_ptr<QSslSocket>(new QSslSocket()))
{    
    connect(m_socket.get(), SIGNAL(encrypted()), this, SLOT(encryptedSlot()));
    connect(m_socket.get(), SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslErrorSlot(const QList<QSslError>&)));
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
//    QImage image((unsigned char*)m_scanData.data(), 320, 480, QImage::Format_Grayscale8);
//    image.save("/home/pva/njdj.png");
}

bool Client::connectionInit(QString &addr, quint16 &port)
{
    if (checkIp(addr)) {
        connect(m_socket.get(), SIGNAL(connected()), this, SLOT(onConnectedSlot()));
        connect(m_socket.get(), SIGNAL(encrypted()), this, SLOT(onEncryptedSlot()));
        m_socket.get()->connectToHostEncrypted(addr, port);        
    } else {
        emit updateLog("Error: Wrong IP address");
    }
    if (!m_socket.get()->isOpen()){
        qDebug() << "Error: Connection unsuccessful";
        return false;
    }    
    connect(m_socket.get(), SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    connect(m_socket.get(), SIGNAL(disconnected()), this, SLOT(disconnectedClient()));
    if (m_socket.get()->isEncrypted()){
        emit updateLog("Status: Encrypted connection established");
    }
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
    emit updateLog("Status: Client disconnecting");
    m_socket.get()->disconnectFromHost();
    m_socket.get()->close();
}

void Client::onReadyRead()
{

}

void Client::onConnectedSlot()
{
    emit updateLog("Status: Connected to host");
}

void Client::onEncryptedSlot()
{

}

void Client::onStateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << "Status: " << state;
}

void Client::disconnectedClient()
{
    emit updateLog("Status: Client disconnected [ SD: " + QString::number(qobject_cast<QSslSocket*>(sender())->socketDescriptor()) + " ]");
}

void Client::encryptedSlot()
{
    emit updateLog("Status: Connection is encrypted");
}

void Client::onSslErrorSlot(const QList<QSslError> &errorList)
{
    emit updateLog("Warning: Ignoring SSL Errors [ SD: " + QString::number(qobject_cast<QSslSocket*>(sender())->socketDescriptor()) + " ]");
    m_socket.get()->ignoreSslErrors(errorList);
}

bool Client::checkIp(QString &receivedIp)
{
    std::string stdIp = receivedIp.toStdString();
    std::regex r{"^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$"};
    if(std::regex_match(stdIp.begin(), stdIp.end(), r)){        
        return true;
    } else if(stdIp == "localhost"){        
        return true;
    } else {
        return false;
    }
}
