#include "client.hpp"

Client::Client(QObject *parent) :
  QObject(parent),
  m_socket(std::make_shared<QSslSocket>(new QSslSocket())),
  m_user(std::make_shared<User>(new User()))
{
  UFS_STATUS err = UFS_Init();
  UFS_CHECK_ERROR(err);
  err = UFS_GetScannerHandle(0, &m_scanner);
  UFS_CHECK_ERROR(err);
}

Client::~Client()
{
  UFS_STATUS err = UFS_Uninit();
  UFS_CHECK_ERROR(err);
  disconnectFromHost();
}

bool Client::connectionInit(const QString &addr, const quint16 &port)
{
  if (m_socket) {
    if (m_socket.get()->isOpen()){
      qDebug() << "[E] : Socket connected - disconnect first";
      return false;
    }
  }

  if (checkIp(addr)) {
    m_socket.reset();
    m_socket = std::shared_ptr<QSslSocket>(new QSslSocket());
    connect(m_socket.get(), SIGNAL(encrypted()), this, SLOT(encryptedSlot()));
    connect(m_socket.get(), SIGNAL(sslErrors(const QList<QSslError>&)),
            this, SLOT(onSslErrorSlot(const QList<QSslError>&)));
    connect(m_socket.get(), SIGNAL(connected()), this, SLOT(onConnectedSlot()));
    connect(m_socket.get(), SIGNAL(encrypted()), this, SLOT(onEncryptedSlot()));

    m_socket.get()->connectToHostEncrypted(addr, port);
    connect(m_socket.get(), SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    connect(m_socket.get(), SIGNAL(disconnected()), this, SLOT(disconnectedClient()));
    connect(m_socket.get(), &QSslSocket::readyRead, this, &Client::onServerResponseRead);
  }
  else {
    qDebug() << "[E] : "<< addr << "is in wrong format!";
    return false;
  }

  if (m_socket) {
    if (!m_socket.get()->isOpen()) {
      m_socket.reset();
      qDebug() << "[E] : " << "Could not connect socket";
      return false;
    }

    if (m_socket.get()->isEncrypted()) {
      qDebug() << "[I] : Encrypted connection is set";
    }
  }

  return true;
}

bool Client::deleteCurrentlyEnrollingUser()
{
  newUser();
  const quint8 _fpc{m_user.get()->getFingersCount()};
  if (0 != _fpc) {
    return false;
  }
  return true;
}

bool Client::addFingerFromScanner()
{
  if (!m_user.get()) {
    newUser();
  }
  const quint8 prev_count{m_user.get()->getFingersCount()};
  QImage f = readFingerFromScanner();
  if (f.isNull()){
    return false;
  }

  const quint8 curr_count{m_user.get()->addFinger(f)};

  if (1 != curr_count - prev_count){
    return false;
  }

  return true;
}

bool Client::addFingerFromFile(const QString &imagePath)
{
  const quint8 prev_count{m_user.get()->getFingersCount()};
  QFileInfo file_info(imagePath);
  if (file_info.isDir()) {
      QVector<QImage> images{};
      quint8 _imCount = readFingersFromDirectory(file_info, &images);
      for(const auto& image : images) {
        m_user.get()->addFinger(image);
      }
      const quint8 curr_count{m_user.get()->getFingersCount()};
      if (prev_count != curr_count - _imCount) {
        return false;
      }
  }
  else if (file_info.isFile()){
    QImage f = readFingerFromImage(imagePath);
    if (f.isNull()) {
      return false;
    }
    const quint8 curr_count{m_user.get()->addFinger(f)};
    if (1 != curr_count - prev_count){
      return false;
    }
  }

  return true;
}

void Client::newUser()
{
  m_user.reset();
  m_user = std::make_shared<User>(new User());
}

bool Client::sendProbeUser(int operation)
{
  if (!m_socket) {
    return false;
  }
  if (m_socket.get()->isEncrypted()) {
    QByteArray serialization_buffer{};
    m_user.get()->serializeUser(serialization_buffer, operation);
    if (serialization_buffer.size() != m_socket.get()->write(serialization_buffer)) {
      qDebug() << "[E] : Failed to send a whole user to server";
      return false;
    }
  }
  else {
    qDebug() << "[E] : Connection is not encrypted";
    return false;
  }

  qDebug() << "[I] : User has been sent";
  return true;
}

bool Client::isImageFormatSupported(const QString &format)
{
  QVector<QString> _supported_formats = { "bmp",
                                          "gif",
                                          "jpg",
                                          "png"};
  const bool supported = _supported_formats.contains(format);
  return supported;
}

bool Client::enrollUser()
{
  const bool result = sendProbeUser(0);
  return result;
}

bool Client::identifyUser()
{
  const bool result = sendProbeUser(1);
  return result;
}

void Client::disconnectFromHost()
{
  if (m_socket) {
    m_socket.get()->disconnectFromHost();
    m_socket.get()->close();
  }
  m_socket.reset();
}

void Client::onServerResponseRead()
{
  if (m_socket) {
    QString _bytes = m_socket.get()->readAll();
    emit updateLog(_bytes);
    qDebug() << _bytes;
  }
}

void Client::onConnectedSlot()
{
  if (m_socket) {
    QString pa = m_socket.get()->peerAddress().toString();
    quint16 pp = m_socket.get()->peerPort();
    qDebug() << "[I] : Connected to " << pa <<  ":" << pp;
  }
}

void Client::onStateChanged(QAbstractSocket::SocketState state)
{
  qDebug() << state;
}

void Client::disconnectedClient()
{
  if (m_socket) {
    qintptr source_sd = m_socket.get()->socketDescriptor();
    qDebug() << "[I] : Client disconnected (SD: " << source_sd << ")";
    m_socket.get()->abort();
  }
}

void Client::encryptedSlot()
{
  qDebug() << "[I] : Connection is encrypted";
}

void Client::onSslErrorSlot(const QList<QSslError> &errorList)
{
  qintptr source_sd = m_socket.get()->socketDescriptor();
  qDebug() << "[W] : Ignoring SSL Errors (SD " << source_sd << ")";
  m_socket.get()->ignoreSslErrors(errorList);
}

QImage Client::readFingerFromScanner()
{
  UFS_STATUS err = UFS_ClearCaptureImageBuffer(m_scanner);
  UFS_CHECK_ERROR(err);
  err = UFS_CaptureSingleImage(m_scanner);
  UFS_CHECK_ERROR(err);
  int width{0}, height{0}, resolution{0};
  err = UFS_GetCaptureImageBufferInfo(m_scanner, &width, &height, &resolution);
  UFS_CHECK_ERROR(err);
  QVector<uchar> bufFinger(width * height);
  err = UFS_GetCaptureImageBuffer(m_scanner, bufFinger.data());
  UFS_CHECK_ERROR(err);
  QImage outFingerImage(bufFinger.data(), width, height, QImage::Format_Grayscale8);
  outFingerImage = outFingerImage.convertToFormat(QImage::Format_ARGB32);
  return outFingerImage;
}

QImage Client::readFingerFromImage(const QString& imagePath)
{
  QImage outFingerImage{};
  QFileInfo _file_info(imagePath);
  if (!_file_info.exists()) {
    return outFingerImage;
  }

  QString imageFormat{};
  {
    QImageReader ireader(imagePath);
    imageFormat = ireader.format();
  }

  const bool _supported = isImageFormatSupported(imageFormat.toLower());
  if (!_supported) {
    return outFingerImage;
  }

  QImage image(imagePath, imageFormat.toStdString().c_str());
  outFingerImage = image.convertToFormat(QImage::Format_ARGB32);

  return outFingerImage;
}

quint8 Client::readFingersFromDirectory(const QFileInfo& fileInfo, QVector<QImage>* outImages)
{
  QString absPath = fileInfo.filePath();
  QDir imgDir(absPath);
  QStringList dirList = imgDir.entryList(QStringList()  << "*.bmp" << "*.BMP"
                                                        << "*.gif" << "*.GIF"
                                                        << "*.jpg" << "*.JPG"
                                                        << "*.png" << "*.PNG",
                                                        QDir::Files);
  for (const auto& image : dirList) {
    const QString pathBuilder = absPath + "/" + image;
    QImage loadedImage = readFingerFromImage(pathBuilder);
    outImages->push_back(loadedImage);
  }
  const  quint8 imgCount = static_cast<quint8>( dirList.size() );
  return imgCount;

}

void Client::onEncryptedSlot()
{
  qDebug() << "[I] : Established encrypted connection";
}

bool Client::checkIp(const QString &receivedIp)
{
  QRegularExpression re("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");
  if(re.match(receivedIp).hasMatch()) {
    return true;
  } else if(receivedIp == "localhost") {
    return true;
  }

  return false;
}

void Client::UFS_CHECK_ERROR(const UFS_STATUS& err)
{
  if(UFS_OK != err){
    char *message{};
    UFS_STATUS _err = UFS_GetErrorString(err, message);
    UFS_CHECK_ERROR(_err);
    qDebug() << QString(message);
    exit(err);
  }
}
