/**
 *  @file scm_TUNER.h
 *
 *  This header file is part of Evude
 *
 *  Version:
 */

/*
 *  Copyright (C) 2009-2010. Evude Pvt. Ltd
 *  All rights reserved.
 */
#ifndef _H_SCM_TUNER_
#define _H_SCM_TUNER_


#define BDA_TUNER 1

#if BDA_TUNER
#include "..\common\Tuner_if\Bda_tuner_if_lib\bda_tuner_if.h"
#define StopCallback()
#else
#include "..\common\Tuner_if\Dib_sdk_if\dib_sdk_if.h"
#endif

#else
#error "Multiple inclusions of scm_tuner.h"
#endif //_H_SCM_TUNER_