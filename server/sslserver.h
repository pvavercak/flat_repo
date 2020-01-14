#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QMainWindow>
#include <QTextEdit>
#include <memory>
#include "server.h"

namespace Ui {
class SSLServer;
}

class SSLServer : public QMainWindow
{
    Q_OBJECT

public:
    explicit SSLServer(QWidget *parent = 0);
    ~SSLServer();
private slots:
    void on_run_pressed();
    void on_clear_pressed();
    void on_terminate_pressed();
signals:
    void bla();

private:
    std::unique_ptr<Ui::SSLServer> ui;
    std::shared_ptr<QTextEdit> console;
    std::shared_ptr<Server> m_server;
};

#endif // SSLSERVER_H
