#include "clientgui.h"
#include <QApplication>
#include "UFScanner.h"
#include <vector>
#include <iostream>
//#include <extraction.h>
//#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClientGUI w;
    w.show();
    return a.exec();
}
