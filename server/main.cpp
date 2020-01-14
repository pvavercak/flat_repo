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
//    db.setDatabaseName("/home/pva/SCHOOL/database.sqlite");
//    if(db.open()){
//        qDebug() << "all ok";
//    }
//    else{
//        qDebug() << "sth went wrong";
//    }

//    QSqlQueryModel qModel;
//    qModel.setQuery("CREATE TABLE registered_users(id int(1000) NOT NULL UNIQUE, template MEDIUMBLOB);");
//    check_query(qModel);
//    QByteArray testData("this is a test");
//    QSqlQuery qry("INSERT INTO registered_users (id, template) VALUES (:userid," + testData + ");");
//    qry.bindValue(":userid", 5);
//    qry.bindValue(":data", testData);
//    qModel.setQuery(qry);
//    check_query(qModel);

    QByteArray f("abc");
    QByteArray b("defgh");
    //f += b;
    f.append(b);
    qDebug() << f.size();

    return a.exec();
}
