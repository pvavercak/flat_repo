#include "clientgui.h"
#include "ui_clientgui.h"

ClientGui::ClientGui(QWidget *parent)
    : QMainWindow(parent)
    , clientui(new Ui::ClientGui)
{
    clientui->setupUi(this);
    clientui->ClientConsole->QTextEdit::append("Console:");
}

ClientGui::~ClientGui()
{
    delete clientui;
}

void ClientGui::append2console(QString s)
{
    clientui->ClientConsole->QTextEdit::append(s);

}

void ClientGui::on_Connect_pressed()
{
    if(checkip(clientui->ipaddr->text()))
    {
        append2console("A valid ip address provided");
    } else {
        append2console("Please, provide a valid ip address");
    }
}

bool ClientGui::checkip(QString ip)
{
    return QAbstractSocket::IPv4Protocol == QHostAddress(ip).protocol();
}
