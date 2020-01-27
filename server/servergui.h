#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QMainWindow>
#include <QTextEdit>
#include <memory>
#include "server.h"

namespace Ui
{
    class Server;
}

class ServerGUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit ServerGUI(QWidget *parent = 0);
    ~ServerGUI();
private slots:
    void on_start_server_pressed();
    void on_terminate_pressed();
    void showImage(QByteArray arr);
    void updateClientListSlot(const QString addr, const QString port, const QString sd);
signals:
    void bla();

private:
    std::shared_ptr<Ui::Server> m_ui;
    std::shared_ptr<QTextEdit> m_console;
    std::shared_ptr<Server> m_server;
};

#endif // SSLSERVER_H
