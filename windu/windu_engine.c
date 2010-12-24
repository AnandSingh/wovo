/* -------------------------------------------- Header File Inclusion */
#include "osal_types.h"
#include "osal_stdio.h"
#include "osal_process.h"
#include "osal_memory.h"
#include "osal_string.h"
#include "osal_list.h"

#include "demux_tssplit.h"
#include "scm_event.h"
#include "scm_info.h"
#include "scm_exp.h"
#include "scm_timer.h"
#include "scm_config.h"
#include "scm_tuner.h"



/* -------------------------------------------- External Global Variables */
/* Thread Local Storage */
SCM_THREAD_CONTEXT_T g_ScmThreadContext;
/*Current state of mobile Tv stack*/
uint8_t tvCurrentState = 0;
/* -------------------------------------------- Exported Global Variables */
extern SCM_INFO_T *pScmInfo;
/* -------------------------------------------- Static Global Variables */
/* Local Thread Index */
//static unsigned long g_dwTlsIndex=0xFFFFFFFF;
unsigned long g_dwTlsIndex=0xFFFFFFFF;


/* -------------------------------------------- Functions */
unsigned __stdcall SCMEngine(void *Param)
{
	//g_pTimerList=NULL;
	//g_nTimerSeqNum = 1;
	ScmInitEngine();
	ScmInitMsgBuf();
   	ScmRun();
	ScmFreeMsgBuf();
	return 0;
}

void ScmInitEngine(void)
{
	int i;
	g_dwTlsIndex = EvTlsAlloc();
	// For the platform independent engine..
	// Save the thread context pointer to the TLS.
	if (0xFFFFFFFF != g_dwTlsIndex)
		EvTlsSetValue(g_dwTlsIndex, (void *)&g_ScmThreadContext);

	Evmemset(&g_ScmThreadContext,0,sizeof(SCM_THREAD_CONTEXT_T));

	g_ScmThreadContext.pEventQueueFront=NULL; 
	g_ScmThreadContext.pEventQueueRear=NULL;

	/* Set all event pool are empty. */
	for (i=0; i<MAX_TV_EVENT; i++)
		g_ScmThreadContext.EventPool[i].nEventID = INVALID_EVENT_ID;

	g_ScmThreadContext.nEventPoolIdx =0;
}



///////////////////////////////////////////////////////////////////////////////////////////
//   nMsgBufHdr  (Get from the head) <============== (Append to the rear) nMsgBufRear
///////////////////////////////////////////////////////////////////////////////////////////
/* Initialize the external event buffer at the current thread. */
bool_t ScmInitMsgBuf()
{
	SCM_THREAD_CONTEXT_T* pThreadContext = (SCM_THREAD_CONTEXT_T*)EvTlsGetValue(g_dwTlsIndex);
	TV_MSG_POOL_T *pMsgPool;

	pThreadContext->pExtEventPool = (void*)Evmalloc(sizeof(TV_MSG_POOL_T));
	pMsgPool =(TV_MSG_POOL_T*)(pThreadContext->pExtEventPool);
	if (NULL==pMsgPool) 
		return FALSE;
	Evmemset(pMsgPool, 0, sizeof(TV_MSG_POOL_T));

	EvCreateMutext(&(pMsgPool->MutextForPool));
	//ThreadIdForEvent
	pMsgPool->ThreadIdForEvent = EvGetCurrentThreadId();
	
	return TRUE;
}

/* Free the external event buffer at the current thread. */
bool_t ScmFreeMsgBuf()
{
	SCM_THREAD_CONTEXT_T* pThreadContext = (SCM_THREAD_CONTEXT_T*)EvTlsGetValue(g_dwTlsIndex);

	if (NULL!=pThreadContext && NULL!=pThreadContext->pExtEventPool)
	{
		Evfree(pThreadContext->pExtEventPool);
		pThreadContext->pExtEventPool= NULL;
		return TRUE;
	}

	return FALSE;
}


bool_t ScmPostEvent(SCM_EVENT_T *pEvent)
{
	SCM_THREAD_CONTEXT_PT pThreadContext=NULL;

	//if (g_pfnGetThreadContext)
		pThreadContext = (SCM_THREAD_CONTEXT_T*)EvTlsGetValue(g_dwTlsIndex);
	if (!pThreadContext) return FALSE;

	if (pEvent == NULL) return FALSE;

	if (pThreadContext->pEventQueueRear==NULL) 
	{ 
		/* The first event in queue. */
		pThreadContext->pEventQueueFront=pThreadContext->pEventQueueRear=pEvent;
	}
	else 
	{
		/* Append the event to queue.*/
		pThreadContext->pEventQueueRear->pNext=pEvent;
		pEvent->pNext=NULL;
		pThreadContext->pEventQueueRear=pEvent;
	}
	return TRUE;
}



