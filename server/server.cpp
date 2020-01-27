#include "server.h"
#include <iostream>
#include <QDataStream>
#include <regex>
#include "../extractor/extraction.h"
#include <QMap>

Server::Server(QObject *parent) :
    QTcpServer(parent),
    m_socket(std::shared_ptr<QSslSocket>(new QSslSocket())),
    m_server(std::unique_ptr<QTcpServer>(new QTcpServer())),    
    m_db(std::shared_ptr<DatabaseConnection>(new DatabaseConnection())),
    m_extractor(std::shared_ptr<Extraction>(new Extraction())),
    m_preprocessing(std::shared_ptr<Preprocessing>(new Preprocessing())),
    m_messageCounter{0},
    m_expectingSize{0},
    m_certificate(QString(SRC_DIR) + "/certificate/server.cert.pem"),
    m_key(QString(SRC_DIR) + "/certificate/server.key.pem")
{
    m_receivedTemplate.clear();
    m_extractor.get()->setCPUOnly(true);
    connect(m_socket.get(), SIGNAL(sslErrors(const QList<QSslError> &)),this, SLOT(onSslErrorSlot(const QList<QSslError> &)));
    connect(m_preprocessing.get(), SIGNAL(preprocessingDoneSignal(PREPROCESSING_RESULTS)), this, SLOT(onPreprocessingDoneSlot(PREPROCESSING_RESULTS)));
    connect(m_preprocessing.get(), SIGNAL(preprocessingErrorSignal(int)), this, SLOT(onPreprocessingErrorSlot(int)));
    qRegisterMetaType<EXTRACTION_RESULTS >("EXTRACTION_RESULTS");
    connect(m_extractor.get(), SIGNAL(extractionDoneSignal(EXTRACTION_RESULTS)), this, SLOT(onExtractionDoneSlot(EXTRACTION_RESULTS)));
    connect(m_extractor.get(), SIGNAL(extractionErrorSignal(int)), this, SLOT(onExtractionErrorSlot(int)));
    connect(m_extractor.get(), SIGNAL(extractionProgressSignal(int)), this, SLOT(onExtractionProgressSlot(int)));

}

Server::~Server()
{
}

void Server::incomingConnection(qintprt socketDescriptor)
{
    QSslSocket *newSocket = new QSslSocket();
    connect(newSocket, SIGNAL(disconnect()), this, SLOT(onSocketDisconnected));
    newSocket->setLocalCertificate(m_certificate);
    newSocket
            //sslerrors
            //ignoresslErrors(list of errors)
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
    connect(m_server.get(), SIGNAL(newConnection()), this, SLOT(newConnectionSlot()));
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

void Server::newConnectionSlot()
{
    if(m_server.get()->hasPendingConnections()){
        m_socket = std::shared_ptr<QSslSocket>(qobject_cast<QSslSocket*>(m_server.get()->nextPendingConnection()));
        connect(m_socket.get(), &QAbstractSocket::readyRead, this, &Server::receivedMessage);
        connect(m_socket.get(), &QAbstractSocket::connected, this, &Server::connectedClient);
        connect(m_socket.get(), SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
        connect(m_socket.get(), SIGNAL(disconnected()), this, SLOT(disconnectedClient()));
    }
    if(!m_socket.get()->isOpen()){
        emit updateLog("error in connection process");
    }
    m_socket.get()->startServerEncryption();
}

void Server::onReadyRead()
{
    QByteArray r = qobject_cast<QSslSocket*>(sender())->readAll();
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
        m_receivedTemplate.clear();
    }

    m_receivedTemplate.append(r);
    if (m_expectingSize == m_receivedTemplate.size()){
        m_expectingSize = 0; //get ready to receive a new fingerprint
        m_messageCounter = 0;
        sendImage(m_receivedTemplate.mid(HEADERSIZE, m_receivedTemplate.size()));
        extraction();
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

void Server::onPreprocessingDoneSlot(PREPROCESSING_RESULTS preprocessingResults)
{
    qDebug() << "preprocessing done, showing skeleton.";
    m_extractor.get()->loadInput(preprocessingResults);
    m_extractor.get()->start();
}

void Server::onPreprocessingErrorSlot(int error)
{
    qDebug() << "error extraction slot: " << error;
}

void Server::onExtractionDoneSlot(EXTRACTION_RESULTS extractionResults)
{
    qDebug() << "extraction done: " << extractionResults.minutiaeCN.size();

}

void Server::onExtractionErrorSlot(int error)
{
    qDebug() << "extraction error number: " << error;
}

void Server::onExtractionProgressSlot(int progress)
{
    qDebug() << "extraction progress: " << progress;
}

void Server::onSslErrorSlot(const QList<QSslError> &errorList)
{
    for(const QSslError &e : errorList){
        qDebug() << "Ssl error: " << e.errorString();
    }
}

bool Server::checkIp(QString &receivedIp)
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

void Server::extraction()
{
    std::vector<unsigned char> templateToExtract(m_receivedTemplate.begin() + HEADERSIZE, m_receivedTemplate.end());
    cv::Mat image(480, 320, CV_8UC1, templateToExtract.data());
    m_preprocessing.get()->loadInput(image);
    m_preprocessing.get()->start();
}
