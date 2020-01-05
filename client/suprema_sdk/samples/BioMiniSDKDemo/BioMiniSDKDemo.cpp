#ifndef WIN32
	#include <unistd.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <memory.h>

	typedef int		BOOL;
	#define TRUE	1
	#define FALSE	0
#else
	#include <windows.h>
	#include <stdio.h>
#endif
//
#include "UFScanner.h"
#include "UFMatcher.h"


#define MAX_TEMPLATE_SIZE	1024
#define MAX_TEMPLATE_NUM	30

#define MAX_TEMPLATE_INPUT_NUM	4
#define MAX_TEMPLATE_OUTPUT_NUM	2

HUFMatcher m_hMatcher;

char m_strError[128];

unsigned char* m_template[MAX_TEMPLATE_NUM]; 
int m_template_size[MAX_TEMPLATE_NUM];
unsigned char* m_template2[MAX_TEMPLATE_NUM]; 
int m_template_size2[MAX_TEMPLATE_NUM];
int m_template_num;

unsigned char* m_enrolltemplate[MAX_TEMPLATE_NUM];
int m_enrolltemplateSize[MAX_TEMPLATE_NUM];

int		m_nBrightness;
int		m_nSensitivity;
BOOL	m_bDetectCore;
int		m_nSecurityLevel;
int		m_nTimeout;
BOOL	m_bFastMode;
int		m_nEnrollQuality;
int		m_nSelectID;
int		m_nCurScannerIndex;


/////////////////////////////////////////////////////////////////////////////
#define AddMessage printf

void GetScannerTypeString(int nScannerType, char* strScannerType)
{
	switch (nScannerType) {
	case UFS_SCANNER_TYPE_SFR200:
		sprintf(strScannerType, "SFR200");
		break;
	case UFS_SCANNER_TYPE_SFR300:
		sprintf(strScannerType, "SFR300");
		break;
	case UFS_SCANNER_TYPE_SFR300v2:
		sprintf(strScannerType, "SFR300v2");
		break;
	case UFS_SCANNER_TYPE_SFR500:
		sprintf(strScannerType, "SFR500");
		break;
	case UFS_SCANNER_TYPE_SFR600:
		sprintf(strScannerType, "SFR600");
		break;
	default:
		sprintf(strScannerType, "Error");
		break;
	}
}

