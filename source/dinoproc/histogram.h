#ifndef __HISTOGRAM_H__
#define __HISTOGRAM_H__
#include "../common/def_os_selector_header.h"
#include "../dinocore/imgcore.h"
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

// Used for image analysis
/*
 * [입력]	- num :		세분화 개수 설정
 *			- min, max :	분석할 컬러의 최소값과 최대값 설정
 *			- arr :		분석할 성분이 1차원 배열 형태로 저장되어있음
 *			- len :		arr의 길이
 * [출력]
 *			- int * : int *형 1차원 배열로서, num의 개수만큼 메모리 할당되어 리턴된다.
 * 
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	int* dinocv_get_histogram(int num, double min, double max, double *arr, int len);

#ifdef __cplusplus
}
#endif

#endif
