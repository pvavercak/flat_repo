#include "databaseconnection.h"
#include <QFile>
#include <QDir>
#include <QSqlQuery>
#include <QCoreApplication>
#include <QDebug>

DatabaseConnection::DatabaseConnection(QObject *parent) :
    QObject(parent),
    dbPath(qApp->applicationDirPath() + QDir::separator() + ".." +QDir::separator() + "database.sqlite")
{
}

DatabaseConnection::~DatabaseConnection()
{
    if(database.isOpen()){
        database.close();
    }
}

bool DatabaseConnection::setDb()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName(dbPath);
    if (database.open()){
        qDebug() << "database opened";
    } else {
        qDebug() << "error while opening database";
        return false;
    }
    if(!database.tables().contains("registered_users")){
        qDebug() << "database is being set";
        return createTable();
    }
    return true;
}

int DatabaseConnection::getLastUserId()
{
    QSqlQuery query;
     QString queryString("SELECT MAX(id) from registered_users WHERE id=:id");
     query.prepare(queryString);
     query.bindValue(":id", "id");
     if(query.exec()){
         qDebug() << query.value("id").toString();
     } else {
         qDebug() << "error selecting max id";
     }
     return query.value("id").toInt();
}

bool DatabaseConnection::writeTemplate(QByteArray templ, int userId)
{
    QSqlQuery query;
    if(database.isOpen()){
        if(userId < 0){ //automatically incremented user id
            query.prepare("INSERT INTO registered_users (template) VALUES (:data);");
            query.bindValue(":data", templ);
        } else {
            query.prepare("INSERT INTO registered_users (id, template) VALUES (:userid, :data);");
            query.bindValue(":data", templ);
            query.bindValue(":userid", userId);
        }
        return query.exec();
    } else {
        qDebug() << "error when inserting template";
        return false;
    }
}

bool DatabaseConnection::createTable()
{
    QSqlQuery query("CREATE TABLE registered_users(id int(1000) NOT NULL UNIQUE, template MEDIUMBLOB);");
    return query.exec();
}
