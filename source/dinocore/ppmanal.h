#ifndef __PPMANAL_H__
#define __PPMANAL_H__
#include "../common/def_os_selector_header.h"
#include "imgcore.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	IMAGE_D *_dinocv_ppm_read(char *filename);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	IMAGE_D *_dinocv_pgm_read(char *filename);

#ifdef __cplusplus
}
#endif

#endif
