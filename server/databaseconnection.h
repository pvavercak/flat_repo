#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlResult>
#include <QDataStream>
#include <QVector>
#include <QByteArray>
#include <QSqlQuery>
#include <QDebug>
#include <QMultiMap>
#include <memory>

class DatabaseConnection : public QObject
{
  Q_OBJECT
private:
  QSqlDatabase m_database;
  bool userIdExists(const quint64 id);
  void userTemplateToBytes(const QVector<QVector<uchar> > &userData, QByteArray* outArray);
public:
  explicit DatabaseConnection(QObject *parent = nullptr);
  ~DatabaseConnection();
  bool setDb(const QString& username, const QString& pass, const QString& host, const QString& dbname);
  bool setDb();
  bool registerUserToDb(const QVector<QVector<uchar>>& userData);
  bool registerUserToDbWithId(const QVector<QVector<uchar>>& userData, const quint64 id);
  bool getUserTemplateByID(const quint64& userId, QVector<QVector<uchar>>* outUserTemplate);
  bool getAllUsersFromDb(QMultiMap<QString, QVector<uchar>>* allUsers);
};
#endif // CONNECTION_H
