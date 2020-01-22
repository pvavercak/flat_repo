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

//    QByteArray raw_input = "rqwertyuiop";
//    raw_input.prepend("abc");
//    qDebug() << raw_input.at(0);

//    QByteArray ba;

//    QDataStream writer(&ba, QIODevice::WriteOnly);
//    writer.writeBytes(raw_input.constData(), raw_input.length());

//    QDataStream reader(ba);

//    char* raw;
//    uint length;
//    reader.readBytes(raw, length);


//    qDebug() << "raw null? " << (raw == NULL) << " ; length = " << length << endl;
//    qDebug() << raw;
//    qDebug() << raw[0];
//    qDebug() << atoi("0000153600");
//    QString xxx{"0000153600"};
//    qDebug() << xxx.length();

//    const char char_max = (char)(((unsigned char) char(-1)) / 2);

//    int i = 128;
//    char c = (i & char_max);
//    qDebug() << (int)c;

//    uint datSiz= 153600;
//    std::vector<unsigned char> dataWithSize;
//    do {
//        dataWithSize.push_back((unsigned char) datSiz % 10);
//    } while ((datSiz = datSiz / 10) != 0);
//    for (unsigned char ch : dataWithSize){
//        qDebug() << ch;
//    }
    return a.exec();
}
