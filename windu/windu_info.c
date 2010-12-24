
#include "osal_types.h"
#include "osal_list.h"
#include "osal_string.h"
#include "osal_memory.h"
#include "osal_stdlib.h"
#include "scm_event.h"
#include "demux_tssplit.h"
#include "scm_info.h"
#include "scm_exp.h"
#include "scm_config.h"
#include "Scm_tuner.h"
//#include "conio.h"

//#define _cprintf

extern SCM_INFO_T *pScmInfo;
extern int32_t scmFreq;
extern int32_t scmBw;

size_t SizeofEpg(const void *el) {
	return sizeof(EPG_T);
}

size_t SizeofStreamDesc(const void *el) {
	return sizeof(STREAM_DESC_T);
}
size_t SizeofServiceDesc(const void *el) {
	return sizeof(SERVICE_DESC_T);
}

int EpgComparatorFunc(const void *a, const void *b) {
	/* compare areas */
	const EPG_T *A = (EPG_T *)a;
	const EPG_T *B = (EPG_T *)b;
	if(A->StartDate == B->StartDate)
	{
		return ((A->StartTime < B->StartTime) - (A->StartTime > B->StartTime));	
	}else
	{
		return ((A->StartDate < B->StartDate) - (A->StartDate > B->StartDate));	
	}
	

	//return ((A->Id < B->Id) - (A->Id > B->Id));	
}


int32_t print_MjdToYmd (long_t MJD/*, /*int8_t *YMD*/)
{
   
   long_t   Year,Month,Date,Var;
   /* algo: ETSI EN 300 468 - ANNEX C*/
   Year =  (long_t) ((MJD  - 15078.2) / 365.25);
   Month =  (long_t) ((MJD - 14956.1 - (long_t)(Year * 365.25) ) / 30.6001);
   Date =  (long_t) (MJD - 14956 - (long_t)(Year * 365.25) - (long_t)(Month * 30.6001));
   Var =  (Month == 14 || Month == 15) ? 1 : 0;
   Year = Year + Var + 1900;
   Month = Month - 1 - Var*12;
   SCM_DEBUG(SCM_SDK,(" %04d-%02d-%02d \n",(int32_t)Year,(int32_t)Month,(int32_t)Date));
   //Evsprintf(YMD, "%04d-%02d-%02d",(int32_t)Year,(int32_t)Month,(int32_t)Date);
   return 0;
}

void print_UtcToHhmm(unsigned int StartTime,unsigned int Duration)
{
	int32_t StartTime_HH,StartTime_MM;
	int32_t Duration_HH, Duration_MM;
	int8_t TmpBuf[256];
	int32_t StartTimeInMinutes,DurationInMin;

	Evsprintf((EvString_t)TmpBuf, "%02lx", (StartTime>>16) &0xFF);
	StartTime_HH = Evatoi((EvString_t)TmpBuf);
	Evsprintf((EvString_t)TmpBuf, "%02lx", (StartTime>>8) &0xFF);
	StartTime_MM = Evatoi((EvString_t)TmpBuf);
	StartTimeInMinutes = ((StartTime_HH*60)+StartTime_MM) ;
	

	SCM_DEBUG(SCM_SDK,("(%d:%d",StartTime_HH,StartTime_MM));

	Evsprintf((EvString_t)TmpBuf, "%02lx", (Duration>>16) &0xFF);
	Duration_HH = Evatoi((EvString_t)TmpBuf);
	Evsprintf((EvString_t)TmpBuf, "%02lx", (Duration>>8) &0xFF);
	Duration_MM = Evatoi((EvString_t)TmpBuf);
	DurationInMin = ((Duration_HH*60)+Duration_MM) ;
	while(Duration_MM+StartTime_MM > 60)
	{
		Duration_HH++;
		Duration_MM = Duration_MM - 60;
	}
	SCM_DEBUG(SCM_SDK,("- %d:%d)",(Duration_HH+StartTime_HH),(Duration_MM+StartTime_MM)));
}


int findService(int16_t serviceId)
{
	int i = -1;
	int nbservice = pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned]->ulNbServices;
	if(nbservice > 0)
	{
		while(nbservice--)
		{
			if(serviceId == pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned]->pServiceDesc[++i]->ServiceId)
			{
				return i;
			}
		}
		return -1;
	}else
	{
		return -1;
	}
	
	
}

int Checkforcompleteinfo()
{
	uint8_t nbservice;
	uint8_t i = -1;
	uint8_t totalinfo = 0;
	
	STREAM_DESC_T *pStreamDesc = pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned];
	if(pStreamDesc == NULL)
	{
		return -1;
	}
	nbservice = pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned]->ulNbServices;
	while(nbservice--)
	{
		if(pStreamDesc->pServiceDesc[++i]->PmtValid)
		{
			totalinfo++;
		}
	}
	if(totalinfo == pStreamDesc->ulNbServices)
		return 0;
	else
		return -1;
	
}

