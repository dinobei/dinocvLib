#ifndef __BINARIZATION_H__
#define __BINARIZATION_H__
#include "../common/def_os_selector_header.h"
#include "../dinocore/imgcore.h"

#define BIN_SIMPLE(th) (th<<8)
enum BIN_TYPE{BIN_REPEAT=0, BIN_OTSU};

#ifdef __cplusplus
extern "C"
{
#endif

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// BINARIZATION ////////////////////////////////////
/*
 * [함수명]
 *		dinocv_binarization()
 * [함수 설명]
 *		영상을 이진화한다.
 *		binarization type(bintype)에 따라 Otsu, Iterative 등의 방법으로 임계값을 구한다.	
 * [파라미터]
 *		image		: 그레이스케일(bpp=8) 이미지
 *		bntype		: 이진화할 때 사용할 방법 지정 파라미터. BN_OTSU 혹은 BN_SIMPLE 등이 있다.
 * [리턴 타입]
 *		int 형의 임계값(Threshold)을 리턴한다.
 *
 * [사용 예시]
 *		int th = dinocv_binarization(img, BN_OTSU);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	int dinocv_binarization(IMAGE_D *image, int bntype);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void _bin_make_image_with_image(IMAGE_D *to_img, IMAGE_D *from_img, int th);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d dinocv_reverse_image(IMAGE_D *image);

#ifdef __cplusplus
}
#endif

#endif
