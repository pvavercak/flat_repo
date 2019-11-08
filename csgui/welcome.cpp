#include "welcome.h"
#include "ui_welcome.h"
#include <QDebug>

Welcome::Welcome(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Welcome)
{
    ui->setupUi(this);
}

Welcome::~Welcome()
{
    delete ui;
}

void Welcome::on_client_clicked()
{
    ClientGui *cg = new ClientGui();
    cg->show();

}

void Welcome::on_server_clicked()
{
    ServerGui *sq = new ServerGui();
    sq->show();
}
