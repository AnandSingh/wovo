#ifndef _H_SCM_TIMER_
#define _H_SCM_TIMER_


/* -------------------------------------------- Header File Inclusion */


typedef struct tagXTIMER_T
{
	unsigned int nTimerID;
	unsigned int nTimeOut;
	unsigned long nSequenceNum;
	SCM_TIMER_PROC_T pfnTimerFunc;
	SCM_THREAD_CONTEXT_T *pDestThread;
	struct tagXTIMER_T *pNext;
} SCM_TIMER_T;


unsigned int ScmSetTimer(unsigned int nTimeOut, SCM_TIMER_PROC_T pfnTimerFunc);
void __stdcall ScmTimerProc(handle_t hwnd, uint16_t uMsg, UINT_PTR nTimerID, dword_t dwTime);


#else
#error "Multiple inclusions of scm_timer.h"
#endif //_H_SCM_TIMER_
