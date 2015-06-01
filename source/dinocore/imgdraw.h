#ifndef __IMGDRAW_H__
#define __IMGDRAW_H__
#include "../common/def_os_selector_header.h"
#include "imgcore.h"

#define sgn(x) ((x<0)?-1:((x>0)?1:0)) /* macro to return the sign of a
                                         number */

#ifdef __cplusplus
extern "C"
{
#endif

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// Draw ////////////////////////////////////////
/*
 * [함수명]
 *		dinocv_draw_rect()
 * [함수 설명]
 *		
 *		단순하게, 한 픽셀을 3픽셀씩 반복함으로써 24비트로 만드는 것이기 때문에 색상은 그레이와 같다.
 *		리턴되는 IMAGE_D 구조체는 메모리 할당을 하여 복사되어 만들어진 것이다.
 *		파라미터의 l,t,r,b는 영상의 좌표를 넘어가면 안된다. 다만, 두께 지정으로 인해 영상을
 *		넘어간 부분은 이 함수에서 처리한다.
 * [파라미터]
 *		image	: 8비트 그레이스케일 이미지 구조체
 *
 * [리턴 타입]
 *		IMAGE_D* 타입의 24비트 트루컬러 이미지 구조체.
 *
 * [사용 예시]
 *		RECT_D rt = dinocv_set_rect(10,10,200,200);
 *		COLOR_D clr = dinocv_set_color(255, 0, 0);
 *		IMAGE_D* trans_img = dinocv_draw_rect(img, &rt, &clr, 3);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_draw_rect(IMAGE_D *image, RECT_D *rect, COLOR_D *color, int thick);


// Draw Line
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_draw_line(IMAGE_D *img, POINT_D2 p1, POINT_D2 p2, COLOR_D *clr);

// Draw Ellipse
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_draw_ellipse(IMAGE_D *img, int xcent, int ycent, int rx, int ry, COLOR_D *clr);

/*
 * [함수명]
 *		dinocv_draw_fill_polygon()
 * [함수 설명]
 *		
 *		주어진 점으로 유향선분을 구성하여 내부를 주어진 색상으로 칠한다.
 *		현재 8비트만 가능하다.
 *
 * [파라미터]
 *		img_polygon	: 8비트 그레이스케일 이미지 구조체
 *		pt_array : 다각형을 구성하는 
 *
 * [리턴 타입]
 *		IMAGE_D* 타입의 24비트 트루컬러 이미지 구조체.
 *
 * [사용 예시]
 *		RECT_D rt = dinocv_set_rect(10,10,200,200);
 *		COLOR_D clr = dinocv_set_color(255, 0, 0);
 *		IMAGE_D* trans_img = dinocv_draw_rect(img, &rt, &clr, 3);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_draw_fill_polygon(IMAGE_D *img_polygon, POINT_D2 *pt_array, uint_d num, COLOR_D *clr);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
void line_fast_gray(IMAGE_D *img, int x1, int y1, int x2, int y2, int color);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
void line_fast_truclr(IMAGE_D *img, int x1, int y1, int x2, int y2, COLOR_D *color);

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
void dinocv_draw_polygon(IMAGE_D *img, POINT_D2 *pt_array, int num, COLOR_D *clr);

#ifdef __cplusplus
}
#endif

#endif
