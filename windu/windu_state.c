
/* ----------------------------------------- Header File Inclusion */
#include "osal_types.h"
#include "osal_stdio.h"
#include "osal_process.h"
#include "osal_list.h"
#include "osal_memory.h"
#include "osal_string.h"

#include "demux_tssplit.h"
#include "scm_event.h"
#include "scm_info.h"
#include "scm_exp.h"
#include "scm_timer.h"
#include "scm_tuner.h"
#include "scm_config.h"
#include "sub_decoder.h"

//#include "conio.h"

#define _cprintf
#define FOR_TEST 1

extern SCM_INFO_T *pScmInfo;

/* ----------------------------------------- Exported Global Variables */

/* ----------------------------------------- Static Global Variables */

/* ----------------------------------------- Functions */


/**
 *  \fn TV_init
 *
 *  \brief
 *
 *  \par Description:
 *  This function implements 
 *
 *  \param 
 *
 *  \return void : 
 */
uint16_t TV_init()
{
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> In Tv_init() \n"));
	/*SCM_INFO structure initailization*/
	pScmInfo = (SCM_INFO_T *)Evmalloc(sizeof(SCM_INFO_T));
	Evmemset(pScmInfo,0,sizeof(SCM_INFO_T));
	if (EmbvidInit(pfnSipsiCallback,pScmInfo) == SCM_FAILURE)
	{
		SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> Failed\n\n"));
		TvExternalEvent(TV_IDLE,NULL);
		return SCM_FAILURE;
	}
	return SCM_SUCCESS;
}

/**
 *  \fn TV_deinit
 *
 *  \brief To start the task of monitoring signal parameters.
 *
 *  \par Description:
 *  This function implements 
 *
 *  \param 
 *
 *  \return void : 
 */
void TV_deinit()
{
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> In Tv_deinit()\n"));
	if (EmbvidDeInit() != SCM_FAILURE)
		TvExternalEvent(TV_IDLE,NULL);
	
}
/**
 *  \fn TV_tune
 *
 *  \brief To start the task of monitoring signal parameters.
 *
 *  \par Description:
 *  This function implements 
 *
 *  \param 
 *
 *  \return void : 
 */

EvTaskHandle TuneThreadHandle;
//static uint8_t stop_tune = 0;




unsigned __stdcall TV_Tune_multiple(void *Param)
{
	TUNE_PARAM_T * tuneParam = (TUNE_PARAM_T *)Param;
	uint32_t FreqKhz;
	uint16_t Bandwidth = 0;	
	DWORD timeout;
	STREAM_DESC_T *p_tempStremDesc = NULL;
	int i,k;
	uint16_t ret;
	pScmInfo->CurrentTuned = -1;
	pScmInfo->mStopScan = 0;
	
	
	for(i=0; i < tuneParam->FreqCount; i++){
		if(!pScmInfo->mStopScan){
			SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> List %d Freq %dHz Bandwidth %d\n",i,
				tuneParam->FerqRange.Freq[i],
				tuneParam->FerqRange.BW[i]));
			Demux_PSIInit();
			FreqKhz = tuneParam->FerqRange.Freq[i];
			Bandwidth = tuneParam->FerqRange.BW[i];
			*pScmInfo->CurrentScanFreq = FreqKhz;
			
				SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> (%d) Freq %dHz Bandwidth %d\n",pScmInfo->CurrentTuned,FreqKhz,Bandwidth));
				pScmInfo->CurrentTuned++;
				pScmInfo->TotalTuned = pScmInfo->CurrentTuned;
				SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> TT (%d) CT (%d)\n",pScmInfo->TotalTuned,pScmInfo->CurrentTuned));
				 p_tempStremDesc = pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned];
				
				if(p_tempStremDesc != NULL)
				{
					for(k =0;k< p_tempStremDesc->ulNbServices ; k++)
					{
						SERVICE_DESC_T *p_tempServiceDesc = p_tempStremDesc->pServiceDesc[k];
						if(p_tempServiceDesc->Epglist != NULL)
						{
							ScmListDestroy(p_tempServiceDesc->Epglist);
							Evfree(p_tempServiceDesc->Epglist);
							p_tempServiceDesc->Epglist = NULL;

						}
						if(p_tempServiceDesc->EpglistScheduled != NULL)
						{
							ScmListDestroy(p_tempServiceDesc->EpglistScheduled);
							Evfree(p_tempServiceDesc->EpglistScheduled);
							p_tempServiceDesc->EpglistScheduled = NULL;
						}
						if(p_tempStremDesc->pServiceDesc[k]->ServiceName != NULL)
						{
							Evfree(p_tempStremDesc->pServiceDesc[k]->ServiceName);
							p_tempStremDesc->pServiceDesc[k]->ServiceName = NULL;
						}
						Evfree(p_tempServiceDesc);
					}
					Evfree(p_tempStremDesc);
					pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned] = NULL;

					//Evfree(pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned]->pServiceDesc);
					//Evfree(pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned]);
					//pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned] = NULL;
				}
			
			TvExternalEvent(TV_SCAN_CURRENT_FREQ,(void *)pScmInfo->CurrentScanFreq);
			pScmInfo->mpServiceRequest = FALSE;
			pScmInfo->mScanDone = FALSE; 
			pScmInfo->mbSDT = FALSE;
			pScmInfo->waitmillsec = 10;
			//TvExternalEvent(TV_SCAN,NULL);  //To ADD PAT and SDT PIDS
			
			ret = EmbvidTune(FreqKhz,Bandwidth);
			if(ret == 0)
			{
				if(!pScmInfo->mStopScan){
				WaitForSingleObject(pScmInfo->hSIPSIDataAvailable,INFINITE);
				}
			}else if(ret == 10)
			{
				/*failed tune at this freq*/
				pScmInfo->CurrentTuned--;
				pScmInfo->TotalTuned = pScmInfo->CurrentTuned;
			}else if(ret == 13)
			{
				/*Scan completed*/
				_cprintf("signaled\n");
			}else if(ret == 14)
			{
				_cprintf("stop tune\n");
				pScmInfo->CurrentTuned--;
				pScmInfo->TotalTuned = pScmInfo->CurrentTuned;
				return 0;/*user canceld*/
			}
		}
	}
	return 0;
}



