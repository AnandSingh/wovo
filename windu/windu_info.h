#ifndef _H_SCM_INFO_
#define _H_SCM_INFO_

/* -------------------------------------------- Header File Inclusion */

#include "..\Include\sipsi\sipsi_parser.h"
#include "..\Include\epg\epg_types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------------------------- Structures/Data Types */
typedef struct SCM_COUNTRY_T_TAG
{

	EvString_t mpszCountry;
	int32_t mCode;
	/*std::list<cTransportStream *> mTransportStreams;*/
	/*cTransportStream *TransportStream(int Channel, unsigned short Id);
	cTransportStream *TransportStream(int Channel);*/
}SCM_COUNTRY_T;

typedef struct MANUAL_SCAN_T_TAG
{
	int32_t FreqHz;
	int32_t Bw;
}MANUAL_SCAN_T;





typedef struct{
	char *RegionName[20];
	int count;
}FreqCountList;                 //To hold the region list used in frequency table

typedef struct FREQ_LIST_T_TAG
{
	int32_t StartFreq;
	int32_t EndFreq;
	int32_t Bandwidth;
}FREQ_LIST_T;


typedef struct FREQ_TABLE_T_TAG
{
	//EvString_t name;
	wchar_t *name;
	struct FREQ_LIST_T_TAG *list;
	int32_t count;
}FREQ_TABLE_T;

#define FREQ_LIST_COUNT(x) (sizeof(x)/sizeof(struct FREQ_LIST_T_TAG))

typedef int ( __stdcall *SCM_STATUS_CALLBACK_T)(dword_t dw, void *pData, void *pParam); 

///////////meher///////////////
/*
typedef struct BDA_INIT_INFO_T_TAG
{
	uint8_t nbDevices;
	uint8_t *TunerName[10];
	void *m_pFilterGraph;
	void *pSource;
}BDA_INIT_INFO_T;*/

#define TUNER_MAX_PATH   260 /* max. length of full pathname */
#define TUNER_MAX_NUM      8

typedef struct BDA_TUNER_INFO_T_TAG
{
	char  *acItemName[TUNER_MAX_NUM];
	int acItemNum;
}BDA_TUNER_INFO_T;

typedef struct DIB_SDK_INFO_T_TAG
{
	uint8_t *TunerName;
}DIB_SDK_INFO_T;



typedef struct SCM_INFO_T_TAG
{
	uint32_t CurrentScanFreq[1];
	int32_t mLocalTimeOffsetInMinute;
    //uint8_t *TunerInfo;
	void *m_pTunerInfo;
	//BDA_INIT_INFO_T *m_pBdaInfo;
	bool_t mbSDT;
	bool_t mbTuneSuccess;
	bool_t ScanComplete;
	uint32_t mCurrentChannel;
	word_t mPIDTable[32];
	int32_t mNumberOfPID;
	bool_t mbRecording;
	dword_t mdwPID;
	void *MutextForDib;

	int32_t CurrentTuned;
	int32_t TotalTuned;  
	bool_t mpServiceRequest;
	bool_t mScanDone;
	STREAM_DESC_T *m_pSectionInfo[10];
	void *m_pATSCMgtInfo;
	HANDLE hSIPSIDataAvailable;  
	void *pUsrCtx;
	void *pMonitUsrCtx;
	bool_t ( __stdcall *m_pfnScmStatusCallback)(dword_t dw, void *pData, void *pParam); 
	void *pCallbackCtx;
	int16_t mCurrentServiceIdx;
	bool_t mStopScan;
	int16_t waitmillsec;

}SCM_INFO_T,*SCM_INFO_PT;


/*////////////////////////////////////////////////////////////////////////////////////*/


union PID
{
	unsigned short network_PID;
	unsigned short program_map_PID;
};

typedef struct PROGRAM_T_TAG
{
	struct PROGRAM_T_TAG *pNext;
	unsigned short program_number;
	union PID pid;
}PROGRAM_T;

typedef struct PAT_T_TAG
{
	unsigned short transport_stream_id;
	struct PROGRAM_T_TAG *pProgram;
}PAT_T;

///////////////////////////////PMT/////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef struct CONDITIONALACCESSDESCRIPTOR_T_TAG
{
	struct CONDITIONALACCESSDESCRIPTOR_T_TAG *pNext;

	unsigned short CA_system_ID;
	unsigned short CA_PID;

	unsigned char PrivateDataLength;
	unsigned char *private_data_byte;
}CONDITIONALACCESSDESCRIPTOR_T;

typedef struct AUDIOSTREAMDESCRIPTOR_T_TAG
{
	struct AUDIOSTREAMDESCRIPTOR_T_TAG *pNext;
	
	unsigned char free_format_flag;
	unsigned char ID;
	unsigned char layer;
	unsigned char variable_rate_audio_indicator;
}AUDIOSTREAMDESCRIPTOR_T;

typedef struct TELETEXT_T_TAG
{
	struct TELETEXT_T_TAG *pNext;

	unsigned int ISO_639_language_code;
	unsigned char teletext_type;
	unsigned char teletext_magazine_number;
	unsigned char teletext_page_number;
}TELETEXT_T;

