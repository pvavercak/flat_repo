#include "clientgui.h"
#include "ui_clientgui.h"
#include <QFileDialog>
#include <QFile>
ClientGUI::ClientGUI(QWidget *parent) :
    QMainWindow(parent),
    m_client(std::unique_ptr<Client>(new Client())),
    m_ui(new Ui::ClientGUI())
{
    m_ui->setupUi(this);
    connect(m_client.get(), SIGNAL(updateLog(QString)), m_ui->clientconsole, SLOT(append(QString)));
}

ClientGUI::~ClientGUI()
{
  delete m_ui;
}

void ClientGUI::on_scan_pressed()
{
    m_client.get()->addFingerFromScanner();
}

void ClientGUI::on_connectToServer_pressed()
{    
    QString addr{m_ui->ipaddr->text()};
    quint16 port = static_cast<quint16>(m_ui->port->text().toInt());
    m_client.get()->connectionInit(addr, port);
}

void ClientGUI::on_disconnect_pressed()
{
    m_client.get()->disconnectFromHost();
}

void ClientGUI::on_newUserButton_pressed()
{
    m_client.get()->newUser();
}

void ClientGUI::on_registerButton_pressed()
{
    m_client.get()->enrollUser();
}

void ClientGUI::on_clearUserButton_pressed()
{
    m_client.get()->deleteCurrentlyEnrollingUser();
}

void ClientGUI::on_identifyButton_pressed()
{
    m_client.get()->identifyUser();
}

void ClientGUI::on_addFromFileButton_pressed()
{
  const QString imagePath = m_ui->pathToFingerImage->text();
  m_client.get()->addFingerFromFile(imagePath);
}
