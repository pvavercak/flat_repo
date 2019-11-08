#include "servergui.h"
#include "ui_servergui.h"

ServerGui::ServerGui(QWidget *parent)
    : QMainWindow(parent)
    , serverui(new Ui::ServerGui)
{
    serverui->setupUi(this);
    serverui->ServerConsole->QTextEdit::append("Console:");
}

ServerGui::~ServerGui()
{
    delete serverui;
}

void ServerGui::append2console(QString s)
{
    serverui->ServerConsole->QTextEdit::append(s);
}

void ServerGui::on_Start_pressed()
{
    if(checkip(serverui->ipaddr->text())){
        append2console("A valid ip address provided");
    } else {
        append2console("Please, provide a valid ip address");
    }
}

bool ServerGui::checkip(QString ip)
{
    return QAbstractSocket::IPv4Protocol == QHostAddress(ip).protocol();
}
