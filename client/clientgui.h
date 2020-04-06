#ifndef CLIENTGUI_H
#define CLIENTGUI_H

#include <QMainWindow>
#include <client.hpp>
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
    void on_connectToServer_pressed();
    void on_disconnect_pressed();
    void on_scan_pressed();
    void on_newUserButton_pressed();

    void on_registerButton_pressed();

    void on_clearUserButton_pressed();

private:
    std::shared_ptr<Client> m_client;
    Ui::ClientGUI *m_ui;
};
#endif // CLIENTGUI_H
