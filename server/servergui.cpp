#include "servergui.h"
#include "ui_sslserver.h"
#include <QImage>

ServerGUI::ServerGUI(QWidget *parent) :
    QMainWindow(parent),    
    m_ui(std::shared_ptr<Ui::Server>(new Ui::Server())),
    m_server(std::shared_ptr<Server>(new Server()))
{
    m_ui.get()->setupUi(this);
    QImage img(320, 480, QImage::Format_Grayscale8);
    img.fill(QColor(255,255,255));
    m_ui.get()->imagebrowser->setPixmap(QPixmap::fromImage(img));

    m_console = std::shared_ptr<QTextEdit>(m_ui.get()->output);
    QObject::connect(m_server.get(), SIGNAL(updateLog(QString)), m_ui.get()->output, SLOT(append(QString)));
    connect(m_server.get(), SIGNAL(sendImage(QByteArray)), this, SLOT(showImage(QByteArray)));
}

ServerGUI::~ServerGUI()
{
}

void ServerGUI::on_start_server_pressed()
{    
    QString addr{m_ui.get()->input_address->text()};
    quint16 port = static_cast<quint16>(m_ui.get()->input_port->text().toInt());
    m_server.get()->initialize(addr, port);    
}

void ServerGUI::on_terminate_pressed()
{
    m_server.get()->terminate();
}

void ServerGUI::showImage(QByteArray arr)
{
    QImage image((unsigned char*)arr.data(), 320, 480, QImage::Format_Grayscale8);
    m_ui.get()->imagebrowser->setPixmap(QPixmap::fromImage(image));
}

