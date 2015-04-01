/*
	* Written by PBJ
	* date		: 2012.4.02
	* amend cnt : 0
	* e-mail	: dinobei89@gmail.com
*/
#ifndef __MEMMGR_H__
#define __MEMMGR_H__
#include "../common/def_os_selector_header.h"
#include "imgcore.h"
#include "linkedlist.h"
#include "queue.h"
#include <stdlib.h>

typedef int MGRHWND;

// MEMMGR CALLBACK FUNC TYPE DEFINITION
typedef void (*IFUNC)(void *);
typedef void (*DFUNC)(void *);
typedef void* (*CFUNC)();

typedef struct{
	QUEUE_D* qu;		/* heap memory queue */
	CFUNC cfunc;		/* create callbackfunc in this queue */
	IFUNC ifunc;		/* initialize callbackfunc in this queue */
	DFUNC dfunc;		/* delete callbackfunc in this queue */
	int const_cnt;		/* allocated number */
	int supplement_cnt;	/* if insufficient memory room is detected,
						 * manager refer this value.
						 */
}MEMMGR_D;


#ifdef __cplusplus
extern "C"
{
#endif

/*
 * create memory manager function
 * parameter 1 : create callback function.
 * parameter 2 : initialize callback function.
 * parameter 3 : memory free callback function.
 * parameter 4 : the number of allocated data at beginning time.
 * return type : integer handle. (you must save it.)
 *
 * note! 
 * care callback function's the number of parameters and return type
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	MGRHWND	create_memmgr(
	CFUNC cfunc,
	IFUNC ifunc,
	DFUNC dfunc,
	int const_cnt,
	int supplement_cnt);

/*
 * get heap memory function
 * parameter 1 : return data of create_memmgr().
 * return type : data that you define.
 * 
 * note!
 * if program frequently call, raise the number of init memory.
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void *get_memmgr(MGRHWND handle);

/*
 * push heap memory function
 * parameter 1 : omitted.
 * parameter 2 : address of heap memory that you have finished usage.
 * return type : address of heap memory. its size varies by the user.
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d put_memmgr(MGRHWND handle, void *mem_pt);

/*
 * delete heap memory function (all handle)
 * parameter 2 : if it's TRUE, manager is revoke all the assigned memory.
 *				 if it's FALSE, manager is revoke some remained memory in queue.
 * 
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d delete_all_memmgr(bool_d is_force_free/* current ignored */);

#ifdef __cplusplus
}
#endif

#endif
