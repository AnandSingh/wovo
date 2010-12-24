/* -------------------------------------------- Header File Inclusion */
#include "osal_types.h"
#include "osal_memory.h"
#include "osal_stdio.h"
#include "osal_process.h"
#include "scm_event.h"
#include "scm_config.h"


/* ----------------------------------------- External Global Variables */
extern uint8_t tvCurrentState;
extern SCM_THREAD_CONTEXT_T g_ScmThreadContext;

/* ----------------------------------------- Exported Global Variables */

/* ----------------------------------------- Static Global Variables */

/* ----------------------------------------- Functions */

void STATE_debug()
{
	Evprintf("\n**\n");
	//Evprintf("\t max state %d\n\t Current state %d\n\t Event Gernrated %d\n\t Event Data %u\n",_tvMaxStates,tvCurrentState, _tvEventGenerated,_pTvEventData);
	Evprintf("**\n");
}




void TvExternalEvent(unsigned char newState, void *pData)
{
    //TvData *pTvData = (TvData *)pData;
	// if we are supposed to ignore this event
    if (newState == EVENT_IGNORED || newState == CANNOT_HAPPEN) {
        // just delete the event data, if any
        //if (pData)  
            //Evfree(pData);
    }
    else {
        // generate the event and execute the state engine
		SCM_DEBUG(SCM_EVENT, ("[SCM_EVENT]: Post ExtEvent ==> newState %s, Data %p\n", getStateString(newState), pData));
		tvCurrentState = newState;
		ScmPostThreadEvent(&g_ScmThreadContext, newState, pData, 0);
    }
}


#define EVENT_TIMER		4 /* Regular timer event */

static void ScmAppendMsgToBuf(void *pArg)
{
	TV_MSG_T *pMsg = (TV_MSG_T*)pArg;
	int nHdr;
	TV_MSG_POOL_T *pMsgPool;
	if (NULL==pMsg || NULL==pMsg->pDestThread || NULL==pMsg->pDestThread->pExtEventPool)
		return;

	pMsgPool = (TV_MSG_POOL_T*)(pMsg->pDestThread->pExtEventPool);
	
	if (((pMsgPool->nMsgBufRear+1) % MSG_BUF_SIZE) == pMsgPool->nMsgBufHdr)
		return; // buffer full.

	// Prevent duplicate SME_EVENT_TIMER event triggered by a timer in the queue.
	nHdr = pMsgPool->nMsgBufHdr;
	if (EVENT_TIMER == pMsg->nMsgID)
	{
		while (nHdr != pMsgPool->nMsgBufRear)
		{
			if (EVENT_TIMER == pMsgPool->MsgBuf[nHdr].nMsgID/* && pMsg->nSequenceNum == pMsgPool->MsgBuf[nHdr].nSequenceNum*/)
				return; 
			nHdr = (nHdr+1) % MSG_BUF_SIZE;
		}
	}

	memcpy(&(pMsgPool->MsgBuf[pMsgPool->nMsgBufRear]),pMsg,sizeof(TV_MSG_T));

	pMsgPool->nMsgBufRear = (pMsgPool->nMsgBufRear+1)%MSG_BUF_SIZE;
}

int ScmSignalEvent(unsigned long *ThreadId, void**pMutex, THREAD_SAFE_ACTION_T pAction, void *pActionParam)
{
	int ret=0; 
	if (pMutex==NULL)
		return -1;
	if (pAction)
	{
		EvMutexLock(pMutex);
		(*pAction)(pActionParam); // At this time, (*pIsConditionOK)() should return TRUE.
		EvMutexUnlock(pMutex);
	}
	if (0==*ThreadId) return -1;

	PostThreadMessage(*ThreadId, WM_EXT_EVENT_ID, 0, 0);
	return 0;
}


int ScmPostThreadEvent(SCM_THREAD_CONTEXT_T* pDestThreadContext, int nMsgID, void* Param1, int Param2)
{
	TV_MSG_T Msg;
	TV_MSG_POOL_T *pMsgPool;
	if (/*nMsgID==0 ||*/ NULL== pDestThreadContext || NULL==pDestThreadContext->pExtEventPool)
		return -1;

	Msg.nMsgID = nMsgID;
	Msg.pDestThread = pDestThreadContext;
	if(Param1 == NULL)
		Msg.Data.nSize = Param2;
	else
		Msg.Data.pData = Param1;
	
	pMsgPool = (TV_MSG_POOL_T *)(pDestThreadContext->pExtEventPool);
	ScmSignalEvent(&(pMsgPool->ThreadIdForEvent),&(pMsgPool->MutextForPool),(THREAD_SAFE_ACTION_T)ScmAppendMsgToBuf,(void *)&Msg);
	return 0;
}


int ScmPostIntEvent(SCM_THREAD_CONTEXT_T* pDestThreadContext, int nMsgID, void* Param1, int Param2)
{
	SCM_EVENT_T *pEvent = ScmGetAEvent();
    if(pEvent != NULL)
	{
		pEvent->nEventID = nMsgID;
		pEvent->bIsConsumed = FALSE;
		if(Param1 == NULL)
			pEvent->Data.nSize = Param2;
		else
			pEvent->Data.pData = Param1;
		
		pEvent->pNext = NULL;
		ScmPostEvent(pEvent);
	}else
	{
		//Sleep(2000);
		return 1;
	}
	
	return 0;
}


void TvInternalEvent(unsigned char newState, void *pTvData)
{
    if (newState == EVENT_IGNORED || newState == CANNOT_HAPPEN) {
        // just delete the event data, if any
        if (pTvData)  
            Evfree(pTvData);
    }
    else {
        // generate the event and execute the state engine
		SCM_DEBUG(SCM_EVENT, ("[SCM_EVENT]: Post IntEvent ==> newIntState %s, Data %d\n", getStateString(newState), pTvData));
		tvCurrentState = newState;
		ScmPostIntEvent(&g_ScmThreadContext, newState, pTvData, 0);
		//while(ScmPostIntEvent(&g_ScmThreadContext, newState, pData, 0) == 1);
    }
}
