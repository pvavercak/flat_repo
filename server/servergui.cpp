#include "servergui.h"
#include "ui_servergui.h"

ServerGUI::ServerGUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ServerGUI)
{
    m_server = new QTcpServer(this);
    ui->setupUi(this);
    ui->port->setValidator(new QIntValidator(this));
}

ServerGUI::~ServerGUI()
{
    delete ui;
    delete m_server;
}

bool ServerGUI::checkip()
{
    this->ip = QHostAddress(ui->ipaddr->text());

    if(QAbstractSocket::IPv4Protocol == this->ip.protocol()){
        this->port = ui->port->text().toInt();
        return true;
    }

    return false;
}

void ServerGUI::toConsole(QString s){
    ui->serverconsole->append(s);
}

void ServerGUI::readFromServer(QTcpSocket& socket)
{
    socket.connectToHost(this->ip, this->port);
    QString s = socket.readAll().data();
    qDebug() << s;
}

void ServerGUI::readyRead()
{
//    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    m_template = qobject_cast<QTcpSocket*>(sender())->readAll();
//    socket->waitForReadyRead(3000);
    qDebug() << m_template.data();
    toConsole("Size of received template: ");
    QString s = QString::number(m_template.size());
    toConsole(s);
}

void ServerGUI::disconnected()
{
    if(!m_server->hasPendingConnections()){
        toConsole("Disconnected...\n");
    }
}
void ServerGUI::on_start_pressed()
{
    if(!checkip()){
        toConsole("Unable to start server!");
        return;
    }

    toConsole("Server initializing...");

    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    if(!m_server->listen(this->ip, this->port)){
        toConsole("Server is not listening!");
    } else {
        toConsole("Server is listening...");
    }
}

void ServerGUI::newConnection(){
    QTcpSocket *socket = m_server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), SLOT(readyRead()));
    connect(socket, SIGNAL(disconnected()), SLOT(disconnected()));

    socket->write("You are connected now...\n");
    socket->flush();
//    socket->waitForBytesWritten();
//    socket->close();
}

