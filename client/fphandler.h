#ifndef FPHANDLER_H
#define FPHANDLER_H
#include <UFScanner.h>
#include <vector>
#include <QString>
#include <QByteArray>

#define HEADERSIZE 15 // in this application, all the fingerprints are sent in format header<0, 15) + rawData<15, end)

class FpHandler
{
public:
    FpHandler();
    ~FpHandler();
    void chooseScanner(int index);
    QString getErrorMessage();
    void startScan();
    QByteArray getScanData();
    void prependScannedDataSize(int dataSize);
private:
    UFS_STATUS errorCode;
    HUFScanner scanner;
    std::vector<unsigned char> capture;
    int scannerIndex;
    std::vector<char> errorMessage;

    //methods
    bool CHECK_ERROR(UFS_STATUS err, bool exitOnFailure);
    const char *getScanDataInternal();
};

#endif // FPHANDLER_H
