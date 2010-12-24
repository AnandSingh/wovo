#include "osal_process.h"
#include "osal_memory.h"
#include "osal_stdio.h"
#include "osal_string.h"
#include <stdio.h>
#include <process.h>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#include <windows.h>

/*---------------------------------------------------------------------------*/

void Evusleep(unsigned long iMicroSecs)
{
	Sleep((max(iMicroSecs, 1000)/1000));
}



/*---------------------------------------------------------------------------*/

int EvInitLock(EvMutex* ppMutex, char *szName)
{
	*ppMutex = Evmalloc(sizeof(CRITICAL_SECTION));
	if(!*ppMutex)
		return -1;
	InitializeCriticalSection(*ppMutex);

	return 0;
}
	
/*---------------------------------------------------------------------------*/

int EvAcquireLock(EvMutex pMutex, long lTimeOut)
{
	if(!pMutex)
		return -1;

	EnterCriticalSection(pMutex);

    return 0;
}

/*---------------------------------------------------------------------------*/

int EvReleaseLock(EvMutex pMutex)
{	
	if(!pMutex)
		return -1;

	LeaveCriticalSection(pMutex);
	
	return 0;
}

/*---------------------------------------------------------------------------*/

int EvCloseMutex(EvMutex pMutex)
{
	if(!pMutex)
		return -1;

	DeleteCriticalSection(pMutex);
	Evfree(pMutex);
	return 0;
}


/*---------------------------------------------------------------------------*/

int EvCreateMutext(EvMutex  *ppMutex)
{
    int  ret_code = 0; 

    if (ppMutex == NULL) {
        return -1;
    }

    // the mutex can be used to sync. threads in this process only.
	// The state of a mutex object is signaled when it is not owned by any thread.
	// The creating thread can use the bInitialOwner flag to request immediate ownership of the mutex. 
	// Otherwise, a thread must use one of the wait functions to request ownership. 
	// When the mutex's state is signaled, one waiting thread is granted ownership, the mutex's state changes to nonsignaled, 
	// and the wait function returns. Only one thread can own a mutex at any given time. 
	// The owning thread uses the ReleaseMutex function to release its ownership.	

	// bInitialOwner: [in] If this value is TRUE and the caller created the mutex, the calling thread obtains initial ownership of the mutex object. Otherwise, the calling thread does not obtain ownership of the mutex. To determine if the caller created the mutex, see the Return Values section.
    *ppMutex = CreateMutex(NULL, /*bInitialOwner*/FALSE, NULL);
    if ( *ppMutex != NULL)
        return 0;
    else {
        return -1;
    }
}

/*---------------------------------------------------------------------------*/

int EvMutexLock(EvMutex *ppMutex)
{
    int  ret_code = 0; 

    if (ppMutex == NULL) {
        return -1;
    }
   if (WaitForSingleObject(*ppMutex, INFINITE)!= WAIT_FAILED)
        return 0; // the mutex's state changes to nonsignaled
    else {
        return -1;
    }
}

/*---------------------------------------------------------------------------*/

int EvMutexUnlock(EvMutex *ppMutex)
{
	int  ret_code = 0; 
    
    if (ppMutex == NULL){
        return -1;
    }
	// Releases ownership of the specified mutex object.
    if (ReleaseMutex(*ppMutex) != 0)
        return 0; // Release the ownership. The mutex's state changes to signaled. 
    else {
        return -1;
    }
}

/*---------------------------------------------------------------------------*/

int EvMutexDestroy(EvMutex  *ppMutex)
{
    int  ret_code = 0; 

    if (ppMutex == NULL) {
        return -1;
    }
    // the mutex can be used to sync. threads in this process only.
    if (CloseHandle(*ppMutex) != 0)
        return 0;
    else {
        return -1;
    }
}

/*---------------------------------------------------------------------------*/
typedef struct _THREAD_PARAMS
{
	EVTASKFUNC	pFunc;
	void*		pArg;
	EvTaskHandle*	pTaskHandle;
}THREAD_PARAMS,*PTHREAD_PARAMS;

/**
 * native thread function
 */