int CheckEventId(SCM_LIST_T *Epglist,unsigned short eventId)
{
	int ret = 0;
	if(Epglist)
	{
		ScmListStart(Epglist);	
		while (ScmListCheckNext(Epglist)) {	
		EPG_T *epg = (EPG_T *)ScmListNext(Epglist);
		if(epg->Id == eventId)
		{
			ret = 1;
			break;
		}
		}
		ScmListEnd(Epglist);
	}
	return ret;
}
#define YEAR0                   1900
#define EPOCH_YR                1970
#define SECS_DAY                (24L * 60L * 60L)
#define LEAPYEAR(year)          (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)          (LEAPYEAR(year) ? 366 : 365)

const int _ytab[2][12] = 
{
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};
void find_mjd(long Month,long Date, long Year, unsigned int *mjd)
{
   long Var = 1; //and Var = 1 
   long temp_Month = Month + 1 + Var*12;
   long temp_Year = Year - 1900 - Var;
   if(Month == 14 || Month == 15)
   {   
	   Month = temp_Month;
	   Year = temp_Year;
   }else
   {
	   Var = 0;	
	   Month = Month + 1 + Var*12;
	   Year = Year - 1900 - Var;
   }

   *mjd = (long)(Date + (long)(Month * 30.6001) + (long)(Year * 365.25) + 14956);

   //printf("mjd %d\n",*mjd);
}


int getTimeDate(uint32_t *timer,unsigned int *StartTime,unsigned int *StartDate)
{
  time_t time = *timer;
  unsigned long dayclock, dayno;
  int year = EPOCH_YR;

  dayclock = (unsigned long) time % SECS_DAY;
  dayno = (unsigned long) time / SECS_DAY;


   unsigned int tm_sec = dayclock % 60;
   unsigned int tm_min = (dayclock % 3600) / 60;
   unsigned int tm_hour = dayclock / 3600;


  *StartTime = (((tm_hour<<16) & 0x00ff0000) | ((tm_min<<8) & 0x0000ff00) | (tm_sec & 0x000000ff));


  uint8_t tm_wday = (dayno + 4) % 7; // Day 0 was a thursday
  while (dayno >= (unsigned long) YEARSIZE(year)) 
  {
    dayno -= YEARSIZE(year);
    year++;
  }
  uint8_t tm_year = year - YEAR0;
  uint8_t tm_yday = dayno;
  uint8_t tm_mon = 0;
  while (dayno >= (unsigned long) _ytab[LEAPYEAR(year)][tm_mon]) 
  {
    dayno -= _ytab[LEAPYEAR(year)][tm_mon];
    tm_mon++;
  }
  uint8_t tm_mday = dayno + 1;
  uint8_t tm_isdst = 0;

  find_mjd(tm_mon+1,tm_mday,year,StartDate);

  return 0;
}


