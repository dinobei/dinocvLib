#ifndef __CONVEXHULL_H__
#define __CONVEXHULL_H__
#include "../common/def_os_selector_header.h"
#include "../dinocore/imgcore.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct PTARRAY_D PTARRAY_D;
struct PTARRAY_D{
	POINT_D *ptarr;
	int cnt;
};

#ifdef __cplusplus
extern "C"
{
#endif

// Graham scan(convex hull)
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	PTARRAY_D* dinocv_graham_scan(PTARRAY_D *arr);



// Convex Hull by dinobei
/*
 * define SIGNED INT's MAX & MIN
 */
#define SINTMIN 0xF0000000
#define SINTMAX 0x7FFFFFFF

/*
 * Zone Index
 */
#define A_ZONE	0
#define B_ZONE	2

/*
 * define value
 */
#define MAXIMUM 0
#define MINIMUM 1
#define X_COOR	0
#define Y_COOR	1

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_init_dinohull(int width, int height, int nop);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	int dinocv_dinohull(int *xasc, int *yasc, int nop, POINT_D2 *chl, int width, int height);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_replacement_idxnval(int **riwi, int *coorArray, int len, int max);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_uninit_dinohull(int width, int height);

// Quick Hull
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void inithull(int *pt, int *x, int *y, int n, int *minx, int *maxx, int **upper, int **lower);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	int *delete_right(int *pt, int *x, int *y, int *num, int p1, int p2);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_quickhull(int *pt, int n, int *x, int *y);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void print_array(int *pt, int n, int *x, int *y);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void print_hull(const int n, int *x, int *y);

#ifdef __cplusplus
}
#endif

#endif


