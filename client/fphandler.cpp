#include "fphandler.h"
#include <QDebug>

FpHandler::FpHandler()
{
    scannerIndex = 0;
    errorCode = UFS_OK;
    errorMessage.resize(128);
    capture.resize(320*480);
    CHECK_ERROR(UFS_Init(), false);
    CHECK_ERROR(UFS_GetScannerHandle(scannerIndex, &scanner), false);
}

FpHandler::~FpHandler()
{
    CHECK_ERROR(UFS_Uninit(), true);
}

void FpHandler::chooseScanner(int index)
{
    scannerIndex = index;
    int count = 0;
    CHECK_ERROR(UFS_GetScannerNumber(&count), false);

    switch (count){
    case 0:
        CHECK_ERROR(UFS_GetScannerHandle(0, &scanner), true);
        break;
    case 1:
        CHECK_ERROR(UFS_GetScannerHandle(0, &scanner), false);
        break;
    default:
        CHECK_ERROR(UFS_GetScannerHandle(scannerIndex, &scanner), true);
        break;
    }
}

QString FpHandler::getErrorMessage()
{
    QString s(errorMessage.data());
    return s;
}

void FpHandler::startScan()
{
    capture.clear();
    capture.resize(320*480);
    errorCode = UFS_CaptureSingleImage(scanner);
    CHECK_ERROR(errorCode, false);
    errorCode = UFS_GetCaptureImageBuffer(scanner, capture.data());
    CHECK_ERROR(errorCode, false);
}

QByteArray FpHandler::getScanData()
{
    prependScannedDataSize(static_cast<int>(capture.size()) + HEADERSIZE);
    return QByteArray(reinterpret_cast<const char*>(capture.data()), static_cast<int>(capture.size()));
}

const char* FpHandler::getScanDataInternal()
{
    prependScannedDataSize(static_cast<int>(capture.size()) + HEADERSIZE);
    return reinterpret_cast<const char*>(capture.data());
}

void FpHandler::prependScannedDataSize(int dataSize)
{
    int cipherCount{0};    
    while (dataSize != 0) {
        capture.insert(capture.begin(), static_cast<unsigned char>(dataSize % 10));
        dataSize = dataSize / 10;
        ++cipherCount;
    }
    if(HEADERSIZE > cipherCount) {
        for(int i = 0; i < (HEADERSIZE - cipherCount); ++i) {
            //insert '>' to header to make sure it has the exact lenght
            capture.insert(capture.begin(), static_cast<unsigned char>(62));
        }
    }    
    return;
}

bool FpHandler::CHECK_ERROR(UFS_STATUS err, bool exitOnFailure){
    if(UFS_OK != err){
        UFS_GetErrorString(err, errorMessage.data());
        if(exitOnFailure) {
            exit(err);
        }
        return false;
    }
    return true;
}
