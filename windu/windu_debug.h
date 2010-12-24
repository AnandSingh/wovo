
#ifndef _H_SCM_DEBUG_H_
#define _H_SCM_DEBUG_H_

#include "osal_debug.h"


#define SCM_DEBUG(_MASK_, _MSG_) if( (DEBUG_LEVEL & _MASK_ ) == _MASK_) { osal_logFunction _MSG_ ; }
#define SCM_ASSERT( foo ) //{ if( ! (foo) ) { SCM_LogFunction ("\nASSERT FAILED in %s at line %d\n",__FILE__,__LINE__); } }


#else
#error "Multiple inclusions of scm_debug.h"
#endif //_H_SCM_DEBUG_H_
