#include "clientgui.h"
#include <QApplication>
#include "UFScanner.h"
#include <vector>
#include <iostream>
//#include <extraction.h>
//#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{

//    Extraction extraction;
//    extraction.setCPUOnly(true);
//    cv::Mat img = cv::imread("/home/pva/deleteme/SDK3135/finger.bmp", 0);
//    cv::Mat imgSkel = cv::imread("/home/pva/Documents/skel.bmp", 0);
//    cv::Mat imgSkelBitwise = cv::imread("/home/pva/Documents/bitwiseSkel.bmp", 0);
//    extraction.loadInput(img, imgSkel, , , imgSkelBitwise);
    QApplication a(argc, argv);
    ClientGUI w;
    w.show();
    return a.exec();
}
