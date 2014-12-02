#ifndef __THINNING_H__
#define __THINNING_H__
#include "../common/def_os_selector_header.h"
#include "../dinocore/imgcore.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_thinning(IMAGE_D *image);

#ifdef __cplusplus
}
#endif

#endif
