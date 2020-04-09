#include "server.h"

Server::Server(QObject *parent) :
  QTcpServer(parent),
  m_db(std::shared_ptr<DatabaseConnection>(new DatabaseConnection())),
  m_receivedTemplate{},
  m_extractor(std::shared_ptr<Extraction>(new Extraction())),
  m_preprocessing(std::shared_ptr<Preprocessing>(new Preprocessing())),
  m_matcher(std::shared_ptr<Matcher>(new Matcher())),
  m_expectingSize{-1},
  m_certificate(QString(SRC_DIR) + "/certificates/server.cert.pem"),
  m_key(QString(SRC_DIR) + "/certificates/server.key.pem")
{
  m_extractor.get()->setCPUOnly(true);
  m_extractor.get()->setFeatures(true);
  m_preprocessing.get()->setCPUOnly(true);

  qRegisterMetaType<QMap<QString, PREPROCESSING_RESULTS>>("QMap<QString, PREPROCESSING_RESULTS>");
  connect(m_preprocessing.get(), SIGNAL(preprocessingSequenceDoneSignal(QMap<QString, PREPROCESSING_RESULTS>)),
          this, SLOT(onPreprocessingSequenceDoneSlot(QMap<QString, PREPROCESSING_RESULTS>)));

  qRegisterMetaType<PREPROCESSING_RESULTS>("PREPROCESSING_RESULTS");
  connect(m_preprocessing.get(), SIGNAL(preprocessingDoneSignal(PREPROCESSING_RESULTS)),
          this, SLOT(onPreprocessingDoneSlot(PREPROCESSING_RESULTS)));
  connect(m_preprocessing.get(), SIGNAL(preprocessingErrorSignal(int)), this, SLOT(onPreprocessingErrorSlot(int)));

  qRegisterMetaType<EXTRACTION_RESULTS >("EXTRACTION_RESULTS");
  connect(m_extractor.get(), SIGNAL(extractionDoneSignal(EXTRACTION_RESULTS)),
          this, SLOT(onExtractionDoneSlot(EXTRACTION_RESULTS)));

  qRegisterMetaType<QMap<QString, EXTRACTION_RESULTS>>("QMap<QString, EXTRACTION_RESULTS>");
  connect(m_extractor.get(), SIGNAL(extractionSequenceDoneSignal(QMap<QString, EXTRACTION_RESULTS>)),
          this, SLOT(onExtractionSequenceDoneSlot(QMap<QString, EXTRACTION_RESULTS>)));
  connect(m_extractor.get(), SIGNAL(extractionErrorSignal(int)), this, SLOT(onExtractionErrorSlot(int)));

  connect(m_matcher.get(), SIGNAL(identificationDoneSignal(bool, QString, float)), this, SLOT(onIdentificationDoneSlot(bool, QString, float)));

  m_db.get()->setDb(); // default db configuration
}

Server::~Server()
{
  for (QSslSocket* socket : m_sockets) {
    delete socket;
  }
  m_sockets.clear();
}

