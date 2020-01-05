#ifndef CLIENTGUI_H
#define CLIENTGUI_H

#include <QMainWindow>
#include <client.h>
#include <fphandler.h>
#include <memory>

namespace Ui {
    class ClientGUI;
}
class ClientGUI : public QMainWindow
{
    Q_OBJECT
public:
    explicit ClientGUI(QWidget *parent = 0);
    ~ClientGUI();
private slots:
    void attemptConnection();
    void connectedToServer();
    void messageReceived(const QString &sender, const QString &text);
    void sendMessage();
    void disconnectedFromServer();
    void error(QAbstractSocket::SocketError socketError);
    void on_send_pressed();
    void on_connectToServer_pressed();
    void on_disconnect_pressed();

private:
    Client *m_client;
    Ui::ClientGUI *ui;
    FpHandler *m_scannerHandler;
};
#endif // CLIENTGUI_H
