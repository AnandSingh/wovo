
/* -------------------------------------------- Header File Inclusion */
#include "osal_types.h"
#include "osal_stdio.h"
#include "osal_process.h"
#include "osal_memory.h"
#include "osal_string.h"
#include "osal_list.h"

#include "scm_event.h"
#include "scm_info.h"
#include "demux_tssplit.h"
#include "scm_exp.h"
#include "scm_tuner.h"
#include "scm_config.h"
#include "sub_decoder.h"




/* -------------------------------------------- External Global Variables */
extern uint8_t tvCurrentState;


SCM_INFO_T *pScmInfo = NULL;
/* -------------------------------------------- Exported Global Variables */

/* -------------------------------------------- Static Global Variables */

/* -------------------------------------------- Functions */

//SCM_LIST_T Epglist; 
//EVENT_T EventList;



void __mobiletv_RegisterAVFunc(ESRECEIVECALLBACK fnAudioCallback,ESRECEIVECALLBACK fnVideoCallback)
{
	Demux_SetAudioCallBk(fnAudioCallback);
	Demux_SetVideoCallBk(fnVideoCallback);
}


void __mobiletv_RegisterSubtitle(SUBTITLECALLBACK fnSubtitleCallback)
{
//	Demux_SetAudioCallBk(fnSubtitleCallback);
//#ifdef _USRDLL
	RegisterSubtitle_Callback(fnSubtitleCallback);
//#else
//	__RegisterSubtitle_Callback(fnSubtitleCallback);
//#endif
}


SCM_INFO_T * __mobiletv_RegisterCallback(SCM_CALLBACK_FUNC_T pCallbackFunction, void *pUsrCtx)
{
	pScmInfo->m_pfnScmStatusCallback = pCallbackFunction;
	//pScmInfo->pCallbackCtx = pParam;
	/***********Initializations*******************/
	pScmInfo->CurrentTuned = -1;
	pScmInfo->mCurrentChannel = -1;
	pScmInfo->mpServiceRequest = FALSE;
	pScmInfo->mScanDone = FALSE;
	
	//RegisterPsip_Callback(ScmInfoReceiveCallback);
	RegisterSipsi_Callback(ScmInfoReceiveCallback,pScmInfo);
	pScmInfo->hSIPSIDataAvailable = CreateEvent(NULL,FALSE,FALSE,NULL);

	pScmInfo->pUsrCtx = pUsrCtx;
	return pScmInfo;
}

//TV_resume_service
#define BEGINE_TV_STATE_MAP \
	static const TvStateStruct TvStateMap[] = {

#define TV_STATE_MAP_ENTRY(entry) \
	{ (TvStateFunc)(entry)},

#define END_TV_STATE_MAP \
		{ (TvStateFunc)(NULL)} \
	}; \
	return &TvStateMap[0];


const TvStateStruct* GetTvStateMap()
{
	BEGINE_TV_STATE_MAP
		TV_STATE_MAP_ENTRY(TV_idle)         // TV_IDLE
		TV_STATE_MAP_ENTRY(TV_init)         // TV_INIT
		TV_STATE_MAP_ENTRY(TV_tune)         // TV_TUNE
		TV_STATE_MAP_ENTRY(TV_scan)         // TV_SCAN
		TV_STATE_MAP_ENTRY(TV_play)         // TV_PLAY
		TV_STATE_MAP_ENTRY(TV_stop)         // TV_STOP
		TV_STATE_MAP_ENTRY(TV_deinit)       // TV_DEINIT
		TV_STATE_MAP_ENTRY(TV_start_monit)  // TV_MONIT
		TV_STATE_MAP_ENTRY(TV_stop_scan)    // TV_SCAN_COMPLETE
		TV_STATE_MAP_ENTRY(NULL)            // TV_SDT_INFO
		TV_STATE_MAP_ENTRY(TV_start_epg)    // TV_EPG
		TV_STATE_MAP_ENTRY(TV_update_epg)   // TV_EPG_INFO 
		TV_STATE_MAP_ENTRY(TV_subtitle)     // TV_SUBTITLE
		TV_STATE_MAP_ENTRY(TV_record)       // TV_RECORD
		TV_STATE_MAP_ENTRY(NULL)	    	//TV_SCAN_CURRENT_FREQ
		TV_STATE_MAP_ENTRY(TV_stop_tune)	//TV_STOP_SCAN
	END_TV_STATE_MAP
}

