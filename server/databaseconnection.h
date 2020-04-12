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

#include "extraction_config.h"

class DatabaseConnection : public QObject
{
  Q_OBJECT
private:
  QSqlDatabase m_database;
  bool userIdExists(const quint64 id);
  void userTemplateToBytes(const QVector<QVector<MINUTIA> > &userData, QByteArray* outArray);
public:
  explicit DatabaseConnection(QObject *parent = nullptr);
  ~DatabaseConnection();
  bool setDb(const QString& username, const QString& pass, const QString& host, const QString& dbname);
  bool setDb();
  bool registerUserToDb(const QVector<QVector<MINUTIA> > &userData);
  bool registerUserToDbWithId(const QVector<QVector<MINUTIA> > &userData, const quint64 id);
  bool getUserTemplateByID(const quint64& userId, QVector<QVector<MINUTIA> > *outUserTemplate);
  bool getAllUsersFromDb(QMultiMap<QString, QVector<MINUTIA>>* allUsers);
};
#endif // CONNECTION_H
