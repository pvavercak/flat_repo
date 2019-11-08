#ifndef CLIENTGUI_H
#define CLIENTGUI_H

#include <QMainWindow>
#include <QObject>
#include <QHostAddress>
#include <QAbstractSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class ClientGui; }
QT_END_NAMESPACE
class ClientGui : public QMainWindow
{
    Q_OBJECT

public:
    ClientGui(QWidget *parent = nullptr);
    ~ClientGui();
    void append2console(QString s);

private slots:
    void on_Connect_pressed();

private:
    Ui::ClientGui *clientui;
    bool checkip(QString ip);
};

#endif // CLIENTGUI_H
