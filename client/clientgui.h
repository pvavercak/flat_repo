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
    explicit ClientGUI(QWidget *parent = nullptr);
    ~ClientGUI();
private slots:
    void attemptConnection();
    void connectedToServer();        
    void disconnectedFromServer();
    void error(QAbstractSocket::SocketError socketError);
    void on_send_pressed();
    void on_scan_pressed();
    void on_connectToServer_pressed();
    void on_disconnect_pressed();

private:
    std::shared_ptr<Client> m_client;
    std::unique_ptr<Ui::ClientGUI> m_ui;
};
#endif // CLIENTGUI_H