void Server::incomingConnection(qintptr socketDescriptor)
{
  QSslSocket *newSocket = new QSslSocket;
  if(newSocket->setSocketDescriptor(socketDescriptor)){
    addPendingConnection(newSocket);
    connect(newSocket, &QSslSocket::encrypted, this, &Server::onEncryptedSlot);
    connect(newSocket, SIGNAL(disconnected()), this, SLOT(disconnectedClientSlot()));
    connect(newSocket, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(onErrorSlot(QAbstractSocket::SocketError)));
    connect(newSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    connect(newSocket, SIGNAL(sslErrors(const QList<QSslError> &)),this, SLOT(onSslErrorSlot(const QList<QSslError> &)));
    connect(newSocket, &QSslSocket::readyRead, this, &Server::receiveUserFromClient);
    newSocket->setPrivateKey(m_key);
    newSocket->setLocalCertificate(m_certificate);
    newSocket->startServerEncryption();
    m_sockets.push_back(newSocket);
    emit updateClientList(m_sockets);
    emit updateLog("Status: Client connected");
  }
  else {
    emit updateLog("Error: Could not bind an encrypted connection with a server");
    delete newSocket;
  }
}

bool Server::checkIp(QString &receivedIp)
{
  std::string stdIp = receivedIp.toStdString();
  std::regex r{"^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$"};
  if(std::regex_match(stdIp.begin(), stdIp.end(), r)){
    return true;
  }
  else if(stdIp == "localhost"){
    return true;
  }
  else {
    return false;
  }
}

void Server::initialize(QString &addr, quint16 &port)
{
  if(!checkIp(addr)) {
    emit updateLog("Error: Wrong IP address");
    return;
  }
  if(this->isListening()) {
    emit updateLog("Warning: Listening state - skipping");
  }
  else if (this->listen(QHostAddress(addr), port)) {
    emit updateLog("Message: Waiting for a connection");
  }
  else {
    emit updateLog("Error: Could not start a server");
  }
}

void Server::terminate()
{
  if (this->isListening()){
    this->close();
    updateLog("Message: Server terminated");
  }
  else {
    updateLog("Error: Server not running");
    return;
  }

  for (QSslSocket* socket : m_sockets) {
    delete socket;
  }
  m_sockets.clear();
}

void Server::deserializeCurrentlyReceivedUser(int* operation)
{
  QByteArray inputBytes = m_receivedTemplate;
  QDataStream tempStream(inputBytes);
  int sz{0}, op{0};
  quint8 fpcount{0};
  QVector<QImage> receivedFingersVector{};
  tempStream >> sz >> op >> fpcount >> receivedFingersVector;
  *operation = op;

  if (0 == op) { // registration
    QVector<cv::Mat> matVector{};
    for (const QImage& singleImage: receivedFingersVector) {
      cv::Mat probeIsoTemplate(singleImage.height(),
                               singleImage.width(),
                               CV_8UC4,
                               (void*)(singleImage.bits()),
                               static_cast<size_t>(singleImage.bytesPerLine()));
      matVector.push_back(probeIsoTemplate);
    }

    m_preprocessing.get()->loadInput(matVector);
  }
  else if (1 == op) { //identification
    QImage singleImage = receivedFingersVector.at(0);
    cv::Mat probeIsoTemplate(singleImage.height(),
                             singleImage.width(),
                             CV_8UC4,
                             (void*)(singleImage.bits()),
                             static_cast<size_t>(singleImage.bytesPerLine()));

    m_preprocessing.get()->loadInput(probeIsoTemplate);
  }
  else {
    qDebug() << "Error: Operation not allowed";
    return;
  }

  m_preprocessing.get()->start();
}

void Server::receiveUserFromClient()
{
  QSslSocket *source = qobject_cast<QSslSocket*>(sender());
  qint64 incomingBytes =  source->bytesAvailable();
  QByteArray r = source->readAll();
  if (-1 == m_expectingSize) {
    QDataStream incommingStream(&r, QIODevice::ReadOnly);
    incommingStream >> m_expectingSize;
  }
  if (m_expectingSize > m_receivedTemplate.size() + incomingBytes) {
    m_receivedTemplate.push_back(r);
  }
  else if (m_expectingSize == m_receivedTemplate.size() + incomingBytes){
    m_receivedTemplate.push_back(r);
    int operation{0};
    deserializeCurrentlyReceivedUser(&operation);
    m_expectingSize = -1;
    m_receivedTemplate.clear();
  }
  else {
    m_expectingSize = -1;
    m_receivedTemplate.clear();
    emit updateLog("Error: Something went wrong when receiving user from client.");
    QTextStream streamErrorMsg;
    streamErrorMsg << "Error: Something went wrong when receiving user from client:\n";
    streamErrorMsg << "\tExpected size of user: " << m_expectingSize << "\n";
    streamErrorMsg << "\tActual size: " << r.size();
    QString errorMsg;
    streamErrorMsg >> errorMsg;
    emit updateLog(errorMsg);
  }
}

void Server::identifyUser(uchar* user)
{
  QMultiMap<QString, QVector<uchar>> users{};
  m_db.get()->getAllUsersFromDb(&users);
  m_matcher.get()->identify(user, users);
}

void Server::onPreprocessingDoneSlot(PREPROCESSING_RESULTS preprocessingSingleResult)
{
  emit updateLog("Status: Preprocessing done");
  m_extractor.get()->loadInput(preprocessingSingleResult);
  m_extractor.get()->start();
}

void Server::onPreprocessingSequenceDoneSlot(QMap<QString, PREPROCESSING_RESULTS> preprocessingResults)
{
  emit updateLog("Status: preprocessing done");
  m_extractor.get()->loadInput(preprocessingResults);
  m_extractor.get()->start();
}

void Server::onPreprocessingErrorSlot(int error)
{
  emit updateLog("Error: Preprocessor returned " + QString::number(error));
}

void Server::onExtractionDoneSlot(EXTRACTION_RESULTS extractionResults)
{
  int isoTplSize = HEADER_LENGHT
                + (FINGER_VIEW_HEADER_LENGTH
                + (extractionResults.minutiaePredicted.size() * ISO_MINUTIA_LENGTH)
                + EXTENDED_DATA_BLOCK_LENGTH);
  QVector<uchar> isoTemplate(isoTplSize);
  memcpy(isoTemplate.data(), extractionResults.minutiaeISO, static_cast<size_t>(isoTplSize));
  identifyUser(isoTemplate.data());
}

void Server::onExtractionSequenceDoneSlot(QMap<QString, EXTRACTION_RESULTS> resultMap)
{
  QVector<QVector<uchar>> userIsoTemplates{};
  for(const auto& result : resultMap){
    int isoTplSize = HEADER_LENGHT
                  + (FINGER_VIEW_HEADER_LENGTH
                  + (result.minutiaePredicted.size() * ISO_MINUTIA_LENGTH)
                  + EXTENDED_DATA_BLOCK_LENGTH);

    QVector<uchar> isoTpl(isoTplSize);
    memcpy(isoTpl.data(), result.minutiaeISO, static_cast<size_t>(isoTplSize));
    userIsoTemplates.push_back(isoTpl);
  }
  if (!m_db.get()->registerUserToDb(userIsoTemplates)) {
    emit updateLog("Error: User is not registered");
  }
  else {
    emit updateLog("Succes: User is registered");
  }
}

void Server::onIdentificationDoneSlot(bool success, QString subject, float score)
{
  if (success) {
    emit updateLog("User is identified: " + subject + " - Best score is " + QString::number(static_cast<double>(score)));
  }
  else {
    emit updateLog("User is not in database");
  }
}

void Server::onMatcherErrorSlot(int errCode)
{
  emit updateLog("Error: Identification failed with code " + QString::number(errCode));
}

void Server::onExtractionErrorSlot(int error)
{
  emit updateLog("Error: Extractor returned " + QString::number(error));
}

void Server::disconnectedClientSlot()
{
  emit updateLog("Status: Client disconnected");
  QSslSocket *disconnectedSocket = qobject_cast<QSslSocket*>(sender());
  m_sockets.removeOne(disconnectedSocket);
}

void Server::onStateChanged(QAbstractSocket::SocketState state)
{
  emit updateLog("Status: " + QString(state));
}

void Server::onErrorSlot(QAbstractSocket::SocketError error)
{
  emit updateLog("Error: " + QString(static_cast<unsigned char>(error)));
}

void Server::onEncryptedSlot()
{
  if (qobject_cast<QSslSocket*>(sender())->isEncrypted()){
    updateLog("Status: Encrypted connection established [SD: " + QString::number(qobject_cast<QSslSocket*>(sender())->socketDescriptor()) + "]");
  }
}

void Server::onSslErrorSlot(const QList<QSslError> &errorList)
{
  for(const QSslError &e : errorList){
    qDebug() << "Ssl error: " << e.errorString();
    qobject_cast<QSslSocket*>(sender())->ignoreSslErrors(errorList);
  }
  emit updateLog("Status: Ignoring SSL errors [SD: " +
                  QString::number(qobject_cast<QSslSocket*>(sender())->socketDescriptor()) + "]");
}
