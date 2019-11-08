#ifndef WELCOME_H
#define WELCOME_H

#include <QMainWindow>

#include <QApplication>
#include "servergui.h"
#include "clientgui.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Welcome; }
QT_END_NAMESPACE

class Welcome : public QMainWindow
{
    Q_OBJECT

public:
    Welcome(QWidget *parent = nullptr);
    ~Welcome();

private slots:
    void on_client_clicked();

    void on_server_clicked();

private:
    Ui::Welcome *ui;
};
#endif // WELCOME_H
