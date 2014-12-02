#ifndef __CHAINCODE_H__
#define __CHAINCODE_H__
#include "../common/def_os_selector_header.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	int dinocv_get_cahincode(int oldx, int oldy, int x, int y);

#ifdef __cplusplus
}
#endif

#endif