SDT_INFO_T *pSdtInfo = NULL;
uint32_t timer[1];
uint32_t i_StartTime;
uint32_t i_StartDate;
uint32_t starttime;
uint32_t startdate;
void ScmInfoReceiveCallback(dword_t dw , void * pData)
{
	int32_t nbService;
	STREAM_DESC_T *pStreamDesc = NULL;
	SERVICE_DESC_T *pServiceDesc = NULL;
	switch(dw)
	{
		case 1://PAT
		{
			PAT_INFO_T *pPatInfo = (PAT_INFO_T *)pData;
			int i=0;
			int nbService = pPatInfo->numbers;
			//_cprintf("nbService %d %d\n",nbService,pScmInfo->CurrentTuned);
			SCM_DEBUG(SCM_SDK,("[SCM_SDK]: PAT Info ===>\n"));
			if (pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned] == NULL)
			{
				//_cprintf("malloc\n");
				pStreamDesc = (STREAM_DESC_T *)Evmalloc(sizeof(STREAM_DESC_T));
				Evmemset(pStreamDesc,0,sizeof(STREAM_DESC_T));
				pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned] = pStreamDesc;
			}else
			{
				pStreamDesc = pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned];
				Evmemset(pStreamDesc,0,sizeof(STREAM_DESC_T));
			}
			
			pStreamDesc->Freq = GetTunedFreq();
			pStreamDesc->Bw = GetTunedBw();
			pStreamDesc->ulNbServices = nbService;
			SCM_DEBUG(SCM_SDK,("[SCM_SDK]: PAT (%d)\n",nbService));
			for(i=0; i<nbService; i++)
			{
				if(pStreamDesc->pServiceDesc[i] == NULL)
				{
					pServiceDesc  =  (SERVICE_DESC_T *)Evmalloc(sizeof(SERVICE_DESC_T));
				}
				Evmemset(pServiceDesc,0,sizeof(SERVICE_DESC_T));
				pServiceDesc->ServiceId = pPatInfo->programs[i];
				pServiceDesc->PidPMT = pPatInfo->pids[i];
				pStreamDesc->pServiceDesc[i] = pServiceDesc;
				SCM_DEBUG(SCM_SDK,("[SCM_SDK]: PAT (%d) (%d)\n",pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned]->pServiceDesc[i]->ServiceId,
					pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned]->pServiceDesc[i]->PidPMT));
			}
			StopCallback();
			TvExternalEvent(TV_SCAN,NULL);  //To ADD PMT PIDS
		}
			break;
		case 2:  //PMT
		{
			PMT_INFO_T *pPmtInfo = (PMT_INFO_T*)pData;
			int k = 0,l;
			int index = 0;
			pStreamDesc = pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned];
			nbService = pStreamDesc->ulNbServices;
			SCM_DEBUG(SCM_SDK,("[SCM_SDK]: PMT Info ===> \n"));
			index = findService(pPmtInfo->programs);
			if(index != -1)
			{
				pStreamDesc->pServiceDesc[index]->PidPCR = pPmtInfo->pcrPid;
				pStreamDesc->pServiceDesc[index]->PidVideo = pPmtInfo->videoPid;
				for(l =0; l<pPmtInfo->audiosNum;l++){
					pStreamDesc->pServiceDesc[index]->PidAudio[l] = pPmtInfo->audioPids[l];
				}
				
				pStreamDesc->pServiceDesc[index]->AudioPidNum = pPmtInfo->audiosNum;
				pStreamDesc->pServiceDesc[index]->PidSubtitle = pPmtInfo->subtitlePids[0];
				pStreamDesc->pServiceDesc[index]->PmtValid = 1; 
				
				if(pPmtInfo->audiosNum > 0 && pPmtInfo->videoPid != 0)		
				{
					pStreamDesc->pServiceDesc[index]->AvValid = 1; 
				}
				pStreamDesc->pServiceDesc[index]->Epglist = NULL;
				pStreamDesc->pServiceDesc[index]->EpglistScheduled = NULL;

			}
			if(!Checkforcompleteinfo() && (pScmInfo->mbSDT))
			{
				StopCallback();
				pScmInfo->mScanDone = TRUE;  //[TO DO]  
				/*Implement a mechanism to stop the tsdemux sending the sipsi data.*/
				//UnMapPids();  /*For BDA*/
				TvExternalEvent(TV_SCAN_COMPLETE,(void *)pStreamDesc);
			}

		}
			break;
		case 3://SDT
		{
			
			SDT_INFO_T *pSdtInfo = (SDT_INFO_T *)pData;
			int i = 0;
			int index = 0;
			//pScmInfo->mbTuneSuccess = TRUE;
			pStreamDesc = pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned];
			if(pStreamDesc != NULL)
			{
				int nbService = pStreamDesc->ulNbServices;
				pSdtInfo->numbers = nbService;
				SCM_DEBUG(SCM_SDK,("[SCM_SDK]: SDT Info ===>\n"));
				for(i=0;i<nbService; i++)
				{
					//SCM_DEBUG(SCM_SDK,("%s\n",pSdtInfo->ServiceName[i]));
					index = findService(pSdtInfo->programs[i]);
					if(index != -1)
					{
						EvString_t pServiceName;
						pServiceName = (EvString_t)Evmalloc(sizeof(EvString_t)*(strlen(pSdtInfo->ServiceName[i])+1));
						Evmemcpy(pServiceName,pSdtInfo->ServiceName[i],strlen(pSdtInfo->ServiceName[i])+1);
						pStreamDesc->pServiceDesc[index]->ServiceName = pServiceName;
						pSdtInfo->ServicedescIdx[i] = index;
					}
				}
			}
			pScmInfo->mbSDT = TRUE;
			TvExternalEvent(TV_SDT_INFO,(void *)pData/*(void *)pProgramAssociation*/);
			if(!Checkforcompleteinfo())
			{
				StopCallback();
				pScmInfo->mScanDone = TRUE;  //[TO DO]  
				/*Implement a mechanism to stop the tsdemux sending the sipsi data.
				*/
				//UnMapPids();  /*For BDA*/
				TvExternalEvent(TV_SCAN_COMPLETE,(void *)pStreamDesc);
			}

		}
			break;
		case 4://NIT
		{
			SCM_DEBUG(SCM_SDK,("NIT Info ===>\n\n"));
		}
			break;
		case 6://EIT
		{
			EIT_T *pEventInformation=(EIT_T *)pData;
			EVENT_T *pEvent = (EVENT_T *)pEventInformation->pEvent;
			SCM_LIST_T *Epglist;
			static int Serviceid = 0,index = 0;
			int pos = 0;
			EPG_T newEpg;
			pStreamDesc = pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned];
			Serviceid = pEventInformation->ServiceId;
		    nbService = pStreamDesc->ulNbServices;
			index = findService(Serviceid);
			
			
			if(index != -1){

				if(pEventInformation->ScheduleEpg == TRUE)
				{
					if(pStreamDesc->pServiceDesc[index]->EpglistScheduled == NULL)
					{
						Epglist = (SCM_LIST_T *)Evmalloc(sizeof(SCM_LIST_T));
						pStreamDesc->pServiceDesc[index]->EpglistScheduled = Epglist;
						ScmListInit(Epglist);    /* initialize the list */
						ScmListAttrCopy(Epglist, SizeofEpg, 1);
						ScmListAttrCompare(Epglist,EpgComparatorFunc);
					}else{
						Epglist = pStreamDesc->pServiceDesc[index]->EpglistScheduled;
						ScmListClear(Epglist);
					}
				}
				else{
					if(pStreamDesc->pServiceDesc[index]->Epglist == NULL)
					{
						Epglist = (SCM_LIST_T *)Evmalloc(sizeof(SCM_LIST_T));
						pStreamDesc->pServiceDesc[index]->Epglist = Epglist;
						ScmListInit(Epglist);    /* initialize the list */
						ScmListAttrCopy(Epglist, SizeofEpg, 1);
						ScmListAttrCompare(Epglist,EpgComparatorFunc);
					}else{
						Epglist = pStreamDesc->pServiceDesc[index]->Epglist;
						ScmListClear(Epglist);
					}
					
				}
			}else
			{
				return;
			}

			while( pEvent )
			{

				newEpg.Id = pEvent->event_id;
				newEpg.StartTime = pEvent->StartTime;
				newEpg.Duration = pEvent->Duration;
				newEpg.StartDate = pEvent->StartDate;
				newEpg.mbScrambled = pEvent->free_CA_mode;

				/*Short event*/
				if( pEvent->pShortEventDescriptor )
				{
					if( 0 < pEvent->pShortEventDescriptor->event_name_length )
					{
						int l=1+scm_MultiByteToWideChar(pEvent->pShortEventDescriptor->event_name_char, pEvent->pShortEventDescriptor->event_name_length, NULL, 0);
						newEpg.pszEventName=(TCHAR *)Evmalloc(sizeof(TCHAR)*l);
						l=scm_MultiByteToWideChar(pEvent->pShortEventDescriptor->event_name_char, pEvent->pShortEventDescriptor->event_name_length, newEpg.pszEventName, l);
						newEpg.pszEventName[l] = '\0';
					}
			
					if( 0< pEvent->pShortEventDescriptor->text_length )
					{
						int l=1+scm_MultiByteToWideChar(pEvent->pShortEventDescriptor->text_char, pEvent->pShortEventDescriptor->text_length, NULL, 0);
						newEpg.pszText=(TCHAR *)Evmalloc(sizeof(TCHAR)*l);
						l=scm_MultiByteToWideChar(pEvent->pShortEventDescriptor->text_char, pEvent->pShortEventDescriptor->text_length, newEpg.pszText, l);
						newEpg.pszText[l]= '\0';
					}else
					{
						newEpg.pszText = NULL;
					}

				}
#if 0
				/*Extended event*/
				if( pEvent->pExtendedEventDescriptor )
				{
					int TotalLength=1,l = 0;
					EXTENDEDEVENTDESCRIPTOR_T *pExtendedEventDescriptor=pEvent->pExtendedEventDescriptor;
					
					while( pExtendedEventDescriptor  )
					{
						if( 0 < pExtendedEventDescriptor->text_length )
						{
							TotalLength+=scm_MultiByteToWideChar(pExtendedEventDescriptor->text_char, pExtendedEventDescriptor->text_length, NULL, 0);
						}
						pExtendedEventDescriptor=pExtendedEventDescriptor->pNext;
					}
					newEpg.pszExtendedText = (TCHAR *)malloc(sizeof(TCHAR)* TotalLength);
					
					pExtendedEventDescriptor=pEvent->pExtendedEventDescriptor;
					while( pExtendedEventDescriptor )
					{
						if( 0 < pExtendedEventDescriptor->text_length )
						{
							l+=scm_MultiByteToWideChar(pExtendedEventDescriptor->text_char, pExtendedEventDescriptor->text_length, &newEpg.pszExtendedText[l]/*&Event.mpEvent->pszExtendedText[l]*/, TotalLength-l);
						}
						pExtendedEventDescriptor=pExtendedEventDescriptor->pNext;
					}
					newEpg.pszExtendedText[l] = '\0';
				}
				
				newEpg.Rating = 0;
				if( pEvent->pParentalRatingDescriptor )
				{
					newEpg.Rating = pEvent->pParentalRatingDescriptor->rating[0];
				}
#endif
				
				ScmListAppend(Epglist, &newEpg);
				ScmListSort(Epglist,1);
				pEvent=pEvent->pNext;
			}