void* __mobiletv_init(void)
{
   const TvStateStruct* pTvStateMap;
   EvTaskHandle ThreadHandle;
   uint16_t status;
   union SCM_DATA_T temp;
   SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: Current State ==> %s \n",getStateString(tvCurrentState)));
   EvCreateTask(SCMEngine, NULL, &ThreadHandle);
   tvCurrentState = TV_INIT;
   pTvStateMap = GetTvStateMap();
   temp.pData = NULL;
   status = (*pTvStateMap[tvCurrentState].pTvStateFunc)(temp);
   SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: New State ==> %s \n",getStateString(tvCurrentState)));
   
#ifdef BDA_TUNER
   return (pScmInfo == NULL) ? (NULL) : (pScmInfo->m_pTunerInfo);
#else
   return (pScmInfo == NULL) ? (NULL) : (pScmInfo->TunerInfo);
#endif
}

static TUNE_PARAM_T ScmTuneParam;
void __mobiletv_tune(TUNE_PARAM_T *TuneParam)
{
	//[TO DO:] Store tune param to local 
	{
	BEGIN_TV_TRANSITION_MAP                      // - Current State -
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	 // TV_IDLE
        TV_TRANSITION_MAP_ENTRY (TV_TUNE)		 // TV_INIT
        TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)  // TV_TUNE
        TV_TRANSITION_MAP_ENTRY (TV_TUNE)	     // TV_SCAN
		TV_TRANSITION_MAP_ENTRY (TV_TUNE)	     // TV_PLAY
        TV_TRANSITION_MAP_ENTRY (TV_TUNE)	     // TV_STOP
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)  // TV_DEINIT
        TV_TRANSITION_MAP_ENTRY (TV_TUNE)        // TV_MONIT
		TV_TRANSITION_MAP_ENTRY (TV_TUNE)        // TV_SCAN_COMPLETE
		TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)	 // TV_SDT_INFO
		TV_TRANSITION_MAP_ENTRY (TV_TUNE)		 // TV_EPG
		TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)  // TV_EPG_INFO
		TV_TRANSITION_MAP_ENTRY (TV_TUNE)        // TV_SUBTITLE
		TV_TRANSITION_MAP_ENTRY (TV_TUNE)        // TV_RECORD
		TV_TRANSITION_MAP_ENTRY (TV_TUNE)        // TV_SCAN_CURRENT_FREQ	
		TV_TRANSITION_MAP_ENTRY (TV_TUNE)        // TV_STOP_SCAN	
    END_TV_TRANSITION_MAP((void *)TuneParam)
	}
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: New State ==> %s \n",getStateString(tvCurrentState)));
}

void __mobiletv_stop_tune(void)
{
//[TO DO:] Store tune param to local 
	{
	BEGIN_TV_TRANSITION_MAP                      // - Current State -
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	 // TV_IDLE
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	 // TV_INIT
        TV_TRANSITION_MAP_ENTRY (TV_STOP_TUNE)   // TV_TUNE
        TV_TRANSITION_MAP_ENTRY (TV_STOP_TUNE)	 // TV_SCAN
		TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	 // TV_PLAY
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	 // TV_STOP
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)  // TV_DEINIT
        TV_TRANSITION_MAP_ENTRY (TV_STOP_TUNE)   // TV_MONIT
		TV_TRANSITION_MAP_ENTRY (TV_STOP_TUNE)   // TV_SCAN_COMPLETE
		TV_TRANSITION_MAP_ENTRY (TV_STOP_TUNE)	 // TV_SDT_INFO
		TV_TRANSITION_MAP_ENTRY (TV_STOP_TUNE)	 // TV_EPG
		TV_TRANSITION_MAP_ENTRY (TV_STOP_TUNE)   // TV_EPG_INFO
		TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)  // TV_SUBTITLE
		TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)  // TV_RECORD
		TV_TRANSITION_MAP_ENTRY (TV_STOP_TUNE)   // TV_SCAN_CURRENT_FREQ
		TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)  // TV_STOP_SCAN	
    END_TV_TRANSITION_MAP(NULL)
	}
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: New State ==> %s \n",getStateString(tvCurrentState)));
}
#define _ATSC


