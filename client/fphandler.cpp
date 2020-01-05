#include "fphandler.h"
#include <QDebug>

FpHandler::FpHandler()
{
    scannerIndex = 0;
    errorCode = UFS_OK;
    errorMessage.resize(128);
    capture.resize( 320 * 480 );
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
    errorCode = UFS_CaptureSingleImage(scanner);
    CHECK_ERROR(errorCode, false);
    errorCode = UFS_GetCaptureImageBuffer(scanner, capture.data());
    CHECK_ERROR(errorCode, false);
    qDebug() << capture.size();
}

QByteArray FpHandler::getScanData()
{
    return QByteArray(reinterpret_cast<char*>(capture.data()), capture.size());
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
