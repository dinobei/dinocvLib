#ifndef __YUVANAL_H__
#define __YUVANAL_H__
#include "../common/def_os_selector_header.h"
#include "imageio.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	IMAGE_D *_dinocv_yuv422_read(char *filename, SIZE_D *size);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void Yuv2Rgb(int Y, int U, int V, int *R, int *G, int *B);

#ifdef __cplusplus
}
#endif

#endif
