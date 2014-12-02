#ifndef __DINOCVLIB_H__
#define __DINOCVLIB_H__
#include "common/def_os_selector_header.h"

// CORE
#include "dinocore/imgcore.h"
#include "dinocore/imageio.h"
#include "dinocore/imgconv.h"
#include "dinocore/imgdraw.h"
#include "dinocore/yuv422anal.h"
#include "dinocore/ppmanal.h"

// PROCESS
#include "dinoproc/binarization.h"
#include "dinocore/imgconv.h"
#include "dinoproc/labeling.h"
#include "dinoproc/morphology.h"
#include "dinoproc/cannyedge.h"
#include "dinoproc/thinning.h"
#include "dinoproc/resize.h"
#include "dinoproc/filter.h"
#include "dinoproc/convexhull.h"
#include "dinoproc/dp.h"
#include "dinoproc/feature.h"
#include "dinoproc/histogram.h"
#include "dinoproc/resize.h"

// DATA STRUCTURE
#include "dinods/ds_sort.h"
#include "dinods/linkedlist.h"
#include "dinods/queue.h"

// MEMORY MANAGER
#include "dinomm/memmgr.h"

// APPLICATION
//void dinocv_number_plate_of_car_detection(IMAGE_D *img, RECT_D *rt); // 매개 변수 아직 덜 작성

#endif