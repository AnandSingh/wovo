/**
 *  @file scm_error.h
 *
 *  This Stack Header File contains Structure and Data Type
 */

/*
 *  Copyright (C) 2009-2010. Evude Software Pvt. Ltd.
 *  All rights reserved.
 */

#ifndef _H_SCM_ERROR_
#define _H_SCM_ERROR_

/*
 *  Theory:
 *    Every API under MobileTv Stack Protocol Suite returns API_RESULT,
 *    which is a 2 Byte Unsigned Short variable. The higher order byte
 *    signifies the Module from which the Error has been generated, and
 *    the lower order byte encodes the actual reason of Error.
 *
 *    Each module under MobileTv Stack will be given unique
 *    Error ID (the higher order byte). Also, for each module the Error
 *    Code (the lower order byte) signifies an unique error situation.
 *
 *    For MobileTv Stack Carrier (eg, DVBT, DVBH), Error IDs are
 *    assigned from the range 0x10 to 0x2F. For MobileTv Stack Carrier-Modules, the range
 *    is from 0x50 to 0x7F.
 *
 *    The definition of "API_SUCCESS" is 0x0000 - which is the 'Success'
 *    return value for an API returning API_RESULT. All other values for
 *    should be treated as Errors.
 *
 *    The definition of "API_FAILURE" is 0xFFFF - which stands for "Unknown
 *    Error Situation" - use of which should be avoided at all costs during
 *    the implementation of MobileTv Carrier/Carrier-Module modules.
 *
 *  Note:
 *    The applications using native/profile APIs should NOT do
 *    any check on the basis of API_FAILURE - rather, the correct way to
 *    detect an error situation is by doing a negative check on API_SUCCESS.
 *    For example,
 *        if ( mobiletv_init(x, y, z) == API_FAILURE ) ... Wrong !
 *        if ( mobiletv_init(x, y, z) != API_SUCCESS ) ... Correct !
 */

/* Definition of API_RESULT */
typedef uint16_t    SCM_RESULT;

/* Definitions of API_SUCCESS & API_FAILURE */
#define SCM_SUCCESS             0x0000
#define SCM_FAILURE             0xFFFF

#define ALREADY_INIT			0x0001
#define ALREADY_DEINIT			-0x0002



#else
#error "Multiple inclusions of scm_error.h"
#endif // _H_SCM_ERROR_ 