SCM_EVENT_T * ScmGetEvent()
{
	/* Get an event from event queue if available. */
	SCM_EVENT_T *pEvent = NULL;
	SCM_THREAD_CONTEXT_PT pThreadContext= NULL;

	//if (g_pfnGetThreadContext)
		pThreadContext = (SCM_THREAD_CONTEXT_T*)EvTlsGetValue(g_dwTlsIndex);//XGetThreadContext();//(*g_pfnGetThreadContext)();
	if (!pThreadContext) return NULL;

	if (pThreadContext->pEventQueueFront != NULL)
	{
		pEvent = pThreadContext->pEventQueueFront;
		pThreadContext->pEventQueueFront = pThreadContext->pEventQueueFront->pNext;
		/* Set the end of queue to NULL if queue is empty.*/
		if (pThreadContext->pEventQueueFront == NULL)
			pThreadContext->pEventQueueRear = NULL;
	}
	return pEvent;
}

typedef bool_t (*IS_CODITION_OK_T)(void*p);
#define SME_UNUSED_VOIDP_PARAM(x) {void* a = (x); (x) = a;}

static bool_t ScmIsMsgAvailable(void *pArg)
{
	SCM_THREAD_CONTEXT_T* p = (SCM_THREAD_CONTEXT_T*)EvTlsGetValue(g_dwTlsIndex);//XGetThreadContext();
	TV_MSG_POOL_T *pMsgPool;
	if (NULL==p ||  NULL== p->pExtEventPool)
		return FALSE;

	SME_UNUSED_VOIDP_PARAM(pArg);

	pMsgPool = (TV_MSG_POOL_T*)p->pExtEventPool;

	if (pMsgPool->nMsgBufHdr==pMsgPool->nMsgBufRear)
		return FALSE; // empty buffer.

	return TRUE;
}


/* Thread-safe action to remove an external event from the current thread event pool.*/
static void ScmGetMsg(void *pArg)
{
	TV_MSG_T *pMsg = (TV_MSG_T*)pArg;
	SCM_THREAD_CONTEXT_T* p = (SCM_THREAD_CONTEXT_T*)EvTlsGetValue(g_dwTlsIndex);
	TV_MSG_POOL_T *pMsgPool;
	if (NULL==pMsg || NULL==p || NULL==p->pExtEventPool)
		return;

	pMsgPool = (TV_MSG_POOL_T*)(p->pExtEventPool);

	if (pMsgPool->nMsgBufHdr==pMsgPool->nMsgBufRear)
		return; // empty buffer.

	Evmemcpy(pMsg,&(pMsgPool->MsgBuf[pMsgPool->nMsgBufHdr]),sizeof(TV_MSG_T));

	pMsgPool->MsgBuf[pMsgPool->nMsgBufHdr].nMsgID =0;

	pMsgPool->nMsgBufHdr = (pMsgPool->nMsgBufHdr+1)%MSG_BUF_SIZE;

}


// Wait for an event signaled and then take some thread-safe actions.
int ScmWaitForEvent(unsigned long *pEvent, handle_t *pMutex, unsigned long MiliSec, IS_CODITION_OK_T pIsConditionOK, void *pCondParam,
				  THREAD_SAFE_ACTION_T pAction, void *pActionParam)
{


	dword_t nRet = 0;//WAIT_OBJECT_0;
	MSG WinMsg;
    pIsConditionOK;
	if (pEvent==NULL || pMutex==NULL || pIsConditionOK==NULL)
		return -1;

	// GetMessage()
	// If the function retrieves a message other than WM_QUIT, the return value is nonzero.
	// If the function retrieves the WM_QUIT message, the return value is zero. 
	while (GetMessage(&WinMsg, NULL, 0, 0))
	{
		if (WinMsg.message == WM_EXT_EVENT_ID)
		{
			// External event is triggered. App go to running state.
			if (pAction)
			{
				EvMutexLock(pMutex);
				(*pAction)(pActionParam);
				EvMutexUnlock(pMutex);
			}
			return  0; 
		} 
		// Handle Windows messages in MMI thread, for example audio messages.
		else //if (handle_win_msg_in_mmi_thread(&WinMsg) == FALSE)
		{
			// MSDN: DispatchMessage API
			// The DispatchMessage function dispatches a message to a window procedure.

			// MSDN: DefWindowProc API
			// The DefWindowProc function calls the default window procedure to provide default 
			// processing for any window messages that an application does not process. This 
			// function ensures that every message is processed. 

			// MSDN: WM_TIMER
			// The WM_TIMER message is posted to the installing thread's message queue 
			// when a timer expires. You can process the message by providing a WM_TIMER case 
			// in the window procedure. Otherwise, the default window procedure (DefWindowProc) 
			// will call the TimerProc callback function specified in the call to the SetTimer 
			// function used to install the timer. 

			// Handle Windows messages by the default window procedure in MMI thread  
			// for example calling the TimerProc callback function for timer messages.
			DispatchMessage(&WinMsg);
		};
	}

	return 0;

}



