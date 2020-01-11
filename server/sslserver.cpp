#include "sslserver.h"
#include "ui_sslserver.h"
#include <iostream>
#include <QDebug>

SSLServer::SSLServer(QWidget *parent) :
    QMainWindow(parent),
    m_server(new Server()),
    ui(new Ui::SSLServer)
{
    this->ui.get()->setupUi(this);
    this->console = std::shared_ptr<QTextEdit>(ui.get()->output);
    QObject::connect(m_server.get(), SIGNAL(updateLog(QString)), ui.get()->output, SLOT(append(QString)));
}

SSLServer::~SSLServer()
{
}

void SSLServer::on_run_pressed()
{
    //this->console.get()->append("S");
    this->m_server.get()->initialize(QHostAddress::LocalHost, 4040);
}

void SSLServer::on_clear_pressed()
{
    this->console.get()->clear();
}

void SSLServer::on_terminate_pressed()
{
    this->m_server.get()->terminate();
}