DWORD WINAPI ThreadFunc(void* pParam)
{
	int				iResult;
	THREAD_PARAMS	ThreadParams;
	
	ThreadParams = *(THREAD_PARAMS*)pParam;
	
	/* deallocate the buffer */
	Evfree(pParam);

	/* call the user's thread function */
	iResult = ThreadParams.pFunc(ThreadParams.pArg);

	return iResult;
}

/*---------------------------------------------------------------------------*/

#if 0
int	EvCreateTask(EVTASKFUNC pEvTaskFunc, char* szTaskName, 
                 unsigned long lPriority, unsigned long lStackSize, 
                 void* pArg, EvTaskHandle* pTaskHandle)
#else
int EvCreateTask(EVTASKFUNC pEvTaskFunc, void *pArg,EvTaskHandle* pTaskHandle)
#endif
{
#if 0
	PTHREAD_PARAMS	pParams;
	/* pass a pointer to a heap (not to a stack), because if this thread dies 
    its stack will be also destroyed */
	pParams = Evmalloc(sizeof(THREAD_PARAMS));
	if(!pParams)
		return -1;

	pParams->pFunc = pEvTaskFunc;
	pParams->pArg = pArg;
	pParams->pTaskHandle = pTaskHandle;

	*pTaskHandle = (EvTaskHandle)CreateThread(NULL, 0, ThreadFunc, pParams, 0, NULL);
	return *pTaskHandle == 0?-1:0;
#else
	
    EvTaskHandle*  thr_handle = 0;
    int   ret_code = 0; 

    if (pEvTaskFunc == NULL) {
		return -1;
    }
	memset(&thr_handle, 0, sizeof(EvTaskHandle*));
    thr_handle = (HANDLE)_beginthread((void (__cdecl *)(void *))pEvTaskFunc, 0, pArg);
    if (-1 != (int)pTaskHandle) {
		ret_code = errno; 
        if (pTaskHandle != NULL)
            *pTaskHandle = thr_handle;

        return 0;
    }
    else {
        return -1;
    }
#endif
}

/*---------------------------------------------------------------------------*/

int EvCloseTaskHandle(EvTaskHandle hTask)
{
	CloseHandle((HANDLE)hTask);
	return 0;
}

/*---------------------------------------------------------------------------*/

int EvExitTask(EvTaskHandle hTask)
{
	/* currently not implemented */
	_endthread();
	return 0;
}

/*---------------------------------------------------------------------------*/
int EvSpawn(const char *cmdName, const char *const *argv)
{
	char arguments[1024];
	PROCESS_INFORMATION process_info;
	STARTUPINFO startup_info;


	Evmemset(&startup_info, 0, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);

	Evmemset(arguments, 0, sizeof(arguments));
	Evstrcat(&arguments[0], (char*)cmdName);
	Evstrcat(arguments, " ");

	while(*argv)
	{
		Evstrcat(&arguments[0], (char*)*argv);
		Evstrcat(arguments, " ");
		argv++;
	}


	if(CreateProcess(NULL /*cmdName*/, (LPWSTR)arguments, 
		  NULL,//LPSECURITY_ATTRIBUTES psaProcess, 
		  NULL,//LPSECURITY_ATTRIBUTES psaThread, 
		  FALSE,//BOOL fInheritHandles, 
		  CREATE_NO_WINDOW, 
		  NULL, 
		  NULL, 
		  &startup_info, 
		  &process_info //LPPROCESS_INFORMATION pProcInfo
		))

		return 0;

	else

	{

		//tmpSize = GetLastError();

		//printf("GetLastError = %d\n", GetLastError());

		return -1;

	}

}

/*---------------------------------------------------------------------------*/
unsigned long EvTlsAlloc()
{
	return TlsAlloc();
}

/*---------------------------------------------------------------------------*/
int EvTlsSetValue(unsigned long dwTlsIndex,void *lpTlsValue)
{
	return TlsSetValue(dwTlsIndex, (void *)lpTlsValue);
}

/*---------------------------------------------------------------------------*/
void * EvTlsGetValue(unsigned long dwTlsIndex)
{
	return TlsGetValue(dwTlsIndex);
}

/*---------------------------------------------------------------------------*/
unsigned long EvGetCurrentThreadId(void)
{
	return GetCurrentThreadId();
}