//#define SME_EVENT_EXIT_LOOP	16

bool_t ScmGetExtEvent(SCM_EVENT_T* pEvent)
{
	TV_MSG_T NativeMsg;
	int i=0;
	int ret=0;

	SCM_THREAD_CONTEXT_T* p = (SCM_THREAD_CONTEXT_T*)TlsGetValue(g_dwTlsIndex);//XGetThreadContext();
	TV_MSG_POOL_T *pMsgPool;
	if (NULL==pEvent || NULL==p || NULL==p->pExtEventPool)
		return FALSE;

	pMsgPool = (TV_MSG_POOL_T*)(p->pExtEventPool);

	memset(&NativeMsg,0,sizeof(NativeMsg));
	while (TRUE)
	{
		while (i<10)
		{
		   ret = ScmWaitForEvent(&(pMsgPool->ThreadIdForEvent), &(pMsgPool->MutextForPool), 10000, (IS_CODITION_OK_T)ScmIsMsgAvailable, NULL, (THREAD_SAFE_ACTION_T)ScmGetMsg,&NativeMsg);
		   if (ret != WAIT_TIMEOUT)
				break;

			i++;
		}

		if (ret == WAIT_TIMEOUT)
			return FALSE;
		else
		{
			// Translate the native message to SCM event.
			memset(pEvent,0,sizeof(SCM_EVENT_T));
			pEvent->nEventID = NativeMsg.nMsgID;
			pEvent->bIsConsumed = FALSE;
			memcpy(&(pEvent->Data),&(NativeMsg.Data), sizeof(union SCM_DATA_T));
			SCM_DEBUG(SCM_ENGINE, ("[SCM_ENGINE]: Externel Event Received !!\n\n") );
			return TRUE;
		}
	}; // while (TRUE)
}


/*******************************************************************************************
* DESCRIPTION:   
* INPUT:  
* OUTPUT: None.
* NOTE: 
********************************************************************************************/
bool_t ScmDispatchEvent(SCM_THREAD_CONTEXT_PT pThreadContext,SCM_EVENT_T *pEvent)
{

	if (pThreadContext==NULL || pEvent==NULL) return FALSE;
	/*Dispatch it to active applications*/
	if (pEvent->nEventID == TV_SCAN_COMPLETE || pEvent->nEventID == TV_SDT_INFO || pEvent->nEventID == TV_EPG_INFO || pEvent->nEventID == TV_SCAN_CURRENT_FREQ || pEvent->nEventID == 100)
	{
		SCM_DEBUG(SCM_ENGINE, ("[SCM_ENGINE]: ScmDispatchEvent ==> EventId %s, Data %p\n", getStateString(pEvent->nEventID), pEvent->Data));
       // Sleep(2000);
 		pScmInfo->m_pfnScmStatusCallback(pEvent->nEventID,(void *)(pEvent->Data.pData),pScmInfo->pUsrCtx); //meher
		if(pEvent->nEventID == TV_SCAN_COMPLETE )  //multiple
			 SetEvent(pScmInfo->hSIPSIDataAvailable);
		//pScmInfo->m_pfnScmStatusCallback(pEvent->nEventID,(void *)(pEvent->Data.pData),pScmInfo->pUsrCtx);

#if 0
		if(pEvent->nEventID == TV_SCAN_COMPLETE )  //multiple
				SetEvent(pScmInfo->hSIPSIDataAvailable);
		pScmInfo->m_pfnScmStatusCallback(pEvent->nEventID,(void *)(pEvent->Data.pData),pScmInfo->pUsrCtx);
#if 0
		if (pEvent->bIsConsumed) 
				break;
			else pApp = pApp->pNext; 
#endif
#endif
		/* This event has destination application. Dispatch it if the application is active.*/
		/* Dispatch it to an destination application. */
		//ScmDispatchEvent(pEvent, pEvent->pDestApp);
	}
	return TRUE;
}




bool_t ScmDelExtEvent(SCM_EVENT_T *pEvent)
{
	if (0==pEvent)
		return FALSE;
	if (pEvent->Data.pData){
		//free(pEvent->Data.pData);
		pEvent->Data.pData=NULL;
	}
	return TRUE;
}

