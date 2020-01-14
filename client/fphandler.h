#ifndef FPHANDLER_H
#define FPHANDLER_H
#include <UFScanner.h>
#include <vector>
#include <QString>
#include <QByteArray>

class FpHandler
{
public:
    FpHandler();
    ~FpHandler();
    void chooseScanner(int index);
    QString getErrorMessage();
    void startScan();
    QByteArray getScanData();
private:
    UFS_STATUS errorCode;
    HUFScanner scanner;
    std::vector<unsigned char> capture;
    int scannerIndex;
    std::vector<char> errorMessage;
    bool CHECK_ERROR(UFS_STATUS err, bool exitOnFailure);
};

#endif // FPHANDLER_H
