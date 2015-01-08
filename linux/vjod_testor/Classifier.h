#include "../../source/dinocvlib.h"
#include <cmath>
#include <time.h>
#include "well512.h"
#include "list.h"
using namespace std;

// Default General Parameters
#define SUB_WINDOW_WIDTH			24
#define SUB_WINDOW_HEIGHT			24
#define SCALE_FACTOR				1.25

// PSO Parameters
#define VELOCITY_MAX_X				50
#define VELOCITY_MAX_Y				50
#define INERTIA_COEFFICIENT			0.6
#define PARTICLE_COEFFICIENT		1.7
#define SWARM_COEFFICIENT			1.7

// Tracking-PSO Parameters
#define MAX_TRACKING_LENGTH			640>>3
#define CANDIDATE_MISS_MAX			30
#define TRACKING_MISS_MAX			10


// for Tracking
#define CANDIDATE_CNT_MAX			3
#define TRACKING_LENGTH_ARR_MAX		5
#define TRACKING_POS_ARR_MAX		3

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
	RECT_D *p_rt;
	int n_objects; // the number of objects
}DETECTION_RESULT;

typedef struct od_parameters od_parameters;
struct od_parameters{
	char model_file_name[255];
	int subwindow_width;
	int subwindow_height;
	int classification_methods;
	float scale_factor;
	int initial_scale_factor_index;
};

typedef struct CANDIDATE_OBJECT CANDIDATE_OBJECT;
struct CANDIDATE_OBJECT
{
	int detection_cnt;
	int x, y, w, h;
	int miss_cnt;
};

typedef struct
{
	// related PSO stages
	int _local_stage;
	double *currentScore;
	double *pbestScore;
	double gbestScore;

	POINT_D *pbestPosition;
	int gbestIndex;

	POINT_D *currentVelocity;
	POINT_D *currentPosition;

	int pos_max_x;
	int pos_max_y;

	int num_stage;
	int num_particles;

	// exit condition
	int numCorrectDetection;

}PSO_PARM_D;


typedef struct
{
	// classifier
	int n_sc;
	int n_sc_max;
	STRONG_CLASSIFIER_D *p_sc;

	// detection result
	DETECTION_RESULT *detection_result;
	DETECTION_RESULT *merged_detection_result;
	
	// LUTs
	unsigned int *lut_wsize_consider_sf;
	unsigned int *lut_hsize_consider_sf;
	int *lut_mv_sf;

	// default parameters
	int parm_sub_window_width;
	int parm_sub_window_height;
	int parm_initial_scale_factor_index;
	float parm_scale_factor;
	int parm_n_lut_sf;
	int parm_detection_result_buffer_size;

	// PSO parm
	PSO_PARM_D *pso_parm;

	// parm for drawing (color, thick)
	COLOR_D color;
	int thick;

	// draw mag
	float lr, tb;

}CASCADED_DETECTOR_D;

// get xy, wh
#define TO_GET_AVG_X(to)					((to)->x_sum/(to)->pos_division_cnt)
#define TO_GET_AVG_Y(to)					((to)->y_sum/(to)->pos_division_cnt)
#define TO_GET_CUR_X(to)					((to)->x_arr[(to)->pos_cidx])
#define TO_GET_CUR_Y(to)					((to)->y_arr[(to)->pos_cidx])

#define TO_GET_AVG_W(to)					((to)->w_sum/(to)->len_division_cnt)
#define TO_GET_AVG_H(to)					((to)->h_sum/(to)->len_division_cnt)
#define TO_GET_CUR_W(to)					((to)->w_arr[(to)->length_cidx])
#define TO_GET_CUR_H(to)					((to)->h_arr[(to)->length_cidx])
typedef struct TRACKING_OBJECT
{
	int length_cidx;
	int pos_cidx;
	int x_arr[TRACKING_POS_ARR_MAX];
	int y_arr[TRACKING_POS_ARR_MAX];
	int w_arr[TRACKING_LENGTH_ARR_MAX];
	int h_arr[TRACKING_LENGTH_ARR_MAX];
	int miss_cnt;
	int len_division_cnt;
	int pos_division_cnt;
	int x_sum, y_sum, w_sum, h_sum;
	int cur_x;
	int cur_y;
}TRACKING_OBJECT;


CASCADED_DETECTOR_D *load_cascaded_detector(const char *model_name,
							const int sub_window_width,
							const int sub_window_height,
							const int initial_scale_factor_index,
							const float scale_factor,
							const int n_lut_sf,
							const int detection_result_buffer_size,
							int width,
							int height);
void release_cascaded_detector(CASCADED_DETECTOR_D *cd);

void create_haar_like_feature(HAAR_LIKE_FEATURE_D &hlf,
							  HAAR_SHAPE_D &hs,
							  int x, int y,
							  int w, int h);

int get_block_sum(HAAR_BLOCK_D &hb, int **ii, int _x, int _y);

int weak_classifier(WEAK_CLASSIFIER_D &wc, int x, int y, int **ii, int sf_idx);

int strong_classifier(STRONG_CLASSIFIER_D &sc, int x, int y, int **ii, int sf_idx);

int cascaded_classifier(CASCADED_DETECTOR_D *cd, int x, int y, int **ii, int sf_idx);

void cascaded_classify(CASCADED_DETECTOR_D *cd, IMAGE_D *img, int **ii);
int **make_integral_image(IMAGE_D *img);

void merge_rect(CASCADED_DETECTOR_D *cd, int n_objects);

// PSO
void cascaded_classify_with_pso(CASCADED_DETECTOR_D *cd, RECT_D *rt, IMAGE_D *img, int **ii, int num_particles, int num_stage);
int cascaded_classifier_with_pso(CASCADED_DETECTOR_D *cd, int x, int y, int **ii, int sf_idx);
bool create_swarm(CASCADED_DETECTOR_D *cd, int pos_min_x, int pos_min_y, int pos_max_x, int pos_max_y, int num_particles, int num_stage, int sf_idx);
bool init_swarm(CASCADED_DETECTOR_D *cd, int pos_min_x, int pos_min_y, int pos_max_x, int pos_max_y, int num_particles, int num_stage, int sf_idx);
void release_swarm(PSO_PARM_D *pso_parm);


bool NextStage(CASCADED_DETECTOR_D *cd, int **ii, int sf_idx, DETECTION_RESULT *detection_result, int *n_objects, RECT_D *boundary);
int EvaluatePosition(CASCADED_DETECTOR_D *cd, int **ii, int sf_idx);
void CalculateVelocity(PSO_PARM_D *pso_parm);
void CalculatePosition(PSO_PARM_D *pso_parm);
void DrawParticles(CASCADED_DETECTOR_D *cd, IMAGE_D *img, int sf_idx);



void candidate_list_update(CASCADED_DETECTOR_D *cd, IMAGE_D *img, LIST_D *cl, LIST_D *tl, int **ii, int n_particles, int n_stages);
void tracking_list_update(CASCADED_DETECTOR_D *cd, IMAGE_D *img, LIST_D *cl, LIST_D *tl, int **ii, int n_particles, int n_stages);
void push_candidate(LIST_D *cl, RECT_D *rt);
void candidate_add(CASCADED_DETECTOR_D *cd, IMAGE_D *img, LIST_D *cl, LIST_D *tl, int **ii, int n_particles, int n_stages);
void cascaded_classify_with_tpso(IMAGE_D *img, CASCADED_DETECTOR_D *cd, LIST_D *cl, LIST_D *tl, int n_particles, int n_stages);
void cascaded_classify_with_swo(CASCADED_DETECTOR_D *cd, IMAGE_D *img);
