#ifndef SERVERGUI_H
#define SERVERGUI_H

#include <QMainWindow>
#include <QObject>
#include <QIntValidator>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>

namespace Ui {
class ServerGUI;
}

class ServerGUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServerGUI(QWidget *parent = 0);
    ~ServerGUI();
signals:
public slots:
    void newConnection();
    void readFromServer(QTcpSocket& socket);
    void readyRead();
    void disconnected();
private slots:
    void on_start_pressed();

private:
    bool checkip();
    void toConsole(QString s);
    Ui::ServerGUI *ui;
    QHostAddress ip;
    quint16 port;
    QTcpServer *m_server;
    QByteArray m_template;
};

#endif // SERVERGUI_H
