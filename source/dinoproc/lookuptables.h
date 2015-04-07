#ifndef __LOOKUPTABLES_H__
#define __LOOKUPTABLES_H__
#include "../common/def_os_selector_header.h"
#include "../dinocore/imgcore.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	IMAGE_D *dinocv_lut_proc(IMAGE_D *image);

#ifdef __cplusplus
}
#endif

#endif
