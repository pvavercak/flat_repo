#include "servergui.h"
#include "ui_sslserver.h"
#include <iostream>
#include <QDebug>

SSLServer::SSLServer(QWidget *parent) :
    QMainWindow(parent),    
    m_ui(std::shared_ptr<Ui::SSLServer>(new Ui::SSLServer())),
    m_server(std::shared_ptr<Server>(new Server()))
{
    m_ui.get()->setupUi(this);
    m_console = std::shared_ptr<QTextEdit>(m_ui.get()->output);
    QObject::connect(m_server.get(), SIGNAL(updateLog(QString)), m_ui.get()->output, SLOT(append(QString)));
}

SSLServer::~SSLServer()
{
}

void SSLServer::on_run_pressed()
{    
    QString addr{m_ui.get()->input_address->text()};
    quint16 port = static_cast<quint16>(m_ui.get()->input_port->text().toInt());
    m_server.get()->initialize(addr, port);
}

void SSLServer::on_clear_pressed()
{
    m_console.get()->clear();
}

void SSLServer::on_terminate_pressed()
{
    m_server.get()->terminate();
}