//static uint8_t epg_init = 0;
void TV_tune(union SCM_DATA_T scmData)
{
	TUNE_PARAM_T * tuneParam = (TUNE_PARAM_T *)scmData.pData;   
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> In Tv_tune() \n"));
	pScmInfo->mStopScan = 0;
	pScmInfo->CurrentTuned = -1;
	epg_InitEIT(0);
	EmbvidStop(pScmInfo);
	EvCreateTask(TV_Tune_multiple, (void *)scmData.pData, &TuneThreadHandle);
}

/**
 *  \fn TV_stop_tune
 *
 *  \brief To start recording of current playing service
 *
 *  \par Description:
 *  This function implements 
 *
 *  \param 
 *
 *  \return void : 
 */
void TV_stop_tune(void)
{
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> In TV_stop_tune() \n"));
	//stop_tune = 1;
	pScmInfo->mStopScan = 1;
	pScmInfo->waitmillsec = 5000;
	SetEvent(pScmInfo->hSIPSIDataAvailable);
}

/**
 *  \fn TV_stop_scan
 *
 *  \brief To start the task of monitoring signal parameters.
 *
 *  \par Description:
 *  This function implements 
 *
 *  \param 
 *
 *  \return void : 
 */
void TV_stop_scan()
{
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> In TV_stop_scan() \n"));
	EmbvidStop(pScmInfo);
}
/**
 *  \fn TV_scan
 *
 *  \brief 
 *
 *  \par Description:
 *  This function implements 
 *
 *  \param 
 *
 *  \return void : 
 */
void TV_scan()
{
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> In Tv_scan() \n"));
	//EvMutexLock(&(pScmInfo->MutextForDib));
	EmbvidGetServiceInfo();
	//EvMutexUnlock(&(pScmInfo->MutextForDib));
}

/**
 *  \fn TV_play
 *
 *  \brief 
 *
 *  \par Description:
 *  This function implements 
 *
 *  \param 
 *
 *  \return void : 
 */

