#include "clientgui.h"
#include <QApplication>
#include <QImageReader>
#include <QByteArray>
#include <QDebug>
#include <QString>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  ClientGUI w{};
  w.show();
  return a.exec();
}
