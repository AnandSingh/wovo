
#ifndef _H_SCM_STATE_
#define _H_SCM_STATE_

/* -------------------------------------------- Header File Inclusion */
#include "scm_event_data.h"

/* -------------------------------------------- Global Definitions */

/*#define BEGIN_TV_TRANSITION_MAP \
    static const unsigned char TV_TRANSITIONS[] = {\*/
#define BEGIN_TV_TRANSITION_MAP \
    unsigned char TV_TRANSITIONS[] = {\

#define TV_TRANSITION_MAP_ENTRY(entry)\
    entry,
#define END_TV_TRANSITION_MAP(data) \
    0 };\
    TvExternalEvent(TV_TRANSITIONS[tvCurrentState], data);

/* -------------------------------------------- Structures/Data Types */

//struct TvStateStruct;
typedef void (*TvStateFunc)(TvData *);
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
#ifdef __cplusplus
extern "C" {
#endif

/*Public function defination*/
void Init_TvStateMachine(int maxStates);
//void Deint_TvStateMachine();

/*Protected function defination*/
void TvExternalEvent(unsigned char newState, void *pTvData);
void TvInternalEvent(unsigned char newState, TvData *pTvData);
const TvStateStruct* GetTvStateMap();

/*Private function defination*/
void TvStateEngine(void);

void STATE_debug();

#ifdef __cplusplus
};
#endif

#endif  //_H_SCM_STATE

