#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QSqlDatabase>
#include <memory>

class DatabaseConnection : public QObject
{
    Q_OBJECT
private:
    QSqlDatabase database;
    QString dbPath;
    bool createTable();
public:
    explicit DatabaseConnection(QObject *parent = nullptr);
    ~DatabaseConnection();
    bool setDb();
    int getLastUserId();
    bool writeTemplate(QByteArray templ, int userId = -1);

signals:

public slots:
};

#endif // CONNECTION_H
