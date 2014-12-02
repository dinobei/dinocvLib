#ifndef __CANNYEDGE_H__
#define __CANNYEDGE_H__
#include "../common/def_os_selector_header.h"
#include "../dinocore/imgcore.h"
#include "../dinocore/imageio.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_canny_edge(IMAGE_D *image, int th_high, int th_low);

#ifdef __cplusplus
}
#endif

#endif
