#ifndef SERVERGUI_H
#define SERVERGUI_H

#include <QMainWindow>
#include <QObject>
#include <QHostAddress>
#include <QAbstractSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class ServerGui; }
QT_END_NAMESPACE
class ServerGui : public QMainWindow
{
    Q_OBJECT

public:
    ServerGui(QWidget *parent = nullptr);
    ~ServerGui();
    void append2console(QString s);

private slots:
    void on_Start_pressed();

private:
    Ui::ServerGui *serverui;
    bool checkip(QString ip);
};

#endif // SERVERGUI_H
