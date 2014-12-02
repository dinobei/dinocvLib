#ifndef __MORPHOLOGY_H__
#define __MORPHOLOGY_H__
#include "../common/def_os_selector_header.h"
#include "../dinocore/imgcore.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_morphology_gray_erosion(IMAGE_D *image);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_morphology_gray_dilation(IMAGE_D *image);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_morphology_gray_opening(IMAGE_D *image);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_morphology_gray_closing(IMAGE_D *image);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_morphology_bin_erosion(IMAGE_D *image);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_morphology_bin_dilation(IMAGE_D *image);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_morphology_bin_opening(IMAGE_D *image);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_morphology_bin_closing(IMAGE_D *image);

#ifdef __cplusplus
}
#endif

#endif
