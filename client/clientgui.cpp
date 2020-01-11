#include "clientgui.h"
#include "ui_clientgui.h"
#include <sstream>

ClientGUI::ClientGUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientGUI),
    m_client(new Client())
{
    ui->setupUi(this);
    connect(m_client, &Client::connected, this, &ClientGUI::connectedToServer);
    connect(m_client, &Client::attemptConnect, this, &ClientGUI::attemptConnection);
    //connect(m_client, &Client::disconnected, this, &ClientGUI::disconnectedFromServer);
    connect(m_client, &Client::error, this, &ClientGUI::error);
}

ClientGUI::~ClientGUI()
{
}

void ClientGUI::attemptConnection()
{
    m_client->connectToServer();
}

void ClientGUI::connectedToServer()
{
    qDebug() << "Connected to server\n";
}

void ClientGUI::messageReceived(const QString &sender, const QString &text)
{

}

void ClientGUI::sendMessage()
{

}

void ClientGUI::disconnectedFromServer()
{

}

void ClientGUI::error(QAbstractSocket::SocketError socketError)
{

}

void ClientGUI::on_send_pressed()
{
    std::unique_ptr<FpHandler> handler{new FpHandler()};
    handler.get()->startScan();
    QByteArray receivedData{QByteArray(handler.get()->getScanData())};
    std::stringstream scanSize;
    scanSize << "expect" << receivedData.size();
    QByteArray header(scanSize.str().c_str());
    if(m_client->writeHeader(header, header.size())){
        //QByteArray xyz("abcdertvfdsfdertfcdccd");
        m_client->writeTemplate(receivedData);
    }
    //qDebug() << receivedData;
}

void ClientGUI::on_connectToServer_pressed()
{
    QString addressToCheck{ui->ipaddr->text()};
    if(m_client->checkIp(addressToCheck, ui->port->text().toInt())){
       emit attemptConnection();
    } else {
        ui->clientconsole->append("It's not possible to connect with such a configuration.");
    }
}

void ClientGUI::on_disconnect_pressed()
{
    this->m_client->disconnectFromHost();
}