#if 0
			//ScmListSort(Epglist,1);
			if(Serviceid == 1026)
			{
				ScmListStart(Epglist);							   /* starting an iteration "session" */
					while (ScmListCheckNext(Epglist)) 
					{				   /* tell whether more values available */
						EPG_T *epg = (EPG_T *)ScmListNext(Epglist);
						SCM_DEBUG(SCM_SDK,(":: %s\n"),epg->pszEventName);
						if(epg->pszText == NULL){
							SCM_DEBUG(SCM_SDK,(":: na\n"));
						}
						else
						{
							SCM_DEBUG(SCM_SDK,(":: %s\n"),pEvent->pShortEventDescriptor->text_char);
						}
						SCM_DEBUG(SCM_SDK,("::_____\n"));
				}
				ScmListEnd(Epglist); 
			}
#endif
			TvExternalEvent(TV_EPG_INFO,(void *)&Serviceid);
			}
			break;
			case 9: // MGT-ATSC
				{
				//fill
				pScmInfo->m_pATSCMgtInfo = pData;
				//void *m_pATSCPsipInfo;
				
				}
				TvExternalEvent(TV_SCAN,NULL);  //To ADD PMT PIDS
				break;
			
			case 10: //VCT-ATSC
				{
				VCT_INFO_T *pVCTInfo=(VCT_INFO_T *)pData;
				
				int32_t inbService = 0;
				int i = 0;
				//_cprintf("%d\n",pVCTInfo->TotalNo);
				if(pSdtInfo == NULL)
				{
						pSdtInfo = (SDT_INFO_T *)Evmalloc(sizeof(SDT_INFO_T));
				}
				if (pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned] == NULL)
				{
					pStreamDesc = (STREAM_DESC_T *)Evmalloc(sizeof(STREAM_DESC_T));
					Evmemset(pStreamDesc,0,sizeof(STREAM_DESC_T));
					pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned] = pStreamDesc;
					//pServiceDesc  =  (SERVICE_DESC_T *)Evmalloc(sizeof(SERVICE_DESC_T)*nbService);
					//Evmemset(pServiceDesc,0,sizeof(SERVICE_DESC_T)*nbService);
					//pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned]->pServiceDesc = pServiceDesc;
				}
				pStreamDesc = pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned];
				inbService = pStreamDesc->ulNbServices = pVCTInfo->TotalNo;

				

				//_cprintf("%d\n",pStreamDesc->ulNbServices );
				pSdtInfo->numbers = inbService;
				//_cprintf("%d\n",inbService);
				SCM_DEBUG(SCM_SDK,("[SCM_SDK]: PMT Info ===> \n"));
				
				for(int i = 0; i<inbService; i++)
				{
					EvString_t pServiceName;


					if(pStreamDesc->pServiceDesc[i] == NULL)
					{
						pServiceDesc  =  (SERVICE_DESC_T *)Evmalloc(sizeof(SERVICE_DESC_T));
					}
					Evmemset(pServiceDesc,0,sizeof(SERVICE_DESC_T));
					
					pServiceName = (EvString_t)Evmalloc(sizeof(EvString_t)*100);
					Evmemcpy(pServiceName,pVCTInfo->ServiceName[i],sizeof(EvString_t)*100);

					pServiceDesc->ServiceName = pServiceName;
					//_cprintf("%s %s\n",pServiceName,pVCTInfo->ServiceName[i]);
					pServiceDesc->ServiceId = pVCTInfo->Prog_Number[i];
					pServiceDesc->PidPCR = pVCTInfo->pVctServInfo[i].Pcr_Pid;
					pServiceDesc->PidAudio[0] = pVCTInfo->pVctServInfo[i].Audio_Pid[0];
					pServiceDesc->AudioPidNum = 1;
					pServiceDesc->PidVideo = pVCTInfo->pVctServInfo[i].Video_Pid;
					pServiceDesc->PmtValid = 1; 
					//pStreamDesc->Freq = pVCTInfo->CarrierFreq[i];

					pStreamDesc->Freq = GetTunedFreq();
					pStreamDesc->Bw = GetTunedBw();

					if(pServiceDesc->AudioPidNum > 0 && pVCTInfo->pVctServInfo[i].Video_Pid != 0)		
					{
						pServiceDesc->AvValid = 1; 
					}
					pSdtInfo->ServiceName[i] = pServiceName;
					pSdtInfo->programs[i]= pVCTInfo->Prog_Number[i];
					pSdtInfo->ServicedescIdx[i] = i;
					pStreamDesc->pServiceDesc[i] = pServiceDesc;

				}
					
					pScmInfo->mbSDT = TRUE;
					//TvExternalEvent(TV_SDT_INFO,(void *)pSdtInfo);
					TvExternalEvent(TV_SDT_INFO,(void *)pSdtInfo);
					Sleep(700);
					StopCallback();
					pScmInfo->mScanDone = TRUE;  //[TO DO]  
					/*Implement a mechanism to stop the tsdemux sending the sipsi data.*/
					//UnMapPids();  /*For BDA*/
					TvExternalEvent(TV_SCAN_COMPLETE,(void *)pStreamDesc);
			}
				break;
			case 11: //EIT-ATSC
				{
			EIT_T *pEventInformation=(EIT_T *)pData;
			EVENT_T *pEvent = (EVENT_T *)pEventInformation->pEvent;
			SCM_LIST_T *Epglist;
			SCM_LIST_T *CurrentEpglist;
			static int Serviceid = 0,index = 0;
			EPG_T newEpg;
			int ETT = -1;
			pStreamDesc = pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned];
			Serviceid = pEventInformation->ServiceId;
			//_cprintf("Serviceid %d\n",Serviceid);
		    //nbService = pStreamDesc->ulNbServices;
			index = findService(Serviceid);
			
			if(index != -1){

				if(pEventInformation->ScheduleEpg == TRUE)
				{
					if(pStreamDesc->pServiceDesc[index]->EpglistScheduled == NULL)
					{
						Epglist = (SCM_LIST_T *)Evmalloc(sizeof(SCM_LIST_T));
						pStreamDesc->pServiceDesc[index]->EpglistScheduled = Epglist;
						ScmListInit(Epglist);    /* initialize the list */
						ScmListAttrCopy(Epglist, SizeofEpg, 1);
						ScmListAttrCompare(Epglist,EpgComparatorFunc);
					}else{
						Epglist = pStreamDesc->pServiceDesc[index]->EpglistScheduled;
						//ScmListClear(Epglist);
					}
					ETT = 0;
				}
				else{
					if(pStreamDesc->pServiceDesc[index]->EpglistScheduled == NULL)
					{
						//Epglist = (SCM_LIST_T *)Evmalloc(sizeof(SCM_LIST_T));
						//pStreamDesc->pServiceDesc[index]->Epglist = Epglist;
						//pStreamDesc->pServiceDesc[index]->EpglistScheduled = Epglist;
						//ScmListInit(Epglist);    /* initialize the list */
						//ScmListAttrCopy(Epglist, SizeofEpg, 1);
						//ScmListAttrCompare(Epglist,EpgComparatorFunc);
						return;
					}else{
						Epglist = pStreamDesc->pServiceDesc[index]->EpglistScheduled;
						//ScmListClear(Epglist);
					}
					ETT = 1;
					
				}
			}else
			{
				return;
			}

			while( pEvent )
			{
				if(ETT == 0)
				{
					if(CheckEventId(Epglist,pEvent->event_id))
					{
						pEvent=pEvent->pNext;
						continue;
					}
							
					newEpg.Id = pEvent->event_id;
					newEpg.StartTime = pEvent->StartTime;
					newEpg.Duration = pEvent->Duration;
					newEpg.StartDate = pEvent->StartDate;
					newEpg.mbScrambled = pEvent->free_CA_mode;

					/*Short event*/
					if( pEvent->pShortEventDescriptor )
					{
						if( 0 < pEvent->pShortEventDescriptor->event_name_length )
						{
							int l=1+scm_MultiByteToWideChar(pEvent->pShortEventDescriptor->event_name_char, pEvent->pShortEventDescriptor->event_name_length, NULL, 0);
							newEpg.pszEventName=(TCHAR *)Evmalloc(sizeof(TCHAR)*l);
							l=scm_MultiByteToWideChar(pEvent->pShortEventDescriptor->event_name_char, pEvent->pShortEventDescriptor->event_name_length, newEpg.pszEventName, l);
							newEpg.pszEventName[l] = '\0';
						}
					if( 0< pEvent->pShortEventDescriptor->text_length )
					{
						int l=1+scm_MultiByteToWideChar(pEvent->pShortEventDescriptor->text_char, pEvent->pShortEventDescriptor->text_length, NULL, 0);
						newEpg.pszText=(TCHAR *)Evmalloc(sizeof(TCHAR)*l);
						l=scm_MultiByteToWideChar(pEvent->pShortEventDescriptor->text_char, pEvent->pShortEventDescriptor->text_length, newEpg.pszText, l);
						newEpg.pszText[l]= '\0';
					}else
					{
						newEpg.pszText = NULL;
					}
					newEpg.Rating = 0;
					if( pEvent->pParentalRatingDescriptor )
					{
						//newEpg.Rating = pEvent->pParentalRatingDescriptor->rating[0];
						newEpg.Rating = pEvent->pParentalRatingDescriptor->NumberOfItems;

						int l=1+scm_MultiByteToWideChar((char *)pEvent->pParentalRatingDescriptor->rating, strlen((const char *)pEvent->pParentalRatingDescriptor->rating), NULL, 0);
						newEpg.pszExtendedText=(TCHAR *)Evmalloc(sizeof(TCHAR)*l);
						l=scm_MultiByteToWideChar((char *)pEvent->pParentalRatingDescriptor->rating,strlen((const char *)pEvent->pParentalRatingDescriptor->rating), newEpg.pszExtendedText, l);
						newEpg.pszExtendedText[l]= '\0';
					}else
					{
						newEpg.pszExtendedText = NULL;
					}

				}
				
				ScmListAppend(Epglist, &newEpg);
				ScmListSort(Epglist,1);
				}else if(ETT == 1)
				{
						if(Epglist)
						{
							ScmListStart(Epglist);	
							while (ScmListCheckNext(Epglist)) {	
							EPG_T *epg = (EPG_T *)ScmListNext(Epglist);
							if(epg->Id == pEvent->event_id)
							{
								if( 0< pEvent->pShortEventDescriptor->text_length )
								{
									int l=1+scm_MultiByteToWideChar(pEvent->pShortEventDescriptor->text_char, pEvent->pShortEventDescriptor->text_length, NULL, 0);
									epg->pszText = (TCHAR *)Evmalloc(sizeof(TCHAR)*l);
									l=scm_MultiByteToWideChar(pEvent->pShortEventDescriptor->text_char, pEvent->pShortEventDescriptor->text_length, epg->pszText, l);
									epg->pszText[l]= '\0';
								}else
								{
									epg->pszText = NULL;
								}
							}
						}
						ScmListEnd(Epglist);
						}
				}
				pEvent=pEvent->pNext;
			}
			
			if(pStreamDesc->pServiceDesc[index]->Epglist == NULL)
			{
			//if((starttime + 500) < i_StartTime)
			//{
			int bypass = 0;
			starttime = i_StartTime;
			startdate = i_StartDate;
			int Foundcurrent = 0;
			if(Epglist)
			{
					ScmListStart(Epglist);	
					while (ScmListCheckNext(Epglist)){	
					EPG_T *epg = (EPG_T *)ScmListNext(Epglist);
					if (bypass == 0)
					{
					if(epg->StartDate == startdate)
					{
						if(Foundcurrent == 0)
						{
						if((epg->StartTime < starttime) && ((epg->Duration + epg->StartTime) > starttime) )
						{   //this is current prgram flag it
							//_cprintf("current program \n");
							//_cprintf("program start %d , duration %d\n",epg->StartTime,epg->Duration);
							//_cprintf("current time %d\n",StartTime);
							if(pStreamDesc->pServiceDesc[index]->Epglist == NULL)
							{
								CurrentEpglist = (SCM_LIST_T *)Evmalloc(sizeof(SCM_LIST_T));
								pStreamDesc->pServiceDesc[index]->Epglist = CurrentEpglist;
								ScmListInit(CurrentEpglist);    /* initialize the list */
								ScmListAttrCopy(CurrentEpglist, SizeofEpg, 1);
								ScmListAttrCompare(CurrentEpglist,EpgComparatorFunc);
								//starttime = 0;
							}else{
								CurrentEpglist = pStreamDesc->pServiceDesc[index]->Epglist;
								ScmListClear(Epglist);
							}
								ScmListAppend(CurrentEpglist, epg);
								ScmListSort(CurrentEpglist,1);
								Foundcurrent = 1;
						}
						}
						else
							{
								//if(Foundcurrent)
								//{
									ScmListAppend(CurrentEpglist, epg);
									ScmListSort(CurrentEpglist,1);
									bypass = 1;
								//}
								//break;
							}
					}
					}
					}
					ScmListEnd(Epglist);
			}
			//}
			}else{
#if 0
				//_cprintf("time to check %d %d \n",starttime,i_StartTime);
				if((starttime + 8) < i_StartTime)
				{
					//_cprintf("======> time to check %d %d \n",starttime,i_StartTime);
					int bypass = 0;
					int Foundcurrent = 0;
					int needtoupdate = 1;
					starttime = i_StartTime;

					CurrentEpglist = pStreamDesc->pServiceDesc[index]->Epglist;
					if(CurrentEpglist)
					{
						while (ScmListCheckNext(CurrentEpglist)){
							EPG_T *epg = (EPG_T *)ScmListNext(CurrentEpglist);
							if(needtoupdate == 0)
							{
								if((epg->StartTime + epg->Duration) < starttime)
								{
									needtoupdate = 1;
								}
							}
						}
						ScmListEnd(CurrentEpglist);
					}
#if 0
					if(needtoupdate == 1)
					{
						if(Epglist)
						{
						ScmListStart(Epglist);	
						while (ScmListCheckNext(Epglist)){	
						EPG_T *epg = (EPG_T *)ScmListNext(Epglist);
						if (bypass == 0)
						{
						if(epg->StartDate == startdate)
						{
							if(Foundcurrent == 0)
							{
							if((epg->StartTime < starttime) && ((epg->Duration + epg->StartTime) > starttime) )
							{   //this is current prgram flag it
							//_cprintf("current program \n");
							//_cprintf("program start %d , duration %d\n",epg->StartTime,epg->Duration);
							//_cprintf("current time %d\n",StartTime);
							//if(pStreamDesc->pServiceDesc[index]->Epglist == NULL)
							//{
							//	CurrentEpglist = (SCM_LIST_T *)Evmalloc(sizeof(SCM_LIST_T));
							//	pStreamDesc->pServiceDesc[index]->Epglist = CurrentEpglist;
							//	ScmListInit(CurrentEpglist);    /* initialize the list */
							//	ScmListAttrCopy(CurrentEpglist, SizeofEpg, 1);
							//	ScmListAttrCompare(CurrentEpglist,EpgComparatorFunc);
								//starttime = 0;
							//}else{
								CurrentEpglist = pStreamDesc->pServiceDesc[index]->Epglist;
								ScmListClear(Epglist);
							//}
								ScmListAppend(CurrentEpglist, epg);
								ScmListSort(CurrentEpglist,1);
								Foundcurrent = 1;
						}
						}
						else
							{
								//if(Foundcurrent)
								//{
									ScmListAppend(CurrentEpglist, epg);
									ScmListSort(CurrentEpglist,1);
									bypass = 1;
								//}
								//break;
							}
					}
					}
					}
					ScmListEnd(Epglist);
						}
					}
#endif
				}
#endif

				
				}

			TvExternalEvent(TV_EPG_INFO,(void *)&Serviceid);
			}
			break;
		
				break;
			case 12: //STT-ATSC
				{
					if(pScmInfo->mpServiceRequest)
					{
					uint32_t *timeer = (uint32_t *)pData;
					timer[0] = *timeer;
					//_cprintf("old time %d, time %d\n",timer[0],*timeer);
					getTimeDate(timer,&i_StartTime,&i_StartDate);
#if 0
					if(*timeer > (timer[0] + 300))
					{
					SCM_LIST_T *Epglist;
					SCM_LIST_T *CurrentEpglist;
					
					int Foundcurrent = 0;
					
					timer[0] = *timeer;
					getTimeDate(timer,&StartTime,&StartDate);
					//_cprintf("time %d, date %d, time %d\n",timer[0],StartTime,StartDate);

					if(pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned])
					{
						pStreamDesc = pScmInfo->m_pSectionInfo[pScmInfo->CurrentTuned];
					}
					int nbService = pStreamDesc->ulNbServices;
					for(int i=0; i<nbService; i++)
					{
						Foundcurrent = 0;
						if(pStreamDesc->pServiceDesc[i])
						{
							Epglist = pStreamDesc->pServiceDesc[i]->EpglistScheduled;
							if(Epglist)
							{
									ScmListStart(Epglist);	
									while (ScmListCheckNext(Epglist)) {	
										EPG_T *epg = (EPG_T *)ScmListNext(Epglist);
										if(epg->StartDate == StartDate)
										{
											if((epg->StartTime < StartTime) && ((epg->Duration + epg->StartTime) > StartTime) )
											{   //this is current prgram flag it
												//_cprintf("current program \n");
												//_cprintf("program start %d , duration %d\n",epg->StartTime,epg->Duration);
												//_cprintf("current time %d\n",StartTime);
												if(pStreamDesc->pServiceDesc[i]->Epglist == NULL)
												{
														CurrentEpglist = (SCM_LIST_T *)Evmalloc(sizeof(SCM_LIST_T));
														pStreamDesc->pServiceDesc[i]->Epglist = CurrentEpglist;
														ScmListInit(CurrentEpglist);    /* initialize the list */
														ScmListAttrCopy(CurrentEpglist, SizeofEpg, 1);
														ScmListAttrCompare(CurrentEpglist,EpgComparatorFunc);
												}else{
														CurrentEpglist = pStreamDesc->pServiceDesc[i]->Epglist;
														ScmListClear(Epglist);
												}
												ScmListAppend(CurrentEpglist, epg);
												ScmListSort(CurrentEpglist,1);
												Foundcurrent = 1;
											}else
											{
												if(Foundcurrent)
												{
													ScmListAppend(CurrentEpglist, epg);
													ScmListSort(CurrentEpglist,1);
													break;
												}
												break;
											}

										}
										
									}
									ScmListEnd(Epglist);
							}
						}
					}
					}
#endif
					}
					break;
				}
				
			case 13: //RRT-ATSC
				break;
		default:
			break;
		}
}