/*******************************************************************************************
* DESCRIPTION:  This API function is the state machine engine event handling loop function. 
*  It will never exit. 
* INPUT:  
* OUTPUT: None.
* NOTE: 

Engine has to check internal event first [no need now], because ScmActivateApp() may trigger some internal events which is not implemented

Case:
	
  
*******************************************************************************************/
#define SME_INVALID_EVENT_ID	1
bool_t ScmDeleteEvent(SCM_EVENT_T *pEvent)
{
	if(!pEvent) return FALSE;
	pEvent->nEventID = SME_INVALID_EVENT_ID;
	pEvent->bIsConsumed = TRUE;
	return TRUE;
}

void ScmRun()
{
	SCM_EVENT_T ExtEvent;
	SCM_EVENT_T *pEvent=NULL;

	const TvStateStruct* pTvStateMap;
	
	SCM_THREAD_CONTEXT_PT pThreadContext=NULL;

		pThreadContext = (SCM_THREAD_CONTEXT_T*)TlsGetValue(g_dwTlsIndex);//XGetThreadContext();
	if (!pThreadContext) return;



	while (TRUE)
	{
		/* Check the internal event pool firstly. */
		//pEvent = ScmGetEvent();
		if (pEvent == NULL)
		{
			/* Wait for an external event. */
			if (FALSE == ScmGetExtEvent(&ExtEvent)) 
				return; // Exit the thread.
            
			//pEvent->nOrigin = EVENT_ORIGIN_EXTERNAL;  [TODO]
            /* Call hook function on an external event coming. */
      
			pEvent = &ExtEvent;
			SCM_DEBUG(SCM_ENGINE, ("[SCM_ENGINE]: Get ExtEvent ==> MsgId %s, Data %d\n", getStateString(pEvent->nEventID), pEvent->Data));
			pTvStateMap = GetTvStateMap();
			if(*pTvStateMap[pEvent->nEventID].pTvStateFunc != NULL)
             (*pTvStateMap[pEvent->nEventID].pTvStateFunc)(pEvent->Data);
		}	
		do {

			if(pEvent != NULL)
			{
				 ScmDispatchEvent(pThreadContext, pEvent);
				 /* Free internal event. Free external event later. */
				  if (pEvent != &ExtEvent)
					ScmDeleteEvent(pEvent);
			}else
			{
				break;
			}
				/* Get an event from event queue if available. */
			pEvent = ScmGetEvent();
			if (pEvent != NULL)
			{
						//pEvent->nOrigin = EVENT_ORIGIN_EXTERNAL;  [TODO]
				SCM_DEBUG(SCM_ENGINE, ("[SCM_ENGINE]: Get IntEvent ==> MsgId %s, Data %d\n", getStateString(pEvent->nEventID), pEvent->Data));
				pTvStateMap = GetTvStateMap();
				if(*pTvStateMap[pEvent->nEventID].pTvStateFunc != NULL)
					(*pTvStateMap[pEvent->nEventID].pTvStateFunc)(pEvent->Data);
			}
		} while (TRUE); /* Get all events from the internal event pool.*/
        
		ScmDelExtEvent(&ExtEvent);
		ScmDeleteEvent(&ExtEvent); 
		/* Free external event if necessary. */
	} /* Wait for an external event. */
}




/*******************************************************************************************
* DESCRIPTION:  Get an event data buffer from event pool.
* INPUT:  None
* OUTPUT: New event pointer. If pool is used up, return NULL. 
* NOTE: 
*   
*******************************************************************************************/
SCM_EVENT_T *ScmGetAEvent()
{
	SCM_EVENT_T *e=NULL;
	int nOldEventPoolIdx;
	SCM_THREAD_CONTEXT_PT pThreadContext=NULL;

	//if (g_pfnGetThreadContext)
		pThreadContext = (SCM_THREAD_CONTEXT_T*)TlsGetValue(g_dwTlsIndex);//(*g_pfnGetThreadContext)();
	if (!pThreadContext) return NULL;

	nOldEventPoolIdx = pThreadContext->nEventPoolIdx;
	do {
		if (pThreadContext->EventPool[pThreadContext->nEventPoolIdx].nEventID == SME_INVALID_EVENT_ID)
		{
			/*This event data buffer is available.*/
			e = &(pThreadContext->EventPool[pThreadContext->nEventPoolIdx]);
			pThreadContext->nEventPoolIdx = (pThreadContext->nEventPoolIdx+1)%MAX_TV_EVENT;
			return e;
		} else
		{
			pThreadContext->nEventPoolIdx = (pThreadContext->nEventPoolIdx+1)%MAX_TV_EVENT;
			if (pThreadContext->nEventPoolIdx == nOldEventPoolIdx)
				return NULL;
		}
	} while(TRUE);

}

