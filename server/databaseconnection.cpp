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

void DatabaseConnection::userTemplateToBytes(const QVector<QVector<uchar> > &userData, QByteArray* outArray)
{
  QDataStream userDataStream(outArray, QIODevice::WriteOnly);
  userDataStream.setVersion(QDataStream::Qt_5_9);
  userDataStream << userData;
}

bool DatabaseConnection::registerUserToDb(const QVector<QVector<uchar>>& userData)
{
  QByteArray userBinary{};
  userTemplateToBytes(userData, &userBinary);
  QSqlQuery query(m_database);
  query.prepare("INSERT INTO user_templates (template) VALUES (?);");
  query.bindValue(0, userBinary);
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
  } else {
    if (query.next()) {
      qDebug() << "Error: User with id " << id << " already registered";
      return true;
    }
  }
  return false;
}

bool DatabaseConnection::registerUserToDbWithId(const QVector<QVector<uchar> > &userData, const quint64 id)
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

bool DatabaseConnection::getUserTemplateByID(const quint64 &userId, QVector<QVector<uchar>>* outUserTemplate)
{
  if (!outUserTemplate) {
    return false;
  }
  QSqlQuery query(m_database);
  query.prepare("SELECT id, template FROM user_templates WHERE id = :searchId;");
  query.bindValue(":searchId", userId);
  if (!query.exec()) {
    qDebug() << "Error: " << query.lastError().text();
    return false;
  } else {
    qDebug() << "success " << query.lastQuery();
    if (query.next()){
      QByteArray deserializationBuffer = query.value(1).toByteArray();
      QDataStream deserializationStream(&deserializationBuffer, QIODevice::ReadOnly);
      deserializationStream >> *outUserTemplate;
    }
  }
  return true;
}

bool DatabaseConnection::getAllUsersFromDb(QMultiMap<QString, QVector<uchar>> *allUsers)
{
  if (!allUsers) {
    return false;
  }
  QSqlQuery query(m_database);
  query.prepare("SELECT id, template FROM user_templates;");
  if (!query.exec()) {
    qDebug() << query.lastError().text();
    return false;
  } else {
    while (query.next()) {
      quint64 currentUserId = static_cast<quint8>(query.value(0).toInt());
      QByteArray deserializationBuffer = query.value(1).toByteArray();
      QDataStream deserializationStream(&deserializationBuffer, QIODevice::ReadOnly);
      QVector<QVector<uchar>> oneUserTemplate;
      deserializationStream >> oneUserTemplate;
      for (const auto& isoTemplate : oneUserTemplate) {
        allUsers->insert(QString::number(currentUserId), isoTemplate);
      }
    }
  }
  return true;
}