#ifdef _ATSC
static PLAY_PARAM_T ScmPlayParam;
static uint32_t ServId;
void __mobiletv_play_sercive(PLAY_PARAM_T *PlayParam)
{
	ServId = PlayParam->ServiceId;
	ScmPlayParam.ServiceId =  PlayParam->ServiceId;
	ScmPlayParam.SectionIdx = PlayParam->SectionIdx;
	{
	BEGIN_TV_TRANSITION_MAP                     // - Current State -
        TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)	// TV_IDLE
        TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)	// TV_INIT
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	// TV_TUNE
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	// TV_SCAN
		TV_TRANSITION_MAP_ENTRY (TV_PLAY)		// TV_PLAY
        TV_TRANSITION_MAP_ENTRY (TV_PLAY)		// TV_STOP
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN) // TV_DEINIT
        TV_TRANSITION_MAP_ENTRY (TV_PLAY)       // TV_MONIT
		TV_TRANSITION_MAP_ENTRY (TV_PLAY)       // TV_SCAN_COMPLETE
		TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	// TV_SDT_INFO
		TV_TRANSITION_MAP_ENTRY (TV_PLAY)	    // TV_EPG
		TV_TRANSITION_MAP_ENTRY (TV_PLAY)       // TV_EPG_INFO
		TV_TRANSITION_MAP_ENTRY (TV_PLAY)       // TV_SUBTITLE_INFO
		TV_TRANSITION_MAP_ENTRY (TV_PLAY)       // TV_RECORD
		TV_TRANSITION_MAP_ENTRY (TV_PLAY)       // TV_SCAN_CURRENT_FREQ	
		TV_TRANSITION_MAP_ENTRY (TV_PLAY)       // TV_STOP_SCAN	
    END_TV_TRANSITION_MAP((void *)&ScmPlayParam)
	}
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: New State ==> %s \n",getStateString(tvCurrentState)));
}
#else

void __mobiletv_play_sercive(uint32_t ServiceId)
{
	ServId = ServiceId;
	{
	BEGIN_TV_TRANSITION_MAP                     // - Current State -
        TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)	// TV_IDLE
        TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)	// TV_INIT
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	// TV_TUNE
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	// TV_SCAN
		TV_TRANSITION_MAP_ENTRY (TV_PLAY)		// TV_PLAY
        TV_TRANSITION_MAP_ENTRY (TV_PLAY)		// TV_STOP
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN) // TV_DEINIT
        TV_TRANSITION_MAP_ENTRY (TV_PLAY)       // TV_MONIT
		TV_TRANSITION_MAP_ENTRY (TV_PLAY)       // TV_SCAN_COMPLETE
		TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	// TV_SDT_INFO
		TV_TRANSITION_MAP_ENTRY (TV_PLAY)	    // TV_EPG
		TV_TRANSITION_MAP_ENTRY (TV_PLAY)       // TV_EPG_INFO
		TV_TRANSITION_MAP_ENTRY (TV_PLAY)       // TV_SUBTITLE_INFO
		TV_TRANSITION_MAP_ENTRY (TV_PLAY)       // TV_RECORD
		TV_TRANSITION_MAP_ENTRY (TV_PLAY)       // TV_SCAN_CURRENT_FREQ	
		TV_TRANSITION_MAP_ENTRY (TV_PLAY)       // TV_STOP_SCAN	
    END_TV_TRANSITION_MAP((void *)&ServId)
	}
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: New State ==> %s \n",getStateString(tvCurrentState)));
}
#endif
void __mobiletv_get_epg(void)
{
	BEGIN_TV_TRANSITION_MAP                     // - Current State -
        TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)	// TV_IDLE
        TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED) // TV_INIT
        TV_TRANSITION_MAP_ENTRY (TV_EPG)		// TV_TUNE
        TV_TRANSITION_MAP_ENTRY (TV_EPG)		// TV_SCAN
		TV_TRANSITION_MAP_ENTRY (TV_EPG)		// TV_PLAY
        TV_TRANSITION_MAP_ENTRY (TV_EPG)		// TV_STOP
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN) // TV_DEINIT
        TV_TRANSITION_MAP_ENTRY (TV_EPG)        // TV_MONIT
		TV_TRANSITION_MAP_ENTRY (TV_EPG)        // TV_SCAN_COMPLETE
		TV_TRANSITION_MAP_ENTRY (TV_EPG)		// TV_SDT_INFO
		TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)	// TV_EPG
		TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED) // TV_EPG_INFO
		TV_TRANSITION_MAP_ENTRY (TV_EPG)        // TV_SUBTITLE_INFO
		TV_TRANSITION_MAP_ENTRY (TV_EPG)        // TV_RECORD
		TV_TRANSITION_MAP_ENTRY (TV_EPG)       // TV_SCAN_CURRENT_FREQ
		TV_TRANSITION_MAP_ENTRY (TV_EPG)       // TV_STOP_TUNE
    END_TV_TRANSITION_MAP(NULL)
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: New State ==> %s \n",getStateString(tvCurrentState)));
}

