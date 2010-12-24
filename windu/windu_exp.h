
/**
 *  @file scm_exp.h
 *
 *  This header file is part of 
 *
 *  Version:
 */

/*
 *  Copyright (C) 2000-2006. Evude Software Pvt. Ltd
 *  All rights reserved.
 */

#ifndef _H_SCM_EXP_
#define _H_SCM_EXP_

/* -------------------------------------------- Header File Inclusion */

#include "scm_error.h"

#pragma once

/* -------------------------------------------- Structures/Data Types */
#ifdef __cplusplus
extern "C" {
#endif

/* Private Data type to represent States in MobileTv Stack */
enum TV_States { 
        TV_IDLE = 0,
        TV_INIT,
		TV_TUNE,
        TV_SCAN,
        TV_PLAY,
		TV_STOP,
		TV_DEINIT,
		TV_MONIT,
		TV_SCAN_COMPLETE,
		TV_SDT_INFO,
		TV_EPG,
		TV_EPG_INFO,
		TV_SUBTITLE,
		TV_RECORD,
		TV_SCAN_CURRENT_FREQ,
		TV_STOP_TUNE,
        TV_MAX_STATES
};

typedef struct FREQ_RANGE_T_TAG
{
	UINT32 Freq[200];
	UINT32 BW[200];
}FREQ_RANGE_T;

typedef struct TUNE_PARAM_T_TAG
{
	FREQ_RANGE_T FerqRange;
	int FreqCount;
}TUNE_PARAM_T;

typedef struct PLAY_PARAM_T_TAG
{
	uint32_t ServiceId;
	int SectionIdx;
}PLAY_PARAM_T;

typedef struct SIGNAL_INFO_T_TAG
{
	double RFPower; 
	double BER;
	double CN;
	double Strength;
}SIGNAL_INFO_T;


typedef int (*SCM_TIMER_PROC_T)(SIGNAL_INFO_T *signal_info,void *UsrCtx);

typedef struct MONIT_PARAM_T_TAG
{
	SCM_TIMER_PROC_T pfnMonitFunc;
	int NoOfIteration;
	int updateTime;
}MONIT_PARAM_T;


/* -------------------------------------------- Function Declarations */

typedef int (__stdcall *SCM_CALLBACK_FUNC_T)(dword_t dw, void *pData, void *pParam);

/*these are also declred in demux_tssplit.h*/
typedef int32_t (*ESRECEIVECALLBACK)(void *);          /* Elementry Stream CallBack declaration*/
typedef void (*SUBTITLECALLBACK)(void *);              /*  Subtitle Stream Callback*/
typedef int32_t (*SIPSICALLBACK)(void *,int32_t);   /* Sipsi CallBack declaration*/

/* Public Functions */
/*****************************/
SCM_INFO_T * __mobiletv_RegisterCallback(SCM_CALLBACK_FUNC_T pCallbackFunctionm, void *pUsrCtx);
void __mobiletv_RegisterAVFunc(ESRECEIVECALLBACK fnAudioCallback,ESRECEIVECALLBACK fnVideoCallback);
void __mobiletv_RegisterSubtitle(SUBTITLECALLBACK fnSubtitleCallback);


void* __mobiletv_init(void);
void __mobiletv_tune(TUNE_PARAM_T *TuneParam);
void __mobiletv_stop_tune(void);
//void __get_service_list(void);
//void __mobiletv_play_sercive(uint32_t ServiceId);
void __mobiletv_play_sercive(PLAY_PARAM_T *PlayParam);
void __mobiletv_get_epg(void);
void __mobiletv_deinit(void);
void __mobiletv_get_monit(MONIT_PARAM_T *MonitParam,void *pUsrCtx);
void __mobiletv_stop_service(void);
void __mobiletv_subtitle(uint32_t SubPid);
void __mobiletv_record(void);


//void register_info_callback(int(*cmpFunc)(const void*, const void*));
/******************************/



/* Private Functions */
void TV_start_epg();
//TV_upadet_epg
void TV_start_monit(union SCM_DATA_T scmData);
//TV_update_monit
uint16_t TV_init(void);
void TV_tune(union SCM_DATA_T scmData);
void TV_scan(void);
void TV_stop_scan(void);
void TV_play(union SCM_DATA_T scmData);
void TV_change_service(void);
void TV_deinit(void);
void TV_stop(void);
//TV_resume_service();
void TV_idle(void);
void TV_update_epg(union SCM_DATA_T scmData);
void TV_subtitle(union SCM_DATA_T scmData);
void TV_record(void);
void TV_stop_tune(void);

#ifdef __cplusplus
};
#endif
/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////









#else
#error "Multiple inclusions of scm_exp.h"
#endif //_H_SCM_EXP_

