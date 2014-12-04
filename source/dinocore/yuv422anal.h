#ifndef __YUVANAL_H__
#define __YUVANAL_H__
#include "../common/def_os_selector_header.h"
#include "imageio.h"
#include "imgconv.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	IMAGE_D *_dinocv_yuv422_read(char *filename, SIZE_D *size);

#ifdef __cplusplus
}
#endif

#endif
