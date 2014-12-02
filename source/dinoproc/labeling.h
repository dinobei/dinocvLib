#ifndef __LABELING_H__
#define __LABELING_H__
#include "../common/def_os_selector_header.h"
#include "../dinocore/imgcore.h"

#include "../dinocore/imageio.h"//tmp

#define DIR_EXTERNAL	0
#define DIR_INTERNAL	1
#define MAX_DIRECTION	7 // since from 0 to 7

#define INITIAL_DIR(dir) ((dir) == DIR_EXTERNAL) ? 7 : 3
#define POSITION(dir)	(dir)%8

#define INITIAL_LABEL_CNT			50
#define EXTENSION_SIZE			10
typedef struct LABELINFO_SOURCE LABELINFO_D;
struct LABELINFO_SOURCE{
	int label_count;
	int **label_map;
	RECT_D *rect;
	int height;

	POINT_D2 **contours;
	POINT_D2 *contour;
	int *n_vertex;
	int *n_inner_contour;
	int predict_label_cnt;
};

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	int dinocv_fclabeling_tracer(unsigned char **image, POINT_D2 *p, const int initial_pos, int dir, int **label_map, const int C, const int OBJ);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_fclabeling_contour_tracing(unsigned char **image, const int x, const int y, int dir, LABELINFO_D *label_info, const int C, const int OBJ);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	LABELINFO_D *dinocv_fclabeling(IMAGE_D *image, int obj_val);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	LABELINFO_D *_dinocv_labelinfo_create(IMAGE_D *image);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_labelinfo_delete(LABELINFO_D *label_info);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void _dinocv_labelinfo_memory_extension(LABELINFO_D *label_info);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void _dinocv_labelinfo_trim(LABELINFO_D *label_info);

#ifdef __cplusplus
}
#endif

#endif
