#ifndef _H_SCM_CONFIG_
#define _H_SCM_CONFIG_

/* -------------------------------------------- Header File Inclusion */
//#include "..\common\Tuner_if\Dib_sdk_if\DibExtDefines.h"
#include "scm_debug.h"

/* -------------------------------------------- Defines */
#define DIB_BOARD eBOARD_9080
#define MONIT_UPDATE_TIME 3000

#define DEBUG_LEVEL SCM_NONE//SCM_ALL  //DIB_SDK

#define SCM_SDK    (1 << 1) 
#define SCM_EVENT  (1 << 2)
#define SCM_ENGINE (1 << 3)
#define SCM_EXP    (1 << 4)
#define SCM_STATE  (1 << 5)
#define SCM_TIMER  (1 << 6)
#define SCM_NONE   (1 << 7)

#define SCM_ALL (SCM_SDK | SCM_EVENT | SCM_ENGINE | SCM_STATE |SCM_EXP /*| SCM_TIMER*/)

char * getStateString(uint16_t stateIdx/*enum TV_States stateIdx*/);



#else
#error "Multiple inclusions of scm_config.h"
#endif //_H_SCM_CONFIG_