void __mobiletv_deinit(void)
{
   const TvStateStruct* pTvStateMap;
   union SCM_DATA_T temp;
   temp.pData = NULL;
   tvCurrentState = TV_DEINIT;
   pTvStateMap = GetTvStateMap();
   (*pTvStateMap[tvCurrentState].pTvStateFunc)(temp);
   SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: New State ==> %s \n",getStateString(tvCurrentState)));
#if 0
	BEGIN_TV_TRANSITION_MAP                     // - Current State -
        TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)	// TV_IDLE
        TV_TRANSITION_MAP_ENTRY (TV_DEINIT)		// TV_INIT
        TV_TRANSITION_MAP_ENTRY (TV_DEINIT)		// TV_TUNE
        TV_TRANSITION_MAP_ENTRY (TV_DEINIT)		// TV_SCAN
		TV_TRANSITION_MAP_ENTRY (TV_DEINIT)		// TV_PLAY
        TV_TRANSITION_MAP_ENTRY (TV_DEINIT)		// TV_STOP
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN) // TV_DEINIT
        TV_TRANSITION_MAP_ENTRY (TV_DEINIT)     // TV_MONIT
		TV_TRANSITION_MAP_ENTRY (TV_DEINIT)     // TV_SCAN_COMPLETE
		TV_TRANSITION_MAP_ENTRY (TV_DEINIT)     // TV_SDT_INFO
		TV_TRANSITION_MAP_ENTRY (TV_DEINIT)		// TV_EPG
		TV_TRANSITION_MAP_ENTRY (TV_DEINIT)     // TV_EPG_INFO
		TV_TRANSITION_MAP_ENTRY (TV_DEINIT)     // TV_SUBTITLE
		TV_TRANSITION_MAP_ENTRY (TV_DEINIT)     // TV_RECORD
		TV_TRANSITION_MAP_ENTRY (TV_DEINIT)       // TV_SCAN_CURRENT_FREQ	
		TV_TRANSITION_MAP_ENTRY (TV_DEINIT)       // TV_STOP_TUNE
    END_TV_TRANSITION_MAP(NULL)
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: New State ==> %s \n",getStateString(tvCurrentState)));
#endif
}


MONIT_PARAM_T *pMonitParam = NULL;
void __mobiletv_get_monit(MONIT_PARAM_T *MonitParam,void *pUsrCtx)
{
	if(pMonitParam == NULL)
	{
		pMonitParam = (MONIT_PARAM_T *)Evmalloc(sizeof(MONIT_PARAM_T));
	}
	pMonitParam->pfnMonitFunc = MonitParam->pfnMonitFunc;
	pMonitParam->updateTime = MonitParam->updateTime;
	pMonitParam->NoOfIteration = MonitParam->NoOfIteration;
	pScmInfo->pMonitUsrCtx = pUsrCtx;
	{
		BEGIN_TV_TRANSITION_MAP                     // - Current State -
			TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)	// TV_IDLE
			TV_TRANSITION_MAP_ENTRY (TV_MONIT)		// TV_INIT
			TV_TRANSITION_MAP_ENTRY (TV_MONIT)		// TV_TUNE
			TV_TRANSITION_MAP_ENTRY (TV_MONIT)		// TV_SCAN
			TV_TRANSITION_MAP_ENTRY (TV_MONIT)		// TV_PLAY
			TV_TRANSITION_MAP_ENTRY (TV_MONIT)		// TV_STOP
			TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN) // TV_DEINIT
			TV_TRANSITION_MAP_ENTRY (TV_MONIT)      // TV_MONIT
			TV_TRANSITION_MAP_ENTRY (TV_MONIT)      // TV_SCAN_COMPLETE
			TV_TRANSITION_MAP_ENTRY (TV_MONIT)      // TV_SDT_INFO
			TV_TRANSITION_MAP_ENTRY (TV_MONIT)	    // TV_EPG
			TV_TRANSITION_MAP_ENTRY (TV_MONIT)      // TV_EPG_INFO
			TV_TRANSITION_MAP_ENTRY (TV_MONIT)      // TV_SUBTITLE
			TV_TRANSITION_MAP_ENTRY (TV_MONIT)      // TV_RECORD
			TV_TRANSITION_MAP_ENTRY (TV_MONIT)       // TV_SCAN_CURRENT_FREQ	
			TV_TRANSITION_MAP_ENTRY (TV_MONIT)       // TV_STOP_TUNE
		END_TV_TRANSITION_MAP((void *)pMonitParam)
	}
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: New State ==> %s \n",getStateString(tvCurrentState)));
}

