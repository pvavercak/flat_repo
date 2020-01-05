#include "clientgui.h"
#include <QApplication>
#include "UFScanner.h"
#include <vector>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClientGUI w;
    w.show();
    return a.exec();
}
