
#ifndef _H_SCM_EVENT_
#define _H_SCM_EVENT_

/* -------------------------------------------- Header File Inclusion */
#include "scm_event_data.h"


//static unsigned long g_dwTlsIndex=0xFFFFFFFF;

/* -------------------------------------------- Global Definitions */

#define BEGIN_TV_TRANSITION_MAP \
    unsigned char TV_TRANSITIONS[] = {\

#define TV_TRANSITION_MAP_ENTRY(entry)\
    entry,
#define END_TV_TRANSITION_MAP(data) \
    0 };\
    TvExternalEvent(TV_TRANSITIONS[tvCurrentState], data);

/* -------------------------------------------- Structures/Data Types */


/* Note: Each thread has an independent message pool, an event/condition and a Mutext. 
When a new external event post, set the event/condition signaled.
The Mutext for a thread pool is to synchronize the message pool access between the event sending thread and the receiving thread.
*/
#define MSG_BUF_SIZE  100


union SCM_DATA_T
{
	void* pData;
	unsigned int nSize;
};


#define MAX_TV_EVENT			100   /* The size of the pool for internal events */
#define INVALID_EVENT_ID	1
//internel queuee
typedef struct SCM_EVENT_T_TAG
{
	uint16_t nEventID;
	struct SCM_EVENT_T_TAG *pNext; 
	union SCM_DATA_T Data;
	//SME_INT32 nOrigin :8; /* An internal event or an external event */
	unsigned int bIsConsumed :8; /* Is consumed. */
	//[TODO]
}SCM_EVENT_T,*SCM_EVENT_PT;


typedef struct SCM_THREAD_CONTEXT_T_TAG{
	int nEventPoolIdx;
	struct SCM_EVENT_T_TAG EventPool[MAX_TV_EVENT]; /* Internal event buffer */
	struct SCM_EVENT_T_TAG *pEventQueueFront;  // internel event queue
	struct SCM_EVENT_T_TAG *pEventQueueRear;   //internel event queue
	void *pData;								/* Preserved for application. */
	void *pExtEventPool;						/* A pointer to the external event pool information. */
}SCM_THREAD_CONTEXT_T, *SCM_THREAD_CONTEXT_PT;

typedef struct tagTVMSG
{
	int nMsgID; // conatins the state information//0: stand for empty entity
	union SCM_DATA_T Data;
	SCM_THREAD_CONTEXT_T* pDestThread;
}	TV_MSG_T;

typedef struct tagTVMSGPOOL
{
	int nMsgBufHdr;
	int nMsgBufRear;
	TV_MSG_T MsgBuf[MSG_BUF_SIZE];
    unsigned long ThreadIdForEvent; //[TODO]
	void * MutextForPool; //[TODO]
} TV_MSG_POOL_T;


SCM_EVENT_T * ScmGetEvent();

//struct TvStateStruct;
//typedef void (*TvStateFunc)(TvData *);
typedef uint16_t (*TvStateFunc)(SCM_DATA_T);

typedef struct _TvStateStruct 
{
    TvStateFunc pTvStateFunc;    
}TvStateStruct;


/*Protected */
enum{
	EVENT_IGNORED = 0xFE,
	CANNOT_HAPPEN
};

/* -------------------------------------------- Function Declarations */


/*Public function defination*/
void Init_TvStateMachine(int maxStates);
//void Deint_TvStateMachine();

/*Protected function defination*/
void TvExternalEvent(unsigned char newState, void *pTvData);
void TvInternalEvent(unsigned char newState, void *pTvData);
const TvStateStruct* GetTvStateMap();

/*Private function defination*/
void TvStateEngine(void);

//void STATE_debug();



#define WM_EXT_EVENT_ID  0xBFFF

typedef int (*THREAD_SAFE_ACTION_T)(void*p);




int ScmPostThreadEvent(SCM_THREAD_CONTEXT_T* pDestThreadContext, int nMsgID, void* Param1, int Param2);	

/*static*/ SCM_EVENT_T *ScmGetAEvent();

unsigned __stdcall SCMEngine(void *Param);
void ScmInitEngine(void);
int ScmInitMsgBuf(void);
void ScmRun(void);
bool_t ScmFreeMsgBuf(void);
bool_t ScmPostEvent(SCM_EVENT_T *pEvent);
void TvInternelEvent(unsigned char newState, void *pData);
unsigned __stdcall TV_Tune_multiple(void *Param);


#else
#error "Multiple inclusions of scm_event.h"
#endif //_H_SCM_EVENT_

