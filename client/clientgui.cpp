#include "clientgui.h"
#include "ui_clientgui.h"
#include <sstream>
#include <bitset>
#include <iostream>
#include <string>
#include <QFileDialog>
#include <QFile>
ClientGUI::ClientGUI(QWidget *parent) :
    QMainWindow(parent),
    m_client(std::shared_ptr<Client>(new Client())),
    m_ui(std::unique_ptr<Ui::ClientGUI>(new Ui::ClientGUI()))
{
    m_ui.get()->setupUi(this);
    connect(m_client.get(), SIGNAL(updateLog(QString)), m_ui.get()->clientconsole, SLOT(append(QString)));
}

ClientGUI::~ClientGUI()
{
}

void ClientGUI::attemptConnection()
{

}

void ClientGUI::connectedToServer()
{
    qDebug() << "Connected to server";
}

void ClientGUI::disconnectedFromServer()
{
    qDebug() << "Disconnected from server";
}

void ClientGUI::error(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Error: " << socketError;
}

void ClientGUI::on_scan_pressed()
{
    m_client.get()->makeScan();
}

void ClientGUI::on_send_pressed()
{
    m_client.get()->writeTemplate();
}

void ClientGUI::on_connectToServer_pressed()
{    
    QString addr{m_ui.get()->ipaddr->text()};
    quint16 port = static_cast<quint16>(m_ui.get()->port->text().toInt());
    m_client.get()->connectionInit(addr, port);
}

void ClientGUI::on_disconnect_pressed()
{
    m_client.get()->disconnectFromHost();
}
