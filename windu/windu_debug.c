
/* ----------------------------------------- Header File Inclusion */
#include "osal_types.h"
#include "osal_file.h"
#include "osal_memory.h"
#include "osal_string.h"
#include "osal_list.h"

#include "scm_event.h"
#include "scm_info.h"
#include "scm_exp.h"


char stateString[10];
char * getStateString(uint16_t stateIdx/*enum TV_States stateIdx*/)
{
	
	///stateString = (char*)Evmalloc(sizeof(char)*10);
	switch(stateIdx)
	{
	case TV_IDLE:
		Evstrcpy(stateString,"TV_IDLE");
		break;
	case TV_INIT:
		Evstrcpy(stateString,"TV_INIT");
		break;
	case TV_TUNE:
		Evstrcpy(stateString,"TV_TUNE");
		break;
	case TV_SCAN:
		Evstrcpy(stateString,"TV_SCAN");
		break;
	case TV_PLAY:
		Evstrcpy(stateString,"TV_PLAY");
		break;
	case TV_STOP:
		Evstrcpy(stateString,"TV_STOP");
		break;
	case TV_DEINIT:
		Evstrcpy(stateString,"TV_DEINIT");
		break;
	case TV_MONIT:
		Evstrcpy(stateString,"TV_MONIT");
		break;
	case TV_SCAN_COMPLETE:
		Evstrcpy(stateString,"TV_SCAN_COMPLETE");
		break;
	case TV_SDT_INFO:
		Evstrcpy(stateString,"TV_SDT_INFO");
		break;
	case TV_EPG:
		Evstrcpy(stateString,"TV_EPG");
		break;
	case TV_EPG_INFO:
		Evstrcpy(stateString,"TV_EPG_INFO");
		break;
	case TV_SUBTITLE:
		Evstrcpy(stateString,"TV_SUBTITLE");
		break;
	case TV_RECORD:
		Evstrcpy(stateString,"TV_RECORD");
		break;
	case TV_SCAN_CURRENT_FREQ:
		Evstrcpy(stateString,"TV_SCAN_CURRENT_FREQ");
		break;
	case TV_STOP_TUNE:
		Evstrcpy(stateString,"TV_STOP_SCAN");
		break;
	default:
		Evstrcpy(stateString,"Error Unknow State !!");	
	}
	return stateString;
}



