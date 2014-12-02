#ifndef __FEATURE_H__
#define __FEATURE_H__
#include "../common/def_os_selector_header.h"
#include "../dinocore/imgcore.h"
#include <math.h>
#include <stdlib.h>

typedef struct{
	int idx;
	float *arr;
}FEATURE_D;


typedef struct{
	float g00, g10, g01, g20, g02, g11, g30, g03, g21, g12; // moment
	float c00, c20, c02, c11, c30, c03, c21, c12; // centroid moment
	float n20, n02, n11, n30, n03, n21, n12; // regulated centroid moment
	float cx, cy;
}MOMENT_D;

#ifdef __cplusplus
extern "C"
{
#endif

// Feature Data Proc
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_feature_data_alloc(FEATURE_D *feature, int size);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_feature_data_init(FEATURE_D *feature);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_feature_data_free(FEATURE_D *feature);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_feature_data_add(FEATURE_D *feature, float data);

// Create & Remove Moment
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	MOMENT_D* dinocv_feature_create_moment(IMAGE_D *image);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d dinocv_feature_remove_moment(MOMENT_D *moment);

// Get Feature
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	int dinocv_feature_get_area(MOMENT_D *moment);					// Area

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	POINT_D dinocv_feature_get_centroid(MOMENT_D *moment);				// Centroid

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	float dinocv_feature_get_row_divergence(MOMENT_D *moment);			// Row Divergence

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	float dinocv_feature_get_column_divergence(MOMENT_D *moment);		// Column Divergence

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	float dinocv_feature_get_mix_divergence(MOMENT_D *moment);			// Mix Divergence

// Add Feature
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d dinocv_feature_add_invariant_moment(MOMENT_D *moment, FEATURE_D *feature);					// Invariant Moment

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d dinocv_feature_add_projection(IMAGE_D *image, int xsplit, int ysplit, FEATURE_D *feature);	// Projection

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d dinocv_feature_add_profile(IMAGE_D *image, FEATURE_D *feature);		// Profile

#ifdef __cplusplus
}
#endif

#endif