#define _ATSC
static int CurrentPlayIdx = -1; 
void TV_play(union SCM_DATA_T scmData)
{
#ifdef _ATSC
	PLAY_PARAM_T *playParam = (PLAY_PARAM_T *)scmData.pData; 
	//_cprintf("section Idx %d, service Id %d\n",playParam->SectionIdx,playParam->ServiceId);
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> In Tv_play()  (%d)\n",playParam->ServiceId));
	if(CurrentPlayIdx != playParam->SectionIdx)
	{
		epg_InitEIT(0);
		CurrentPlayIdx = playParam->SectionIdx;
	}
	EmbvidStartService(playParam->ServiceId,playParam->SectionIdx,pScmInfo);
#else
	uint32_t *serviceID = (uint32_t *)scmData.pData;
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> In Tv_play()  (%d)\n",*serviceID));
	
	EmbvidStartService(*serviceID,pScmInfo);
#endif
}


/**
 *  \fn TV_stop
 *
 *  \brief 
 *
 *  \par Description:
 *  This function implements 
 *
 *  \param 
 *
 *  \return void : 
 */
void TV_stop()
{
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> In TV_stop() \n"));
	EmbvidStop(pScmInfo);
}

/**
 *  \fn TV_stop
 *
 *  \brief To start 
 *
 *  \par Description:
 *  This function implements 
 *
 *  \param 
 *
 *  \return void : 
 */
void TV_idle()
{
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> In Tv_idle() \n"));
}



int MonitTimerCallback(double Strength[])
{
	//SCM_DEBUG(SCM_TIMER,("[SCM_TIMER]: MonitTimer\n"));
	//SCM_DEBUG(SCM_TIMER, ("[SCM_TIMER]: Signle Strength RF:%f BER:%f C//N: %f Signal :%f \n", Strength[0], Strength[1], Strength[2], Strength[3]));
	return 0;
}

/**
 *  \fn TV_start_monit
 *
 *  \brief To start the task of monitoring signal parameters.
 *
 *  \par Description:
 *  This function implements 
 *
 *  \param 
 *
 *  \return void : 
 */
static unsigned int m_MonitTimer = 0; 
unsigned int MonitInter  = 0 ;
SIGNAL_INFO_T *sigInfo;
void TV_start_monit(union SCM_DATA_T scmData)
{
	MONIT_PARAM_T * monitParam = (MONIT_PARAM_T *)scmData.pData;

	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> In TV_start_monit() \n"));
	if(m_MonitTimer == 0)
	{
		sigInfo = (SIGNAL_INFO_T *)Evmalloc(sizeof(SIGNAL_INFO_T) * (monitParam->NoOfIteration));
		MonitInter = monitParam->NoOfIteration;
		m_MonitTimer = ScmSetTimer(monitParam->updateTime, monitParam->pfnMonitFunc);
	}
}


/**
 *  \fn TV_start_epg
 *
 *  \brief To start the task of monitoring signal parameters.
 *
 *  \par Description:
 *  This function implements 
 *
 *  \param 
 *
 *  \return void : 
 */
void TV_start_epg()
{
	uint32_t pid[1];
	pid[0] = 0x12; //EIT
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> In TV_start_epg() \n"));
	//EmbvidAddTsPid((uint32_t *)pid, 1);  //for TS Path
	
}





///can be deleted later///
void TV_update_epg(union SCM_DATA_T scmData)
{
	//EIT_T *EitParam = (MONIT_PARAM_T *)scmData.pData;
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> In TV_update_epg() \n"));
}

/**
 *  \fn TV_subtitle
 *
 *  \brief To start subtitle display of current playing service
 *
 *  \par Description:
 *  This function implements 
 *
 *  \param 
 *
 *  \return void : 
 */
void TV_subtitle(union SCM_DATA_T scmData)
{
	uint32_t *SubPid = (uint32_t *)scmData.pData;
	uint32_t sub_pid = *SubPid;
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> In TV_subtitle() [%d]\n",*SubPid));
	//StartSubDecoder(*SubPid,1,1);
	Demux_ActivateSub(*SubPid, 1, 1/*page_id, ancillary_page_id*/); 
	EmbvidAddTsPid((ULONG *)SubPid, 1, 1);  //for TS Path
}


/**
 *  \fn TV_record
 *
 *  \brief To start recording of current playing service
 *
 *  \par Description:
 *  This function implements 
 *
 *  \param 
 *
 *  \return void : 
 */
void TV_record(void)
{
	SCM_DEBUG(SCM_STATE, ("[SCM_STATE]: ==> In TV_subtitle() \n"));
}
