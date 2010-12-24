#include "osal_types.h"
#include "osal_memory.h"
#include "osal_list.h"
#include "scm_event.h"
#include "demux_tssplit.h"
#include "scm_info.h"


#include "scm_exp.h"
#include "scm_timer.h"
#include "scm_config.h"
#include "scm_tuner.h"


extern unsigned long g_dwTlsIndex;
static SCM_TIMER_T *g_pTimerList = NULL;
static unsigned long g_nTimerSeqNum = 1;
extern SIGNAL_INFO_T *sigInfo;
extern SCM_INFO_T *pScmInfo;

/*uint64_t ScmSetTimer(uint16_t nTimeOut, SCM_TIMER_PROC_T pfnTimerFunc)*/
unsigned int ScmSetTimer(unsigned int nTimeOut, SCM_TIMER_PROC_T pfnTimerFunc)

{
	SCM_TIMER_T *pTimerData;
	
	UINT_PTR nTimerID;

/*	nTimerID = SetTimer(NULL, 24, nTimeOut, (TIMERPROC)ScmTimerProc); */
	nTimerID = SetTimer(NULL, 24, nTimeOut, (TIMERPROC)ScmTimerProc); 

	if (nTimerID==0)
		return 0;
    pTimerData = (SCM_TIMER_T*)Evmalloc(sizeof(SCM_TIMER_T));
	if (!pTimerData)
		return 0;

	pTimerData->nTimerID = nTimerID;
	pTimerData->pDestThread = (SCM_THREAD_CONTEXT_T*)TlsGetValue(g_dwTlsIndex);/* The time-out event destination thread is the current calling thread. */
	pTimerData->nTimeOut = nTimeOut;
	pTimerData->nSequenceNum = g_nTimerSeqNum;
	pTimerData->pfnTimerFunc = pfnTimerFunc;
	pTimerData->pNext = g_pTimerList;
	g_pTimerList = pTimerData;
	g_nTimerSeqNum++;

	return pTimerData->nSequenceNum;
}

/*void __stdcall ScmTimerProc(handle_t hwnd, uint16_t uMsg, uint64_t *nTimerID, dword_t dwTime)*/
void __stdcall ScmTimerProc(handle_t hwnd, uint16_t uMsg, UINT_PTR nTimerID, dword_t dwTime)
{
	SCM_TIMER_T *pTimerData = g_pTimerList;
	//_cprintf("__timeout %d\n",pTimerData->nTimeOut);
	//_cprintf("__timer ID %d\n",pTimerData->nTimerID);
	//_cprintf("__timer %d\n",pTimerData);
	//printf("ScmTimerProc\n");
    //SCM_DEBUG(SCM_TIMER, ("[SCM_TIMER]: ==> In ScmTimerProc() ID%d\n",nTimerID));
	while (pTimerData)
	{
		//_cprintf("__timer___________%d\n",*nTimerID);
		if (pTimerData->nTimerID == nTimerID)
		{
			//_cprintf("timer)))))))))))%d\n",*nTimerID);
			//sigInfo;
			EmbvidGetMonit(sigInfo); 
			(*(pTimerData->pfnTimerFunc))(sigInfo,pScmInfo->pMonitUsrCtx);

			//either invoke the callback function directly 
			//	or 
		    //create an externel timer event and post it
 			
#if 0
			if (pTimerData->pfnTimerFunc)
			{
				// Invoke the call back function.
				(*(pTimerData->pfnTimerFunc))( parameters);
			} else
			{
				//Create an external timer event and post it .
				ScmPostThreadEvent
			}
#endif
			return;
		}
		pTimerData = pTimerData->pNext;
	}

}
