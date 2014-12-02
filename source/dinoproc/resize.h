#ifndef __RESIZE_H__
#define __RESIZE_H__
#include "../common/def_os_selector_header.h"
#include "../dinocore/imgcore.h"

#define REGULATION_SIZE			50
#define VERTICAL_CENTER			1
#define HORIZONTAL_CENTER		2

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_resize_interpolate(IMAGE_D *image);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	IMAGE_D* dinocv_resize_crop(IMAGE_D *image, RECT_D *rect);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	IMAGE_D* dinocv_resize_regulation(IMAGE_D *image); // need to supplement or replaced with nearest, bilinear

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	IMAGE_D *dinocv_resize_nearest(IMAGE_D * image, int width, int height);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	IMAGE_D* dinocv_resize_bilinear(IMAGE_D * image, int width, int height);

#ifdef __cplusplus
}
#endif

#endif