void __mobiletv_stop_service(void)
{
	BEGIN_TV_TRANSITION_MAP                     // - Current State -
        TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)	// TV_IDLE
        TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)	// TV_INIT
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	// TV_TUNE
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	// TV_SCAN
		TV_TRANSITION_MAP_ENTRY (TV_STOP)		// TV_PLAY
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	// TV_STOP
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN) // TV_DEINIT
        TV_TRANSITION_MAP_ENTRY (TV_STOP)       // TV_MONIT
		TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN) //TV_SCAN_COMPLETE
		TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	//TV_SDT_INFO
		TV_TRANSITION_MAP_ENTRY (TV_STOP)	    //TV_EPG
		TV_TRANSITION_MAP_ENTRY (TV_STOP)		//TV_EPG_INFO
		TV_TRANSITION_MAP_ENTRY (TV_STOP)       //TV_SUBTITLE
		TV_TRANSITION_MAP_ENTRY (TV_STOP)       //TV_RECORD
		TV_TRANSITION_MAP_ENTRY (TV_STOP)       // TV_SCAN_CURRENT_FREQ	
		TV_TRANSITION_MAP_ENTRY (TV_STOP)       // TV_STOP_TUNE
    END_TV_TRANSITION_MAP((void *)&ServId)
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: New State ==> %s \n",getStateString(tvCurrentState)));
}



static uint32_t SubtitlePid;
void __mobiletv_subtitle(uint32_t SubPid)
{
	SubtitlePid = SubPid;
	{
		BEGIN_TV_TRANSITION_MAP                    // - Current State -
			TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)	   // TV_IDLE
			TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)	   // TV_INIT
			TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	   // TV_TUNE
			TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)    // TV_SCAN
			TV_TRANSITION_MAP_ENTRY (TV_SUBTITLE)	   // TV_PLAY
			TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	   // TV_STOP
			TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)    // TV_DEINIT
			TV_TRANSITION_MAP_ENTRY (TV_SUBTITLE)      // TV_MONIT
			TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)    // TV_SCAN_COMPLETE
			TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	   // TV_SDT_INFO
			TV_TRANSITION_MAP_ENTRY (TV_SUBTITLE)	   // TV_EPG
			TV_TRANSITION_MAP_ENTRY (TV_SUBTITLE)	   // TV_EPG_INFO
			TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)    // TV_SUBTITLE
			TV_TRANSITION_MAP_ENTRY (TV_SUBTITLE)	   // TV_RECORD
			TV_TRANSITION_MAP_ENTRY (TV_SUBTITLE)       // TV_SCAN_CURRENT_FREQ	
			TV_TRANSITION_MAP_ENTRY (TV_SUBTITLE)       // TV_STOP_TUNE
		END_TV_TRANSITION_MAP((void *)&SubtitlePid)
	}
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: New State ==> %s [subPid: %d]\n",getStateString(tvCurrentState),SubtitlePid));
}

void __mobiletv_record(void)
{
	BEGIN_TV_TRANSITION_MAP                    // - Current State -
        TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)	   // TV_IDLE
        TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)	   // TV_INIT
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	   // TV_TUNE
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)    // TV_SCAN
		TV_TRANSITION_MAP_ENTRY (TV_RECORD)	       // TV_PLAY
        TV_TRANSITION_MAP_ENTRY (TV_RECORD)	       // TV_STOP
        TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)    // TV_DEINIT
        TV_TRANSITION_MAP_ENTRY (TV_RECORD)        // TV_MONIT
		TV_TRANSITION_MAP_ENTRY (TV_RECORD)        // TV_SCAN_COMPLETE
		TV_TRANSITION_MAP_ENTRY (CANNOT_HAPPEN)	   // TV_SDT_INFO
		TV_TRANSITION_MAP_ENTRY (TV_RECORD)	       // TV_EPG
		TV_TRANSITION_MAP_ENTRY (TV_RECORD)	       // TV_EPG_INFO
		TV_TRANSITION_MAP_ENTRY (TV_RECORD)        // TV_SUBTITLE
		TV_TRANSITION_MAP_ENTRY (EVENT_IGNORED)    // TV_RECORD
		TV_TRANSITION_MAP_ENTRY (TV_RECORD)       // TV_SCAN_CURRENT_FREQ	
		TV_TRANSITION_MAP_ENTRY (TV_RECORD)       // TV_STOP_TUNE
    END_TV_TRANSITION_MAP(NULL)
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: New State ==> %s \n",getStateString(tvCurrentState)));
}
