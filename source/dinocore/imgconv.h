#ifndef __IMGCONV_H__
#define __IMGCONV_H__
#include "../common/def_os_selector_header.h"
#include "imgcore.h"
#include <math.h>

#ifdef __cplusplus
extern "C"
{
#endif

//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// 8 to 24 //////////////////////////////////////
/*
 * [함수명]
 *		dinocv_trans_8to24()
 * [함수 설명]
 *		bpp가 8인 그레이 영상을 파라미터로 받아서 24비트로 확대한다.
 *		단순하게, 한 픽셀을 3픽셀씩 반복함으로써 24비트로 만드는 것이기 때문에 색상은 그레이와 같다.
 *		리턴되는 IMAGE_D 구조체는 메모리 할당을 하여 복사되어 만들어진 것이다.
 * [파라미터]
 *		image	: 8비트 그레이스케일 이미지 구조체
 *
 * [리턴 타입]
 *		IMAGE_D* 타입의 24비트 트루컬러 이미지 구조체.
 *
 * [사용 예시]
 *		IMAGE_D* trans_img = dinocv_trans_8to24(img);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	IMAGE_D* dinocv_conv_8to24(IMAGE_D *image);

/*
 * [함수명]
 *		dinocv_trans_8to24_cpy()
 * [함수 설명]
 *		bpp가 8인 그레이 영상을 파라미터로 받아서 24비트로 확대한다.
 *		단순하게, 한 픽셀을 3픽셀씩 반복함으로써 24비트로 만드는 것이기 때문에 색상은 그레이와 같다.
 *		내부적으로 메모리 할당을 하지 않고 픽셀 데이터의 값만 복사한다.
 * [파라미터]
 *		image	: 8비트 그레이스케일 이미지 구조체
 *
 * [리턴 타입]
 *		IMAGE_D* 타입의 24비트 트루컬러 이미지 구조체.
 *
 * [사용 예시]
 *		dinocv_trans_8to24_cpy(img, img2);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_conv_8to24_cpy(IMAGE_D *image, IMAGE_D *tciimg);



//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// 24 to 8 //////////////////////////////////////
/*
 * [함수명]
 *		dinocv_trans_24to8()
 * [함수 설명]
 *		bpp가 24인 트루컬러 영상을 파라미터로 받아서 8비트로 축소(그레이화)한다.
 *		리턴되는 IMAGE_D 구조체는 메모리 할당을 한 뒤 복사되어 만들어진 것이다.
 *
 *		            | gray scale formula |
 *		gray_pixel = (Red*306 + Green*601 + Blue*117) >> 10
 *
 * [파라미터]
 *		image	: 24비트 트루컬러 이미지 구조체
 *
 * [리턴 타입]
 *		IMAGE_D* 타입의 8비트 그레이스케일 이미지 구조체.
 *
 * [사용 예시]
 *		IMAGE_D* trans_img = dinocv_trans_24to8(img);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	IMAGE_D* dinocv_conv_24to8(IMAGE_D *image);

/*
 * [함수명]
 *		dinocv_trans_24to8_cpy()
 * [함수 설명]
 *		bpp가 24인 트루컬러 영상을 파라미터로 받아서 8비트로 축소(그레이화)한다.
 *		내부적으로 메모리 할당을 하지 않고 픽셀 데이터의 값만 복사한다.
 * [파라미터]
 *		image	: 24비트 트루컬러 이미지 구조체
 *
 * [리턴 타입]
 *		IMAGE_D* 타입의 8비트 그레이스케일 이미지 구조체.
 *
 * [사용 예시]
 *		dinocv_trans_24to8_cpy(img, img2);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_conv_24to8_cpy(IMAGE_D *image, IMAGE_D *gsiimg);


//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// Color Space ////////////////////////////////////
#define PI 3.14159265358979

typedef struct RGB_COLOR_D RGB_COLOR_D;
struct RGB_COLOR_D{
    unsigned char r, g, b;    /* Channel intensities between 0 and 255 */
};

typedef struct HSV_COLOR_D HSV_COLOR_D;
struct HSV_COLOR_D{
    unsigned char hue;        /* Hue degree between 0 and 255 */
    unsigned char sat;        /* Saturation between 0 (gray) and 255 */
    unsigned char val;        /* Value between 0 (black) and 255 */
};

typedef struct HSI_COLOR_D HSI_COLOR_D;
struct HSI_COLOR_D{
    unsigned char hue;			/* Hue degree between 0 and 255 */
    unsigned char sat;			/* Saturation between 0 (gray) and 255 */
    unsigned char inten;		/* intensity between 0 (black) and 255 */
};

typedef struct YUV_COLOR_D YUV_COLOR_D;
struct YUV_COLOR_D{
    unsigned char y;			/* luminance */
    unsigned char u;			/* chrominance */
    unsigned char v;		    /* chrominance */
};

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	HSV_COLOR_D dinocv_conv_rgb2hsv(RGB_COLOR_D rgb);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_conv_rgb2hsi(double r, double g, double b,
							double *h, double *s, double *i);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_conv_yuv2rgb(int y, int u, int v, int *r, int *g, int *b);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_conv_rgb2yuv_0_255(int r, int g, int b, int *y, int *u, int *v);


/* Example
int main(int argc, char* argv[]) {
    struct rgb_color rgb;
    struct hsv_color hsv;
    rgb.r = (unsigned char)atoi(argv[1]);
    rgb.g = (unsigned char)atoi(argv[2]);
    rgb.b = (unsigned char)atoi(argv[3]);
    hsv = rgb_to_hsv(rgb);
    printf("Hue: %d\nSaturation: %d\nValue: %d\n\n", hsv.hue, hsv.sat, hsv.val);
    return 0;
}
*/

#ifdef __cplusplus
}
#endif

#endif
