#ifndef __CLASSIFIER_H__
#define __CLASSIFIER_H__

#ifdef _DEBUG
	#pragma comment(lib, "dinocvLib_v0.1/dinocvLib_v0.1d.lib")
#endif
	#pragma comment(lib, "dinocvLib_v0.1/dinocvLib_v0.1.lib")

#include "dinocvLib_v0.1/dinocvlib.h"
#include "well512.h"

#define SUB_WINDOW_WIDTH			24//24
#define SUB_WINDOW_HEIGHT			24//24
#define INITIAL_SCALE_FACTOR_INDEX	4 // (1.25)^INITIAL_SCALE_FACTOR_INDEX
#define SCALE_FACTOR				1.25
#define N_LUT_SF					25 // 영상 크기에 따라 자동으로 선택할 수 있도록
//#define DETECTOR_MODEL_NAME			"cascaded_detector_gamza_test.model"
#define DETECTOR_MODEL_NAME			"cascaded_detector_real4.model"

using namespace std;

typedef struct
{
	int _w;
	int _h;
}HAAR_SHAPE_D;

typedef struct
{
	int weight;
	int x, y;
	int w, h;
}HAAR_BLOCK_D;

typedef struct
{
	int n_hb;
	HAAR_BLOCK_D p_hb[3];
	HAAR_SHAPE_D hs;
}HAAR_LIKE_FEATURE_D;

typedef struct
{
	int n_hb;
	HAAR_BLOCK_D **p_hb;
	float *reversed_square_scale_factor;
	HAAR_SHAPE_D hs;
	int *area;
}FAST_HAAR_LIKE_FEATURE_D;

typedef struct
{
	FAST_HAAR_LIKE_FEATURE_D fhlf;
	int polarity;
	int threshold;
	float alpha;
}WEAK_CLASSIFIER_D;

typedef struct
{
	int n_wc;
	WEAK_CLASSIFIER_D *p_wc;
	float threshold;
}STRONG_CLASSIFIER_D;

typedef struct
{
	int n_sc;
	STRONG_CLASSIFIER_D *p_sc;

}CASCADED_DETECTOR_D;

typedef struct
{
	RECT_D *p_rt;
	int n_objects; // the number of objects
}DETECTION_RESULT;




bool load_cascaded_detector(const char *model_name,
							const int sub_window_width,
							const int sub_window_height,
							const int initial_scale_factor_index,
							const float scale_factor,
							const int n_lut_sf,
							const int detection_result_buffer_size,
							int width,
							int height);
void release_cascaded_detector();

void create_haar_like_feature(HAAR_LIKE_FEATURE_D &hlf,
							  HAAR_SHAPE_D &hs,
							  int x, int y,
							  int w, int h);

int get_block_sum(HAAR_BLOCK_D &hb, int **ii, int _x, int _y);

int weak_classifier(WEAK_CLASSIFIER_D &wc, int x, int y, int **ii, int sf_idx);

int strong_classifier(STRONG_CLASSIFIER_D &sc, int x, int y, int **ii, int sf_idx);

int cascaded_classifier(int x, int y, int **ii, int sf_idx);

DETECTION_RESULT *cascaded_classify(IMAGE_D *img);
char *cascaded_classify_for_ndk(unsigned char *src, int width, int height);

int **make_integral_image(IMAGE_D *img);


void merge_rect();








#endif
