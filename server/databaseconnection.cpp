#include "databaseconnection.h"

DatabaseConnection::DatabaseConnection(QObject *parent) : QObject(parent)
{
  m_database = QSqlDatabase::addDatabase("QMYSQL");
}

DatabaseConnection::~DatabaseConnection()
{
  if (m_database.isOpen()) {
    m_database.close();
  }
}

bool DatabaseConnection::setDb(const QString& username, const QString& pass, const QString& host, const QString& dbname)
{
  m_database.setHostName(host);
  m_database.setUserName(username);
  m_database.setPassword(pass);
  m_database.setDatabaseName(dbname);
  if (!m_database.open()) {
    return false;
  }
  return true;
}

bool DatabaseConnection::setDb()
{
  if (!setDb("bachelor", "vavercak", "localhost", "users")) {
    return false;
  }
  return true;
}

void DatabaseConnection::userTemplateToBytes(const QVector<QVector<MINUTIA> > &userData, QByteArray* outArray)
{
  QDataStream userDataStream(outArray, QIODevice::WriteOnly);
  userDataStream.setVersion(QDataStream::Qt_5_9);
  userDataStream << userData;
}

QDataStream& operator<<(QDataStream& _stream, const MINUTIA& _min) {
  return _stream << _min.xy << _min.type << _min.angle << _min.quality << _min.imgWH;
}

QDataStream& operator<<(QDataStream& _stream, const QVector<MINUTIA>& _vec)
{
  _stream << static_cast<int>(_vec.size());
  for(const auto& _min : _vec){
    _stream << _min.xy << _min.type << _min.angle << _min.quality << _min.imgWH;
  }
  return _stream;
}

QDataStream& operator>>(QDataStream& _stream, MINUTIA& _min) {
  return _stream >> _min.xy >> _min.type >> _min.angle >> _min.quality >> _min.imgWH;
}

QDataStream& operator>>(QDataStream& _stream, QVector<MINUTIA>& vec)
{
  int _sz{};
  vec.clear();
  _stream >> _sz;
  vec.reserve(_sz);
  MINUTIA _min{};
  while(0 != _sz) {
    _stream >> _min;
    vec.push_back(_min);
    --_sz;
  }
  return _stream;
}

bool DatabaseConnection::registerUserToDb(const QVector<QVector<MINUTIA>>& userData)
{
  QByteArray userBinary{};
  userTemplateToBytes(userData, &userBinary);
  QSqlQuery query(m_database);
  query.prepare("INSERT INTO user_templates (template) VALUES (:userBinary);");
  query.bindValue(":userBinary", userBinary);
  if (!query.exec()) {
    qDebug() << query.lastError().text();
    return false;
  }
  return true;
}

bool DatabaseConnection::userIdExists(const quint64 id)
{
  QSqlQuery query(m_database);
  query.prepare("SELECT id FROM user_templates WHERE id = :inputId");
  query.bindValue(":inputId", id);
  if (!query.exec()) {
    qDebug() << "Error [SQL]: " << query.lastError().text();
    return true;
  }
  else {
    if (query.next()) {
      qDebug() << "Error: User with id " << id << " already registered";
      return true;
    }
  }
  return false;
}

bool DatabaseConnection::registerUserToDbWithId(const QVector<QVector<MINUTIA> > &userData, const quint64 id)
{
  if (userIdExists(id)) {
    return false;
  }
  QByteArray userBinary{};
  userTemplateToBytes(userData, &userBinary);
  QSqlQuery query(m_database);
  query.prepare("INSERT INTO user_templates (id, template) VALUES (:inputId, :inputUser);");
  query.bindValue(":inputId", id);
  query.bindValue(":inputUser", userBinary);
  if (!query.exec()) {
    qDebug() << "Error [SQL]: " << query.lastError().text();
    return false;
  }
  return true;
}

bool DatabaseConnection::getUserTemplateByID(const quint64 &userId, QVector<QVector<MINUTIA>>* outUserTemplate)
{
  if (!outUserTemplate) {
    return false;
  }
  if (!userIdExists(userId)) {
    qDebug() << "Error: user does not exist";
    return false;
  }
  QSqlQuery query(m_database);
  query.prepare("SELECT id, template FROM user_templates WHERE id = :searchId;");
  query.bindValue(":searchId", userId);
  if (!query.exec()) {
    qDebug() << "Error: " << query.lastError().text();
    return false;
  }
  else {
    qDebug() << "success " << query.lastQuery();
    if (query.next()){
      QByteArray deserializationBuffer = query.value(1).toByteArray();
      QDataStream deserializationStream(&deserializationBuffer, QIODevice::ReadOnly);
      deserializationStream >> *outUserTemplate;
    }
  }
  return true;
}

bool DatabaseConnection::getAllUsersFromDb(QMultiMap<QString, QVector<MINUTIA>> *allUsers)
{
  if (!allUsers) {
    return false;
  }
  QSqlQuery query(m_database);
  query.prepare("SELECT id, template FROM user_templates;");
  if (!query.exec()) {
    qDebug() << query.lastError().text();
    return false;
  }
  else {
    while (query.next()) {
      QString currentUserId = query.value(0).toString();
      QByteArray deserializationBuffer = query.value(1).toByteArray();
      QDataStream deserializationStream(&deserializationBuffer, QIODevice::ReadOnly);
      QVector<QVector<MINUTIA>> allUserTemplates;
      deserializationStream >> allUserTemplates;
      for (const auto& isoTemplate : allUserTemplates) {
        allUsers->insert(currentUserId, isoTemplate);
      }
    }
  }
  return true;
}
