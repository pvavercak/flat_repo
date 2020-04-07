#include "client.hpp"

Client::Client(QObject *parent) :
    QObject(parent),
    m_socket(std::shared_ptr<QSslSocket>(new QSslSocket())),
    m_user{}
{    
    CHECK_ERROR(UFS_Init());
    CHECK_ERROR(UFS_GetScannerHandle(0, &m_scanner));
    connect(m_socket.get(), SIGNAL(encrypted()), this, SLOT(encryptedSlot()));
    connect(m_socket.get(), SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(onSslErrorSlot(const QList<QSslError>&)));
}

Client::~Client()
{
    CHECK_ERROR(UFS_Uninit());
}

bool Client::connectionInit(const QString &addr, const quint16 &port)
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

bool Client::deleteCurrentlyEnrollingUser()
{
    if (!m_user.removeAllFingers()) {
        emit updateLog("Message: User cleared");
        return true;
    } else {
        emit updateLog("Error: User was not cleared");
        return false;
    }
}

bool Client::addFingerFromScanner()
{
    quint8 prev_count{m_user.getFingersCount()};
    finger f = readFingerFromScanner();
    if (f.isEmpty()){
        return false;
    }

    quint8 curr_count{m_user.addFinger(f)};

    if (1 != curr_count - prev_count){
        return false;
    }

    return true;
}

bool Client::addFingerFromFile(const QString &imagePath)
{
    quint8 prev_count{m_user.getFingersCount()};
    finger f = readFingerFromImage(imagePath);
    if (f.isEmpty()) {
        return false;
    }

    quint8 curr_count{m_user.addFinger(f)};

    if (1 != curr_count - prev_count){
        return false;
    }

    return true;
}

void Client::newUser()
{
    m_user.removeAllFingers();
}

bool Client::sendProbeUser(int operation)
{
    if(m_socket.get()->isEncrypted()){
        QByteArray serialization_buffer{};
        m_user.serializeUser(serialization_buffer, operation);
        if(serialization_buffer.size() != m_socket.get()->write(serialization_buffer)){
            return false;
        }
    } else {
        return false;
    }

    return true;
}

void Client::sendBadMessage()
{
    qDebug() << m_socket.get()->write("vlkjdnvdsjnvsdjn");
}

void Client::enrollUser()
{
    // 0 for registration
    if (sendProbeUser(0)){
        emit updateLog("Status: User registration");
    } else {
        emit updateLog("Error: User registration failed");
    }
}

void Client::identifyUser()
{
    // 1 for identification
    if (sendProbeUser(1)){
        emit updateLog("Status: User identification");
    } else {
        emit updateLog("Error: User identification failed");
    }
}

bool Client::processInputFinger()
{
    return true;
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

bool Client::isSocketConnected()
{
    if (m_socket.get()->isOpen()) {
        return true;
    }
    return false;
}

bool Client::checkIp(const QString &receivedIp)
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

finger Client::readFingerFromScanner()
{
    CHECK_ERROR(UFS_ClearCaptureImageBuffer(m_scanner));
    CHECK_ERROR(UFS_CaptureSingleImage(m_scanner));
    int width{0}, height{0}, resolution{0};
    CHECK_ERROR(UFS_GetCaptureImageBufferInfo(m_scanner, &width, &height, &resolution));
    finger outFinger(width * height);
    UFS_GetCaptureImageBuffer(m_scanner, outFinger.data());
    return outFinger;
}

finger Client::readFingerFromImage(QString imagePath)
{
    finger outFinger(0);
    if (!QFile::exists(imagePath)) {
        return outFinger;
    }

    QByteArray imageFormat{};
    {
        QImageReader ireader(imagePath);
        imageFormat = ireader.format();
    }
    if (imageFormat.isEmpty()) {
        return outFinger;
    }
    {
        QImage image(imagePath, imageFormat.data());
        outFinger.resize(image.byteCount());
        memcpy(outFinger.data(), image.bits(), static_cast<size_t> (image.byteCount()));
    }
    return outFinger;
}

void Client::CHECK_ERROR(UFS_STATUS err){
    if(UFS_OK != err){
        char *message{};
        UFS_GetErrorString(err, message);
        qDebug() << QString(message);
        exit(err);
    }
}
