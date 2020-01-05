/////////////////////////////////////////////////////////////////////////////
//
// UniFinger Engine SDK 3.6
//
// UFScanner.h
// Header file for UFScanner module
//
// Copyright (C) 2016 Suprema Inc.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _UFSCANNER_H_
#define _UFSCANNER_H_

#ifdef WIN32
# ifdef UFS_EXPORTS
#  define UFS_API __declspec(dllexport) __stdcall
# else
#  define UFS_API __stdcall
# endif
# define UFS_CALLBACK __stdcall
#else
# define UFS_API
# define UFS_CALLBACK
#endif

#ifdef __cplusplus
extern "C" {
#endif


// Status Definition
#define UFS_STATUS						int

// Status Return Values
#define UFS_OK							0
#define UFS_ERROR						-1
#define UFS_ERR_NO_LICENSE				-101
#define UFS_ERR_LICENSE_NOT_MATCH		-102
#define UFS_ERR_LICENSE_EXPIRED			-103
#define UFS_ERR_NOT_SUPPORTED			-111
#define UFS_ERR_INVALID_PARAMETERS		-112
#define UFS_ERR_UNKNOWN_TEMPLATE_FORMAT	-120
#define UFS_ERR_INVALID_ENCRYPTION		-121
// For Scanner
#define UFS_ERR_ALREADY_INITIALIZED		-201
#define UFS_ERR_NOT_INITIALIZED			-202
#define UFS_ERR_DEVICE_NUMBER_EXCEED	-203
#define UFS_ERR_LOAD_SCANNER_LIBRARY	-204
#define UFS_ERR_CAPTURE_RUNNING			-211
#define UFS_ERR_CAPTURE_FAILED			-212
// For Extraction
#define UFS_ERR_NOT_GOOD_IMAGE			-301
#define UFS_ERR_EXTRACTION_FAILED		-302
// For Extraction: Core Detection
#define UFS_ERR_CORE_NOT_DETECTED		-351
#define UFS_ERR_CORE_TO_LEFT			-352
#define UFS_ERR_CORE_TO_LEFT_TOP		-353
#define UFS_ERR_CORE_TO_TOP				-354
#define UFS_ERR_CORE_TO_RIGHT_TOP		-355
#define UFS_ERR_CORE_TO_RIGHT			-356
#define UFS_ERR_CORE_TO_RIGHT_BOTTOM	-357
#define UFS_ERR_CORE_TO_BOTTOM			-358
#define UFS_ERR_CORE_TO_LEFT_BOTTOM		-359
//
#define UFS_ERR_FINGER_TOO_RIGHT		-401
#define UFS_ERR_FINGER_TOO_LEFT			-402
#define UFS_ERR_FINGER_TOO_TOP			-403
#define UFS_ERR_FINGER_TOO_BOTTOM		-404
#define UFS_ERR_FINGER_TIP				-405
//
#define	UFS_ERR_FAKE_FINGER				-221
//
#define UFS_ERR_FINGER_ON_SENSOR		-231

// Parameters
// For Scanner
#define UFS_PARAM_TIMEOUT				201
#define UFS_PARAM_BRIGHTNESS			202
#define UFS_PARAM_SENSITIVITY			203
#define UFS_PARAM_SERIAL				204
#define UFS_PARAM_SDK_VERSION			210
#define UFS_PARAM_SDK_COPYRIGHT			211
#define UFS_PARAM_SCANNING_MODE			220
// For Extraction
#define UFS_PARAM_DETECT_CORE			301
#define UFS_PARAM_TEMPLATE_SIZE			302
#define UFS_PARAM_USE_SIF				311
#define UFS_PARAM_CHECK_ENROLL_QUALITY	321
// For LFD
#define UFS_PARAM_DETECT_FAKE			312
// Misc.
#ifdef WIN32
#define UFS_PARAM_LANGUAGE				401
#endif
#define UFS_PARAM_FPQUALITY_MODE		402
#define UFS_PARAM_LOGLEVEL				403
#define UFS_PARAM_LOGFILE				404

// Scanner Type
#define UFS_SCANNER_TYPE_SFR200			1001
#define UFS_SCANNER_TYPE_SFR300			1002
#define UFS_SCANNER_TYPE_SFR300v2		1003
#define UFS_SCANNER_TYPE_SFR500			1004
#define UFS_SCANNER_TYPE_SFR600			1005
#define UFS_SCANNER_TYPE_SFR410			1006
#define UFS_SCANNER_TYPE_SFR550			1007
#define UFS_SCANNER_TYPE_SFR650			1010

// Template Type
#define UFS_TEMPLATE_TYPE_SUPREMA		2001
#define UFS_TEMPLATE_TYPE_ISO19794_2	2002
#define UFS_TEMPLATE_TYPE_ANSI378		2003

#define UFS_MAX_IMAGE_BUFFER_LENGTH		1000000

#define UF_PACK_SECURITY(x) ((x)&0xF)
#define UF_PACK_QUALITY(x) (((x)&0xFF) << 4)
#define UF_PACK_NFINGERS(x) (((x)&0xF) << 12)
#define UF_PACK_NTEMPLATES(x) (((x)&0xF) << 16)

#define UF_UNPACK_SECURITY(x) ((x)&0xF)
#define UF_UNPACK_QUALITY(x) (((x)>>4)&0xFF)
#define UF_UNPACK_NFINGERS(x) (((x)>>12)&0xF)
#define UF_UNPACK_NTEMPLATES(x) (((x)>>16)&0xF)

typedef void* HUFScanner;

typedef int UFS_CALLBACK UFS_SCANNER_PROC(const char* szScannerID, int bSensorOn, void* pParam);
typedef int UFS_CALLBACK UFS_CAPTURE_PROC(HUFScanner hScanner, int bFingerOn, unsigned char* pImage, int nWidth, int nHeight, int nResolution, void* pParam);

UFS_STATUS UFS_API UFS_Init();
UFS_STATUS UFS_API UFS_Update();
UFS_STATUS UFS_API UFS_Uninit();

UFS_STATUS UFS_API UFS_SetScannerCallback(UFS_SCANNER_PROC* pScannerProc, void* pParam);
UFS_STATUS UFS_API UFS_RemoveScannerCallback();

UFS_STATUS UFS_API UFS_GetScannerNumber(int* pnScannerNumber);
UFS_STATUS UFS_API UFS_GetScannerHandle(int nScannerIndex, HUFScanner* phScanner);
UFS_STATUS UFS_API UFS_GetScannerHandleByID(const char* szScannerID, HUFScanner* phScanner);
UFS_STATUS UFS_API UFS_GetScannerIndex(HUFScanner hScanner, int* pnScannerIndex);
UFS_STATUS UFS_API UFS_GetScannerID(HUFScanner hScanner, char* szScannerID);
UFS_STATUS UFS_API UFS_GetScannerType(HUFScanner hScanner, int* pnScannerType);
UFS_STATUS UFS_API UFS_GetCompanyID(HUFScanner hScanner, char* szCompanyID);

UFS_STATUS UFS_API UFS_GetParameter(HUFScanner hScanner, int nParam, void* pValue);
UFS_STATUS UFS_API UFS_SetParameter(HUFScanner hScanner, int nParam, void* pValue);

UFS_STATUS UFS_API UFS_IsSensorOn(HUFScanner hScanner, int* pbSensorOn);
UFS_STATUS UFS_API UFS_IsFingerOn(HUFScanner hScanner, int* pbFingerOn);

UFS_STATUS UFS_API UFS_CaptureSingleImage(HUFScanner hScanner);
UFS_STATUS UFS_API UFS_StartCapturing(HUFScanner hScanner, UFS_CAPTURE_PROC* pCaptureProc, void* pParam);
UFS_STATUS UFS_API UFS_StartAutoCapture(HUFScanner hScanner, UFS_CAPTURE_PROC* pCaptureProc, void* pParam);
UFS_STATUS UFS_API UFS_IsCapturing(HUFScanner hScanner, int* pbCapturing);
UFS_STATUS UFS_API UFS_AbortCapturing(HUFScanner hScanner);

UFS_STATUS UFS_API UFS_Extract(HUFScanner hScanner, unsigned char* pTemplate, int* pnTemplateSize, int* pnEnrollQuality);
UFS_STATUS UFS_API UFS_ExtractEx(HUFScanner hScanner, int nBufferSize, unsigned char* pTemplate, int* pnTemplateSize, int* pnEnrollQuality);

UFS_STATUS UFS_API UFS_SetEncryptionKey(HUFScanner hScanner, unsigned char* pKey);
UFS_STATUS UFS_API UFS_EncryptTemplate(HUFScanner hScanner, unsigned char* pTemplateInput, int nTemplateInputSize, unsigned char* pTemplateOutput, int* pnTemplateOutputSize);
UFS_STATUS UFS_API UFS_DecryptTemplate(HUFScanner hScanner, unsigned char* pTemplateInput, int nTemplateInputSize, unsigned char* pTemplateOutput, int* pnTemplateOutputSize);

UFS_STATUS UFS_API UFS_GetCaptureImageBufferInfo(HUFScanner hScanner, int* pnWidth, int* pnHeight, int* pnResolution);
UFS_STATUS UFS_API UFS_GetCaptureImageBuffer(HUFScanner hScanner, unsigned char* pImageData);
UFS_STATUS UFS_API UFS_ClearCaptureImageBuffer(HUFScanner hScanner);

UFS_STATUS UFS_API UFS_GetCaptureImageBufferToBMPImageBuffer(HUFScanner hScanner, unsigned char* pImageData, int* pImageDataLength);
UFS_STATUS UFS_API UFS_GetCaptureImageBufferTo19794_4ImageBuffer(HUFScanner hScanner, unsigned char* pImageData, int* pImageDataLength);
UFS_STATUS UFS_API UFS_GetCaptureImageBufferToWSQImageBuffer(HUFScanner hScanner, const float ratio, unsigned char* wsqData, int* wsqDataLen);
UFS_STATUS UFS_API UFS_GetCaptureImageBufferToWSQImageBufferVar(HUFScanner hScanner, const float ratio, unsigned char* wsqData, int* wsqDataLen, int nWidth, int nHeight);

UFS_STATUS UFS_API UFS_SaveCaptureImageBufferToBMP(HUFScanner hScanner, char* szFileName); 
UFS_STATUS UFS_API UFS_SaveCaptureImageBufferTo19794_4(HUFScanner hScanner, char* szFileName);
UFS_STATUS UFS_API UFS_SaveCaptureImageBufferToWSQ(HUFScanner hScanner, const float ratio, char* szFileName);
UFS_STATUS UFS_API UFS_SaveCaptureImageBufferToWSQVar(HUFScanner hScanner, const float ratio, char* szFileName, int nWidth, int nHeight);

UFS_STATUS UFS_API UFS_DecompressWSQBMP(HUFScanner hScanner, char* wsqFile, char* bmpFile);
UFS_STATUS UFS_API UFS_DecompressWSQBMPMem(HUFScanner hScanner, unsigned char* wsqBuffer, int wsqBufferLen, unsigned char* bmpBuffer, int* bmpBufferLen);

UFS_STATUS UFS_API UFS_GetFeatureNumber(HUFScanner hScanner, unsigned char* pTemplate, int nTemplateSize, int* pnFeatureNumber);

UFS_STATUS UFS_API UFS_GetErrorString(UFS_STATUS res, char* szErrorString);

UFS_STATUS UFS_API UFS_SetTemplateType(HUFScanner hScanner, int nTemplateType);
UFS_STATUS UFS_API UFS_GetTemplateType(HUFScanner hScanner, int *nTemplateType);

UFS_STATUS UFS_API UFS_SelectTemplate(HUFScanner hScanner, unsigned char** ppTemplateInput, int* pnTemplateInputSize, int nTemplateInputNum, unsigned char** ppTemplateOutput, int* pnTemplateOutputSize, int nTemplateOutputNum);
UFS_STATUS UFS_API UFS_SelectTemplateEx(HUFScanner hScanner, int nBufferSize, unsigned char** ppTemplateInput, int* pnTemplateInputSize, int nTemplateInputNum, unsigned char** ppTemplateOutput, int* pnTemplateOutputSize, int nTemplateOutputNum);

UFS_STATUS UFS_API UFS_GetFPQuality(HUFScanner hScanner ,unsigned char* pFPImage, int nWidth, int nHeight, int* pnFPQuality);


#ifdef WIN32
UFS_STATUS UFS_API UFS_DrawCaptureImageBuffer(HUFScanner hScanner, HDC hDC, int nLeft, int nTop, int nRight, int nBottom, int bCore);
UFS_STATUS UFS_API UFS_DrawFeatureBuffer(HUFScanner hScanner, HDC hDC, int nLeft, int nTop, int nRight, int nBottom, int bCore);
UFS_STATUS UFS_API UFS_EnrollUI(HUFScanner hScanner, int nTimeout, int nOptions, BYTE* pUF_FIR_Buf, int* pUF_FIR_Buf_Len, BYTE* pISO_FIR_Buf, int* pISO_FIR_Buf_Len, char* pImages_Path, BYTE* pImage_Buf = NULL, int* pImage_Buf_Len= NULL);
UFS_STATUS UFS_API UFS_VerifyUI(HUFScanner hScanner, int nTimeout, int nOptions, int nFPTemplateType, BYTE* pFIR_Buf, int* pFIR_Buf_Len, char* pImage_Name, int* nFingerIndex);
UFS_STATUS UFS_API UFS_CaptureSingleUI(HUFScanner hScanner, int nTimeout, int nOptions, BYTE* pUFImageBuf, int* pUFImageWidth, int* pUFImageHeight, char* pImages_Path , int* nFPQuality);
#endif //WIN32


#ifdef __cplusplus
}
#endif

#endif // _UFSCANNER_H_
