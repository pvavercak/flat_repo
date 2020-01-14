#include "sslserver.h"
#include <QApplication>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>

void check_query(QSqlQueryModel &model){
    if (model.lastError().isValid())
        qDebug() << model.lastError();
    else
        qDebug() << model.query().lastQuery() << " returns OK\n";
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SSLServer w;
    w.show();
//    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
//    db.setHostName("localhost");
//    db.setUserName("pva");
//    db.setPassword("");

//    if(db.open()){
//        qDebug() << "all ok";
//    }
//    else{
//        qDebug() << db.lastError();
//        qDebug() << "sth went wrong";
//    }

//    QSqlQueryModel model;
//    model.setQuery("CREATE DATABASE test_database;");
//    check_query(model);
//    model.setQuery("USE test_database;");
//    check_query(model);
//    model.setQuery("CREATE TABLE stored_users (id INT, name varchar(255), surname varchar(255));");
//    check_query(model);
//    model.setQuery("INSERT INTO stored_users (id, name, surname) VALUES ('1', 'Patrik', 'Vavercak');");
//    check_query(model);

//    QSqlQuery qry;
//    qry.exec("CREATE TABLE stored_users (id, name, surname)");
//    qry.exec("insert into stored_users (id, name, surname) values (1, Patrik, Vavercak)");



//    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//    db.setDatabaseName("/home/pva/SCHOOL/testDatabase.db");
//    if(db.open()){
//        qDebug() << "all ok";
//    }
//    else{
//        qDebug() << "sth went wrong";
//    }

//    QSqlQuery qry;
//    qry.exec("update stored_users set name = 'Pvavercak' where id = 1");
    return a.exec();
}
