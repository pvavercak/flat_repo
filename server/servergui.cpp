#include "servergui.h"
#include "ui_server.h"
#include <QImage>

ServerGUI::ServerGUI(QWidget *parent) :
    QMainWindow(parent),    
    m_ui(std::shared_ptr<Ui::Server>(new Ui::Server())),
    m_server(std::shared_ptr<Server>(new Server()))
{
    m_ui.get()->setupUi(this);
    m_ui.get()->imagebrowser->setStyleSheet("background-color: white;");    
    m_ui.get()->clientTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_ui.get()->clientTable->setHorizontalHeaderLabels(QStringList() << "Client information");
    m_ui.get()->clientTable->insertRow (m_ui.get()->clientTable->rowCount());
    m_ui.get()->clientTable->insertColumn (m_ui.get()->clientTable->columnCount());
    m_ui.get()->clientTable->insertColumn (m_ui.get()->clientTable->columnCount());
    m_ui.get()->clientTable->insertColumn (m_ui.get()->clientTable->columnCount());
    QObject::connect(m_server.get(), SIGNAL(updateLog(QString)), m_ui.get()->output, SLOT(append(QString)));
    connect(m_server.get(), SIGNAL(sendImage(QByteArray)), this, SLOT(showImage(QByteArray)));    
    connect(m_server.get(), SIGNAL(sendImage(QPixmap)), this, SLOT(showImage(QPixmap)));
    connect(m_server.get(), SIGNAL(updateClientList(QVector<QSslSocket*>)), this, SLOT(updateClientListSlot(QVector<QSslSocket*>)));
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

void ServerGUI::showImage(QPixmap img)
{
    m_ui.get()->imagebrowser->setPixmap(img);
}

void ServerGUI::updateClientListSlot(QVector<QSslSocket*> sockets)
{
    m_ui.get()->clientTable->clearContents();
    m_ui.get()->clientTable->setRowCount(0);
    for (auto& socket  : sockets){
        m_ui.get()->clientTable->insertRow (m_ui.get()->clientTable->rowCount());
        m_ui.get()->clientTable->setItem( m_ui.get()->clientTable->rowCount()-1,
                                 0,
                                 new QTableWidgetItem(socket->peerAddress().toString()));
        m_ui.get()->clientTable->setItem( m_ui.get()->clientTable->rowCount()-1,
                                 1,
                                 new QTableWidgetItem(QString::number(socket->peerPort())));
        m_ui.get()->clientTable->setItem( m_ui.get()->clientTable->rowCount()-1,
                                 2,
                                 new QTableWidgetItem(QString::number(socket->socketDescriptor())));
    }
}