BOOL GetCurrentScannerHandle(HUFScanner* phScanner)
{
	int nCurScannerIndex;
	UFS_STATUS ufs_res;

	nCurScannerIndex = m_nCurScannerIndex;
	ufs_res = UFS_GetScannerHandle(nCurScannerIndex, phScanner);
	if (ufs_res == UFS_OK) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void GetCurrentScannerSettings()
{
	HUFScanner hScanner;
	int value;

	if (!GetCurrentScannerHandle(&hScanner)) {
		AddMessage("There's no scanner detected.\n");
		return;
	}

	printf("\n");
	printf("Scanner Parameters:\n");
	printf("-------------------\n");

	// Unit of timeout is millisecond
	UFS_GetParameter(hScanner, UFS_PARAM_TIMEOUT, &value);
	m_nTimeout = value / 1000;
	printf("Timeout = %d sec\n", m_nTimeout);

	UFS_GetParameter(hScanner, UFS_PARAM_BRIGHTNESS, &value);
	m_nBrightness = value;
	printf("Brightness = %d\n", m_nBrightness);

	UFS_GetParameter(hScanner, UFS_PARAM_SENSITIVITY, &value);
	m_nSensitivity = value;
	printf("Sensitivity = %d\n", m_nSensitivity);

	UFS_GetParameter(hScanner, UFS_PARAM_DETECT_CORE, &value);
	m_bDetectCore = value;
	printf("Detect Core = %d\n", m_bDetectCore);
	
	UFS_GetParameter(hScanner, UFS_PARAM_DETECT_FAKE, &value);
	printf("Detect Fake = %d\n", value);

	printf("-------------------\n");
}

void GetMatcherSettings(HUFMatcher hMatcher)
{
	int value;

	printf("\n");
	printf("Matcher Parameters:\n");
	printf("-------------------\n");

	// Security level ranges from 1 to 7
	UFM_GetParameter(hMatcher, UFM_PARAM_SECURITY_LEVEL, &value);
	m_nSecurityLevel = value;
	printf("Security Level = %d\n", m_nSecurityLevel);

	UFM_GetParameter(hMatcher, UFM_PARAM_FAST_MODE, &value);
	m_bFastMode = value;
	printf("Fast Mode = %d\n", m_bFastMode);

	printf("-------------------\n");
}
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
void UpdateScannerList()
{
	UFS_STATUS ufs_res;
	int nScannerNumber;
	int i;

	ufs_res = UFS_GetScannerNumber(&nScannerNumber);
	if (ufs_res != UFS_OK) {
		UFS_GetErrorString(ufs_res, m_strError);
		AddMessage("UFS_GetScannerNumber: %s\r\n", m_strError);
		return;
	}

	for (i = 0; i < nScannerNumber; i++) {
		HUFScanner hScanner;
		int nScannerType;
		char strScannerType[64];
		char strID[64];
		char CID[8];

		ufs_res = UFS_GetScannerHandle(i, &hScanner);
		if (ufs_res != UFS_OK) {
			continue;
		}
		UFS_GetScannerType(hScanner, &nScannerType);
		UFS_GetScannerID(hScanner, strID);
		GetScannerTypeString(nScannerType, strScannerType);

		AddMessage("Scanner %d: %s ID: %s \r\n", i, strScannerType, strID);
	}

	if (nScannerNumber > 0) {
		m_nCurScannerIndex = 0;
		GetCurrentScannerSettings();
	}
}

int UFS_CALLBACK ScannerProc(const char* szScannerID, int bSensorOn, void* pParam)
{
	if (bSensorOn) {
		// We cannot call UpdateData() directly from the different thread,
		// so we use PostMessage() to call UpdateScannerList() indirectly
		AddMessage("\n");
		AddMessage("----------------------------------------\n");
		AddMessage("Scanner (ID = %s) is connected\n", szScannerID);
		AddMessage("----------------------------------------\n");
	} else {
		AddMessage("\n");
		AddMessage("----------------------------------------\n");
		AddMessage("Scanner (ID = %s) is disconnected\n", szScannerID);
		AddMessage("----------------------------------------\n");
	}

	return 1;
}

void OnInit() 
{
	/////////////////////////////////////////////////////////////////////////////
	// Initilize scanner module and get scanner list
	/////////////////////////////////////////////////////////////////////////////
	UFS_STATUS ufs_res;
	int nScannerNumber;

	ufs_res = UFS_Init();
	if (ufs_res == UFS_OK) {
		AddMessage("UFS_Init: OK\r\n");
	} else {
		UFS_GetErrorString(ufs_res, m_strError);
		AddMessage("UFS_Init: %s\r\n", m_strError);
		return;
	}

	ufs_res = UFS_SetScannerCallback(ScannerProc, NULL);
	if (ufs_res == UFS_OK) {
		AddMessage("UFS_SetScannerCallback: OK\r\n");
	} else {
		UFS_GetErrorString(ufs_res, m_strError);
		AddMessage("UFS_SetScannerCallback: %s\r\n", m_strError);
		return;
	}

	ufs_res = UFS_GetScannerNumber(&nScannerNumber);
	if (ufs_res == UFS_OK) {
		AddMessage("UFS_GetScannerNumber: %d\r\n", nScannerNumber);
	} else {
		UFS_GetErrorString(ufs_res, m_strError);
		AddMessage("UFS_GetScannerNumber: %s\r\n", m_strError);
		return;
	}
	
	UpdateScannerList();
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	// Create one matcher
	/////////////////////////////////////////////////////////////////////////////
	UFM_STATUS ufm_res;

	ufm_res = UFM_Create(&m_hMatcher);
	if (ufm_res == UFM_OK) {
		AddMessage("UFM_Create: OK\r\n");
	} else {
		UFM_GetErrorString(ufm_res, m_strError);
		AddMessage("UFM_Create: %s\r\n", m_strError);
		return;
	}

	GetMatcherSettings(m_hMatcher);
	/////////////////////////////////////////////////////////////////////////////
}

void OnUpdate() 
{
	UFS_STATUS ufs_res;

	ufs_res = UFS_Update();

	if (ufs_res == UFS_OK) {
		AddMessage("UFS_Update: OK\r\n");
		UpdateScannerList();
	} else {
		UFS_GetErrorString(ufs_res, m_strError);
		AddMessage("UFS_Update: %s\r\n", m_strError);
	}	
}

void OnUninit() 
{
	/////////////////////////////////////////////////////////////////////////////
	// Uninit scanner module
	/////////////////////////////////////////////////////////////////////////////
	UFS_STATUS ufs_res;

	ufs_res = UFS_Uninit();

	if (ufs_res == UFS_OK) {
		AddMessage("UFS_Uninit: OK\r\n");
	} else {
		UFS_GetErrorString(ufs_res, m_strError);
		AddMessage("UFS_Uninit: %s\r\n", m_strError);
	}
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	// Delete matcher
	/////////////////////////////////////////////////////////////////////////////
	if (m_hMatcher != NULL) {
		UFM_STATUS ufm_res;

		ufm_res = UFM_Delete(m_hMatcher);
		if (ufm_res == UFM_OK) {
			AddMessage("UFM_Delete: OK\r\n");
		} else {
			UFM_GetErrorString(ufm_res, m_strError);
			AddMessage("UFM_Delete: %s\r\n", m_strError);
		}
		m_hMatcher = NULL;
	}
	/////////////////////////////////////////////////////////////////////////////
}

void OnGetParameters() 
{
	GetCurrentScannerSettings();

	if (m_hMatcher != NULL) {
		GetMatcherSettings(m_hMatcher);
	}
}

void OnSetTemplateType()
{
	HUFScanner hScanner;
	char line[256];
	int value;

	if (m_template_num > 0) {
		AddMessage("Template type cannot be changed if one or more template enrolled\n");
		return;	
	}

	if (!GetCurrentScannerHandle(&hScanner)) {
		AddMessage("There's no scanner detected.\n");
		return;
	}

	printf("\n");
	printf("set template type\n");
	printf("1.suprema type\n");
	printf("2.iso type\n");
	printf("3.ansi178 type\n");
	printf("-------------------\n");

	// Unit of timeout is millisecond
	printf("select number 1,2,3  = ");
	gets(line);
	if (line[0] != '\n') {
		value = atoi(line);

		switch(value){

			case 1:
				UFS_SetTemplateType(hScanner, UFS_TEMPLATE_TYPE_SUPREMA);
				UFM_SetTemplateType(m_hMatcher, UFS_TEMPLATE_TYPE_SUPREMA);
				printf("template type:suprema");
				break;
			case 2:
				UFS_SetTemplateType(hScanner, UFS_TEMPLATE_TYPE_ISO19794_2);
				UFM_SetTemplateType(m_hMatcher, UFS_TEMPLATE_TYPE_ISO19794_2);
				printf("template type:iso");
				break;
			case 3:
				UFS_SetTemplateType(hScanner, UFS_TEMPLATE_TYPE_ANSI378);
				UFM_SetTemplateType(m_hMatcher, UFS_TEMPLATE_TYPE_ANSI378);
				printf("template type:ansi");
				break;
			default:
				printf("check number..\n");
				return;
		}
	}
	printf("-------------------\n");

}

void OnSetParameters() 
{
	HUFScanner hScanner;
	char line[256];
	int value;

	if (!GetCurrentScannerHandle(&hScanner)) {
		AddMessage("There's no scanner detected.\n");
		return;
	}

	printf("\n");
	printf("Scanner Parameters:\n");
	printf("-------------------\n");

	// Unit of timeout is millisecond
	printf("Timeout (sec) = ");
	gets(line);
	if (line[0] != '\n') {
		value = atoi(line);
		value *= 1000;
		UFS_SetParameter(hScanner, UFS_PARAM_TIMEOUT, &value);
		UFS_GetParameter(hScanner, UFS_PARAM_TIMEOUT, &value);
		m_nTimeout = value / 1000;
	}
	//
	printf("Brightness (0~255) = ");
	gets(line);
	if (line[0] != '\n') {
		value = atoi(line);
		UFS_SetParameter(hScanner, UFS_PARAM_BRIGHTNESS, &value);
		UFS_GetParameter(hScanner, UFS_PARAM_BRIGHTNESS, &value);
		m_nBrightness = value;
	}
	//
	printf("Sensitivity (0~7) = ");
	gets(line);
	if (line[0] != '\n') {
		value = atoi(line);
		UFS_SetParameter(hScanner, UFS_PARAM_SENSITIVITY, &value);
		UFS_GetParameter(hScanner, UFS_PARAM_SENSITIVITY, &value);
		m_nSensitivity = value;
	}
	//
	printf("Detect Core (0,1) = ");
	gets(line);
	if (line[0] != '\n') {
		value = atoi(line);
		UFS_SetParameter(hScanner, UFS_PARAM_DETECT_CORE, &value);
		UFS_GetParameter(hScanner, UFS_PARAM_DETECT_CORE, &value);
		m_bDetectCore = value;
	}
	
	printf("Detect Fake (0~5) = ");
	gets(line);
	if (line[0] != '\n') {
		value = atoi(line);
		UFS_SetParameter(hScanner, UFS_PARAM_DETECT_FAKE, &value);
		UFS_GetParameter(hScanner, UFS_PARAM_DETECT_FAKE, &value);	
	}

	printf("-------------------\n");

	printf("\n");
	printf("Matcher Parameters:\n");
	printf("-------------------\n");

	// Security level ranges from 1 to 7
	printf("Security Level (1~7) = ");
	gets(line);
	if (line[0] != '\n') {
		value = atoi(line);
		UFM_SetParameter(m_hMatcher, UFM_PARAM_SECURITY_LEVEL, &value);
		UFM_GetParameter(m_hMatcher, UFM_PARAM_SECURITY_LEVEL, &value);
		m_nSecurityLevel = value;
	}
	//
	printf("Fast Mode (0,1) = ");
	gets(line);
	if (line[0] != '\n') {
		value = atoi(line);
		UFM_SetParameter(m_hMatcher, UFM_PARAM_FAST_MODE, &value);
		UFM_GetParameter(m_hMatcher, UFM_PARAM_FAST_MODE, &value);
		m_bFastMode = value;
	}

	printf("-------------------\n");
}

void OnCaptureSingle() 
{
	HUFScanner hScanner;
	UFS_STATUS ufs_res;

	if (!GetCurrentScannerHandle(&hScanner)) {
		AddMessage("There's no scanner detected.\n");
		return;
	}

	AddMessage("Place a Finger\r\n");

	ufs_res = UFS_CaptureSingleImage(hScanner);

	if (ufs_res == UFS_OK) {
		AddMessage("UFS_CaptureSingleImage: OK\r\n");
	} else {
		UFS_GetErrorString(ufs_res, m_strError);
		AddMessage("UFS_CaptureSingleImage: %s\r\n", m_strError);
	}	
}

int UFS_CALLBACK CaptureProc(HUFScanner hScanner, int bFingerOn, unsigned char* pImage, int nWidth, int nHeight, int nResolution, void* pParam)
{
	AddMessage("Capture callback is called. bFingerOn = %d, Image size = (%d,%d)\n", bFingerOn, nWidth, nHeight);

	return 1;
}

void OnExtractTemplate()
{
	HUFScanner hScanner;
	UFS_STATUS ufs_res;
	int nTemplateSize;
	unsigned char pTemplate[MAX_TEMPLATE_SIZE];
	int nEnrollQuality;

	if (!GetCurrentScannerHandle(&hScanner)) {
		AddMessage("There's no scanner detected.\n");
		return;
	}

	ufs_res = UFS_ExtractEx(hScanner,MAX_TEMPLATE_SIZE,pTemplate,&nTemplateSize,&nEnrollQuality);

	if (ufs_res == UFS_OK) {
		AddMessage("UFS_ExtractEx: OK ; Quality: %d\r\n", nEnrollQuality);
	} else {
		UFS_GetErrorString(ufs_res, m_strError);
		AddMessage("UFS_ExtractEx: %s\r\n", m_strError);
	}
}

void OnStartCapturing() 
{
	HUFScanner hScanner;
	UFS_STATUS ufs_res;

	if (!GetCurrentScannerHandle(&hScanner)) {
		AddMessage("There's no scanner detected.\n");
		return;
	}

	ufs_res = UFS_StartCapturing(hScanner, CaptureProc, NULL);
	if (ufs_res == UFS_OK) {
		AddMessage("UFS_StartCapturing: OK\r\n");
	} else {
		UFS_GetErrorString(ufs_res, m_strError);
		AddMessage("UFS_StartCapturing: %s\r\n", m_strError);
	}	
}

void OnStartAutoCapture() 
{
	HUFScanner hScanner;
	UFS_STATUS ufs_res;

	if (!GetCurrentScannerHandle(&hScanner)) {
		AddMessage("There's no scanner detected.\n");
		return;
	}

	ufs_res = UFS_StartAutoCapture(hScanner, CaptureProc, NULL);
	if (ufs_res == UFS_OK) {
		AddMessage("UFS_StartCapturing: OK\r\n");
	} else {
		UFS_GetErrorString(ufs_res, m_strError);
		AddMessage("UFS_StartCapturing: %s\r\n", m_strError);
	}	
}

void OnAbortCapturing() 
{
	HUFScanner hScanner;
	UFS_STATUS ufs_res;

	if (!GetCurrentScannerHandle(&hScanner)) {
		AddMessage("There's no scanner detected.\n");
		return;
	}

	ufs_res = UFS_AbortCapturing(hScanner);
	if (ufs_res == UFS_OK) {
		AddMessage("UFS_AbortCapturing: OK\r\n");
	} else {
		UFS_GetErrorString(ufs_res, m_strError);
		AddMessage("UFS_AbortCapturing: %s\r\n", m_strError);
	}
}

void OnEnroll() 
{
	HUFScanner hScanner;
	UFS_STATUS ufs_res;
	int nEnrollQuality;

	if(m_template_num+1 == MAX_TEMPLATE_NUM) {
		AddMessage("Template memory is full\r\n");
		return;
	}

	if (!GetCurrentScannerHandle(&hScanner)) {
		AddMessage("There's no scanner detected.\n");
		return;
	}
	UFS_ClearCaptureImageBuffer(hScanner);

	AddMessage("Place a finger\r\n");

    while (TRUE) {
		ufs_res = UFS_CaptureSingleImage(hScanner);
		if (ufs_res != UFS_OK) {
			UFS_GetErrorString(ufs_res, m_strError);
			AddMessage("UFS_CaptureSingleImage: %s\r\n", m_strError);
			return;
		}

		ufs_res = UFS_ExtractEx(hScanner,MAX_TEMPLATE_SIZE, m_template[m_template_num], &m_template_size[m_template_num], &nEnrollQuality);
		if (ufs_res == UFS_OK) {
			AddMessage("UFS_ExtractEx: OK\r\n");
			break;
		} else {
			UFS_GetErrorString(ufs_res, m_strError);
			AddMessage("UFS_ExtractEx: %s\r\n", m_strError);
		}
	}
/*
    if (nEnrollQuality < m_nEnrollQuality * 10 + 40 ) {
		AddMessage("Too low quality [Q:%d]\r\n", nEnrollQuality);
        return;
    }
*/
	AddMessage("Enrollment success (No.%d) [Q:%d]\r\n", m_template_num+1, nEnrollQuality);

    if (m_template_num+1 == MAX_TEMPLATE_NUM) {
		AddMessage("Template memory is full\r\n");
    } else {
		m_template_num++;
	}
}

// four times input for an enrollment (Using Advanced Enroll)
void OnEnrollTwoTemplateAdvanced() 
{
	HUFScanner hScanner;
	UFS_STATUS ufs_res;
	int nEnrollQuality;
	int fingeron;
	int i;
	
	if(m_template_num+1 == MAX_TEMPLATE_NUM) {
		AddMessage("Template memory is full\r\n");
		return;
	}

	if (!GetCurrentScannerHandle(&hScanner)) {
		AddMessage("There's no scanner detected.\n");
		return;
	}
	UFS_ClearCaptureImageBuffer(hScanner);

	for( i = 0; i < 2; i++)
	{
		m_enrolltemplate[i] = (unsigned char*)malloc(MAX_TEMPLATE_SIZE);
		m_enrolltemplateSize[i] = 0;
	}
		
	unsigned char* m_EnrollTemplate_input[MAX_TEMPLATE_SIZE];
	int m_EnrollTemplateSize_input[MAX_TEMPLATE_INPUT_NUM];
	unsigned char* m_EnrollTemplate_output[MAX_TEMPLATE_SIZE];
	int m_EnrollTemplateSize_output[MAX_TEMPLATE_OUTPUT_NUM];

	for (i = 0; i < MAX_TEMPLATE_INPUT_NUM; i++) {
		m_EnrollTemplate_input[i] = (unsigned char*)malloc(MAX_TEMPLATE_SIZE);
		memset(m_EnrollTemplate_input[i], 0, MAX_TEMPLATE_SIZE);
		m_EnrollTemplateSize_input[i] = 0;
	}
		
	for (i = 0; i < MAX_TEMPLATE_OUTPUT_NUM; i++) {
		m_EnrollTemplate_output[i] = (unsigned char*)malloc(MAX_TEMPLATE_SIZE);
		memset(m_EnrollTemplate_output[i], 0, MAX_TEMPLATE_SIZE);
		m_EnrollTemplateSize_output[i] = 0;
	}

	int nInput = 0;

	AddMessage("Place a finger\r\n");
	while (TRUE) {
		ufs_res = UFS_CaptureSingleImage(hScanner);
		if (ufs_res != UFS_OK) {
			UFS_GetErrorString(ufs_res, m_strError);
			AddMessage("UFS_CaptureSingleImage: %s\r\n", m_strError);
			continue;
		}
							
		ufs_res = UFS_ExtractEx(hScanner,MAX_TEMPLATE_SIZE, m_EnrollTemplate_input[nInput], &m_EnrollTemplateSize_input[nInput], &nEnrollQuality);

		if (ufs_res == UFS_OK) {
			AddMessage("UFS_ExtractEx: OK\r\n");

			nInput++;
			AddMessage("Getting template (%d/4)\r\n",nInput);
			AddMessage("Remove finger\r\n");

			if(nInput == 4)
			{
				UFS_SelectTemplate(hScanner, m_EnrollTemplate_input, m_EnrollTemplateSize_input, 4, m_EnrollTemplate_output, m_EnrollTemplateSize_output, 2);
				memcpy(m_enrolltemplate[0], m_EnrollTemplate_output[0], m_EnrollTemplateSize_output[0]);
				m_enrolltemplateSize[0] = m_EnrollTemplateSize_output[0];
				memcpy(m_enrolltemplate[1], m_EnrollTemplate_output[1], m_EnrollTemplateSize_output[1]);
				m_enrolltemplateSize[1] = m_EnrollTemplateSize_output[1];
				
				break;
			}
			
			while(1) {
				ufs_res = UFS_IsFingerOn(hScanner, &fingeron);
				if(fingeron == 0) {
					AddMessage("Place a finger\r\n");
					break;
				}
			}
		} 
		else 
		{
			AddMessage("Extraction Error\r\n");
		}
	}

	for (i = 0; i < MAX_TEMPLATE_INPUT_NUM; i++) {
		free(m_EnrollTemplate_input[i]);
	}		
	for (i = 0; i < MAX_TEMPLATE_OUTPUT_NUM; i++) {
		free(m_EnrollTemplate_output[i]);
	}
	
	if(m_enrolltemplateSize[0] != 0) {
		if (m_template_num+1 == MAX_TEMPLATE_NUM) {
			AddMessage("Template memory is full\r\n");
		} else {
			memcpy(m_template[m_template_num], m_enrolltemplate[0], m_enrolltemplateSize[0]);
			m_template_size[m_template_num] = m_enrolltemplateSize[0];
			memcpy(m_template2[m_template_num], m_enrolltemplate[1], m_enrolltemplateSize[1]);
			m_template_size2[m_template_num] = m_enrolltemplateSize[1];

			m_template_num++;
			AddMessage("Enrollment success (No.%d) [Q:%d]\r\n", m_template_num, nEnrollQuality);
		}
	} else {
		AddMessage("Enrollment is failed\r\n");
	}
				
	for( i = 0; i < 2; i++)	{
		free(m_enrolltemplate[i]);
	}
}

void OnVerify() 
{
	HUFScanner hScanner;
	UFS_STATUS ufs_res;
	UFM_STATUS ufm_res;
	unsigned char Template[MAX_TEMPLATE_SIZE];
	int TemplateSize;
	int nEnrollQuality;
	int bVerifySucceed;
	char line[256];
	int value;

	if (!GetCurrentScannerHandle(&hScanner)) {
		AddMessage("There's no scanner detected.\n");
		return;
	}
	UFS_ClearCaptureImageBuffer(hScanner);

	if (m_template_num <= 0) {
	    AddMessage("There no enrolled finger.\n");
		return;
	} else {
	    AddMessage("There are %d fingers enrolled.\n", m_template_num);
	}
	AddMessage("Select Enroll ID (1~%d): ", m_template_num);
	gets(line);
	if (line[0] == '\n') {
		return;
	}
	value = atoi(line);
	if ((value <= 0) || (value > m_template_num)) {
		return;
	}
	m_nSelectID = value-1;

    AddMessage("Place a finger\r\n");

	ufs_res = UFS_CaptureSingleImage(hScanner);
	if (ufs_res != UFS_OK) {
		UFS_GetErrorString(ufs_res, m_strError);
		AddMessage("UFS_CaptureSingleImage: %s\r\n", m_strError);
		return;
	}

	ufs_res = UFS_ExtractEx(hScanner, MAX_TEMPLATE_SIZE, Template, &TemplateSize, &nEnrollQuality);
	if (ufs_res == UFS_OK) {
	} else {
		UFS_GetErrorString(ufs_res, m_strError);
		AddMessage("UFS_ExtractEx: %s\r\n", m_strError);
		return;
	}

	ufm_res = UFM_Verify(m_hMatcher, Template, TemplateSize, m_template[m_nSelectID], m_template_size[m_nSelectID], &bVerifySucceed);
	if (ufm_res != UFM_OK) {
		UFM_GetErrorString(ufm_res, m_strError);
		AddMessage("UFM_Verify:%s\r\n",m_strError);
		return;
	}

	if (bVerifySucceed) {
		AddMessage("Verification succeed (No.%d)\r\n", m_nSelectID+1);
	} else {
		if(m_template_size2[m_nSelectID] != 0) {
			ufm_res = UFM_Verify(m_hMatcher, Template, TemplateSize, m_template2[m_nSelectID], m_template_size2[m_nSelectID], &bVerifySucceed);
			if (ufm_res != UFM_OK) {
				UFM_GetErrorString(ufm_res, m_strError);
				AddMessage("UFM_Verify:%s\r\n", m_strError);
				return;
			}
			if (bVerifySucceed) {
				AddMessage("Verification succeed (No.%d)\r\n", m_nSelectID+1);
			} else {
				AddMessage("Verification failed\r\n");
			}
		} else {
			AddMessage("Verification failed\r\n");
		}
	}
}

void OnIdentify() 
{
	HUFScanner hScanner;
	UFS_STATUS ufs_res;
	UFM_STATUS ufm_res;
	unsigned char Template[MAX_TEMPLATE_SIZE];
	int TemplateSize;
	int nEnrollQuality;
	int i, j = 0;
	int nMaxTemplateNum = 0;

	if (!GetCurrentScannerHandle(&hScanner)) {
		AddMessage("There's no scanner detected.\n");
		return;
	}
	
	//
	
	unsigned char* template_all[MAX_TEMPLATE_NUM * 2];
  int templateSize_all[MAX_TEMPLATE_NUM * 2];
	int nindex[MAX_TEMPLATE_NUM * 2];

	for (i = 0; i < m_template_num * 2; i++) {
		template_all[i] = (unsigned char*)malloc(MAX_TEMPLATE_SIZE);
		memset(template_all[i], 0, MAX_TEMPLATE_SIZE);
		templateSize_all[i] = 0;
	}

	for( i = 0; i < m_template_num * 2; i++) {
		if(i < m_template_num) {
			if(m_template_size[i] != 0) {
				memcpy(template_all[j], m_template[i], m_template_size[i]);
				templateSize_all[j] = m_template_size[i];
				nindex[j] = i;
				j++;
			}
		} else {
			if(m_template_size2[i-m_template_num] != 0) {
				memcpy(template_all[j], m_template2[i-m_template_num], m_template_size2[i-m_template_num]);
				templateSize_all[j] = m_template_size2[i-m_template_num];
				nindex[j] = i - m_template_num;
				j++;
			}
		}
	}

	nMaxTemplateNum = j;
	
	//
	UFS_ClearCaptureImageBuffer(hScanner);

    AddMessage("Place a finger\r\n");

	ufs_res = UFS_CaptureSingleImage(hScanner);
	if (ufs_res != UFS_OK) {
		UFS_GetErrorString(ufs_res, m_strError);
		AddMessage("UFS_CaptureSingleImage: %s\r\n", m_strError);
		return;
	}

	ufs_res = UFS_ExtractEx(hScanner, MAX_TEMPLATE_SIZE, Template, &TemplateSize, &nEnrollQuality);
	if (ufs_res == UFS_OK) {
	} else {
		UFS_GetErrorString(ufs_res, m_strError);
		AddMessage("UFS_ExtractEx: %s\r\n", m_strError);
		return;
	}

	{
		int nMatchIndex;

		ufm_res = UFM_Identify(m_hMatcher, Template, TemplateSize, template_all, templateSize_all, nMaxTemplateNum, 5000, &nMatchIndex);
		//ufm_res = UFM_IdentifyMT(m_hMatcher, Template, TemplateSize, template_all, templateSize_all, nMaxTemplateNum, 5000, &nMatchIndex);

		if (ufm_res != UFM_OK) {
			UFM_GetErrorString(ufm_res, m_strError);
			AddMessage("UFM_Identify: %s\r\n", m_strError);
			goto errret;
		}

		if (nMatchIndex != -1) {
			AddMessage("Identification succeed (No.%d)\r\n", nindex[nMatchIndex]+1);
		} else {
			AddMessage("Identification failed\r\n");
		}
	}
	
errret:
	for (i = 0; i < m_template_num * 2; i++) {
		free(template_all[i]);
	}
	
}

void OnSaveTemplate() 
{
	char line[256];
	int value;

	if (m_template_num <= 0) {
	    AddMessage("There no enrolled finger.\n");
		return;
	} else {
	    AddMessage("There are %d fingers enrolled.\n", m_template_num);
	}
	AddMessage("Select Enroll ID (1~%d): ", m_template_num);
	gets(line);
	if (line[0] == '\n') {
		return;
	}
	value = atoi(line);
	if ((value <= 0) || (value > m_template_num)) {
		return;
	}
	m_nSelectID = value-1;

	AddMessage("Input file name (*.tmp): ");
	gets(line);
	if (line[0] == '\n') {
		return;
	}

	{
		FILE* fp;
		fp = fopen(line, "wb");
		if (fp == NULL) {
			return;
		}
		fwrite(m_template[m_nSelectID], 1, m_template_size[m_nSelectID], fp);
		fclose(fp);
	}

	AddMessage("Selected template is saved\r\n");
}

void OnSaveImage() 
{
	HUFScanner hScanner;
	char line[256];

	if (!GetCurrentScannerHandle(&hScanner)) {
		AddMessage("There's no scanner detected.\n");
		return;
	}

	AddMessage("Input image file name (*.raw): ");
	gets(line);
	if (line[0] == '\n') {
		return;
	}

	{
		unsigned char* pImageData = NULL;
		int nWidth, nHeight, nResolution;
		FILE* fp;

		UFS_GetCaptureImageBufferInfo(hScanner, &nWidth, &nHeight, &nResolution);

		pImageData = (unsigned char*)malloc(nWidth*nHeight*sizeof(unsigned char));
		
		UFS_GetCaptureImageBuffer(hScanner, pImageData);
		
		fp = fopen(line, "wb");
		if (fp == NULL) {
			return;
		}
		fwrite(pImageData, 1, nWidth*nHeight*sizeof(unsigned char), fp);
		fclose(fp);
		
		free(pImageData);
	}

	AddMessage("Captured image is saved\r\n");
}
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
	int i;

	/////////////////////////////////////////////////////////////////////////////
	for (i = 0; i < MAX_TEMPLATE_NUM; i++) {
		m_template[i] = (unsigned char*)malloc(MAX_TEMPLATE_SIZE);
		m_template2[i] = (unsigned char*)malloc(MAX_TEMPLATE_SIZE);
		memset(m_template[i], 0, MAX_TEMPLATE_SIZE);
		memset(m_template2[i], 0, MAX_TEMPLATE_SIZE);
		m_template_size[i] = 0;
		m_template_size2[i] = 0;
	}
	m_template_num = 0;

	m_hMatcher = NULL;

	m_nBrightness = -1;
	m_nSensitivity = -1;
	m_bDetectCore = FALSE;
	m_nSecurityLevel = 4;
	m_nTimeout = -1;
	m_bFastMode = FALSE;
	m_nEnrollQuality = 0;
	m_nSelectID = -1;
	m_nCurScannerIndex = 0;
	/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	while (TRUE) {
		int command = -1;
		printf("\n");
		printf("=====================================\n");
		printf("Suprema PC SDK Console Demo\n");
		printf("=====================================\n");
		printf(" 1: Init Scanner\n");
		printf(" 2: Uninit Scanner\n");
		printf(" 3: Get Parameters\n");
		printf(" 4: Set Parameters\n");
		printf(" 5: Set Template type\n");
		printf(" 6: Capture Single Image\n");
		printf(" 7: Extract template\n");
		printf(" 8: Start Capturing\n");
		printf(" 9: Start AutoCapture\n");
		printf("10: Abort Capturing\n");
		printf("11: Enroll Finger \n");
		printf("12: Enroll Two Template for a Finger\n");
		printf("13: Verify Finger\n");
		printf("14: Identify Finger\n");
		printf("15: Save Template\n");
		printf("16: Save Image\n");
		printf(" 0: Exit\n");
		printf("=====================================\n");
		printf("Select Command: ");
		{
			char line[256];
			gets(line);
			if (line[0] == 0) {
				continue;
			} else if (line[1] == 0) {
				if ((line[0] < '0') || (line[0] > '9')) {
					continue;
				}
			} else if (line[2] == 0) {
				if ((line[0] < '0') || (line[0] > '9') || (line[1] < '0') || (line[1] > '9')) {
					continue;
				}
			} else {
				continue;
			}
			command = atoi(line);
		}
		printf("===================================\n");
		switch (command) {
		case 1:
			OnInit();
			break;
		case 2:
			OnUninit();
			break;
		case 3:
			OnGetParameters();
			break;
		case 4:
			OnSetParameters();
			break;
		case 5:
			OnSetTemplateType();
			break;
		case 6:
			OnCaptureSingle();
			break;
		case 7:
			OnExtractTemplate();
			break;
		case 8:
			OnStartCapturing();
			break;
		case 9:
			OnStartAutoCapture();
			break;
		case 10:
			OnAbortCapturing();
			break;
		case 11:
			OnEnroll();
			break;
		case 12:
			OnEnrollTwoTemplateAdvanced();
			break;
		case 13:
			OnVerify();
			break;
		case 14:
			OnIdentify();
			break;
		case 15:
			OnSaveTemplate();
			break;
		case 16:
			OnSaveImage();
			break;
		case 0:
			goto errret;
			break;
		default:
			break;
		}
	}
	/////////////////////////////////////////////////////////////////////////////
	
errret:
	/////////////////////////////////////////////////////////////////////////////
	OnUninit();

	for (i = 0; i < MAX_TEMPLATE_NUM; i++) {
		free(m_template[i]);
		free(m_template2[i]);
	}
	/////////////////////////////////////////////////////////////////////////////
	
	return 1;
}
/////////////////////////////////////////////////////////////////////////////