typedef struct TELETEXTDESCRIPTOR_T_TAG
{
	struct TELETEXTDESCRIPTOR_T_TAG *pNext;
	struct TELETEXT_T_TAG *pTeletext;
}TELETEXTDESCRIPTOR_T;

typedef struct SUBTITLING_T_TAG
{
	struct SUBTITLING_T_TAG *pNext;
	unsigned int ISO_639_language_code;
	unsigned char subtitling_type;
	unsigned short composition_page_id;
	unsigned short ancillary_page_id;
}SUBTITLING_T;

typedef struct SUBTITLINGDESCRIPTOR_T_TAG
{
	struct SUBTITLINGDESCRIPTOR_T_TAG *pNext;
	struct SUBTITLING_T_TAG *pSubtitling;
}SUBTITLINGDESCRIPTOR_T;

typedef struct ISO639LANGUAGE_T_TAG
{
	struct ISO639LANGUAGE_T_TAG *pNext;
	unsigned int ISO_639_language_code;
	unsigned char audio_type;
}ISO639LANGUAGE_T;

typedef struct ISO639LANGUAGEDESCRIPTOR_T_TAG
{
	struct ISO639LANGUAGEDESCRIPTOR_T_TAG *pNext;
	struct ISO639LANGUAGE_T_TAG *pISO639Language;
}ISO639LANGUAGEDESCRIPTOR_T;


typedef enum STREAMTYPE_T_TAG
{
		STREAMTYPE_NONE=0x00,
		STREAMTYPE_MPEG1_VIDEO=0x01,
		STREAMTYPE_VIDEO=0x02,
		STREAMTYPE_AUDIO1=0x03,		// MPEG-1 Audio
		STREAMTYPE_AUDIO2=0x04,		// MPEG-2 Audio
		STREAMTYPE_PRIVATE=0x06,
		STREAMTYPE_AUDIO_AAC=0x0f,
		STREAMTYPE_AUDIO_AC3_1=0x80, 
		STREAMTYPE_AUDIO_AC3_2=0x81, // AC3
}STREAMTYPE_T;

typedef struct ELEMENTARYSTREAM_T_TAG
{
	
	enum STREAMTYPE_T eStream_type;

	struct ELEMENTARYSTREAM_T_TAG *pNext;

	unsigned char stream_type;
	unsigned short elementray_PID;

	struct AUDIOSTREAMDESCRIPTOR_T_TAG *pAudioStreamDescriptor;
	struct CONDITIONALACCESSDESCRIPTOR_T_TAG *pConditionalAccessDescriptor;
	struct ISO639LANGUAGEDESCRIPTOR_T_TAG *pISO639LanguageDescriptor;
	struct TELETEXTDESCRIPTOR_T_TAG *pTeletextDescriptor;
	struct SUBTITLINGDESCRIPTOR_T_TAG *pSubtitlingDescriptor;
}ELEMENTARYSTREAM_T;

typedef struct PMT_T_TAG
{
	struct ELEMENTARYSTREAM_T_TAG *pElementaryStream;
	unsigned short program_number;
	unsigned short PCR_PID;
}PMT_T;



///////////////////////////////SDT info/////////////////////////////////
//
typedef struct SERVICEDESCRIPTOR_T_TAG
{
	struct SERVICEDESCRIPTOR_T_TAG *pNext;

	unsigned char service_type;

	unsigned char service_provider_name_length;
	char *service_provider_name_char;

	unsigned char service_name_length;
	char *service_name_char;
}SERVICEDESCRIPTOR_T;

typedef struct SERVICE_T_TAG
{
	struct SERVICE_T_TAG *pNext;

	unsigned short service_id;
	unsigned char EIT_schedule_flag;
	unsigned char EIT_present_following_flag;
	unsigned char running_status;
	unsigned char free_CA_mode;

	struct SERVICEDESCRIPTOR_T *pServiceDescriptor;
}SERVICE_T;

typedef struct SDT_T_TAG
{
	unsigned short transport_stream_id;
	unsigned short original_network_id;

	struct SERVICE_T *pService;
}SDT_T;



/////////////////////////////////EIT///////////////////////////////////////////
typedef struct EPG_T_TAG
{
		unsigned short Id;

		uint32_t StartTime;
		uint32_t Duration;
		uint32_t StartDate;
		BOOL mbScrambled;

		TCHAR *pszEventName;
		TCHAR *pszText;
		TCHAR *pszExtendedText;

		BYTE ContentNibbleLevel1;
		BYTE ContentNibbleLevel2;

		unsigned char Rating;

}EPG_T;



int scm_MultiByteToWideChar(char *pMultiByteStr, int cbMultiByte, wchar_t *pWideCharStr, int cchWideChar);
void ScmInfoReceiveCallback(dword_t dw , void * pData);
size_t SizeofEpg(const void *el);

int32_t pfnSipsiCallback(void *pBufAdd,int32_t pBufSize);  /* Sipsi CallBack declaration*/

int32_t pfnVideoCallback(void * pAuInfo);
int32_t pfnAudioCallback(void * pAuInfo);
#ifdef __cplusplus
};
#endif

#else
#error "Multiple inclusions of scm_info.h"
#endif //_H_SCM_INFO_