int32_t pfnSipsiCallback(void *pBufAdd,int32_t pBufSize)  /* Sipsi CallBack declaration*/
{
#ifdef _USRDLL
	if(!pScmInfo->mScanDone || !pScmInfo->mpServiceRequest)
	{

		pScmInfo->mbTuneSuccess = TRUE;	
		//uncomment for DVBT where section is passed through TS data
		/*ParseTable(pBufAdd,NULL,pBufSize);*/
	}
	/*comment for DVBT*/
	ParseTable(pBufAdd,NULL,pBufSize);
#else 
		__ParseTable(pBufAdd,NULL,0);
#endif
	return 0;
}

int32_t pfnVideoCallback(void * pAuInfo)
{
	//SCM_DEBUG(SCM_SDK,("[SCM_SDK]: Video Data  %d bytes\n",pAuInfo->esBufSize));
	return 0;
}
int32_t pfnAudioCallback(void * pAuInfo)
{
	//SCM_DEBUG(SCM_SDK,("[SCM_SDK]: Audio Data for %d bytes\n",pAuInfo->esBufSize));
	return 0;
}



#if 0

int __stdcall scm_getServiceList(SCM_SERVICE_T ServList)
{
	return 0;
}
#endif


bool_t __stdcall scm_channelAvailable(void)
{
	return TRUE;
}

int32_t __stdcall scm_getChannelScannedNo(void)
{
	return 0;
}

bool_t __stdcall scm_country(int32_t countryCode,MANUAL_SCAN_T *ScanParam/*int32_t Freq*/)
{
#if 0  //multiple
	scmCountryIdx = countryCode;

	if(ScanParam != NULL){
		scmFreq    = ScanParam->FreqHz;
		scmBw      = ScanParam->Bw / 100;
		scmAutoscan   = FALSE;
	}
#endif
	return TRUE;
}

