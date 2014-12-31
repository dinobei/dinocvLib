#ifndef __IMGCORE_H__
#define __IMGCORE_H__
#include "../common/def_os_selector_header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Alias
#define d_true					1
#define d_false					0
typedef unsigned long			ulong_d;
typedef int						bool_d;
typedef unsigned char			uchar_d;
typedef unsigned short			ushort_d;
typedef unsigned int			uint_d;
typedef long					long_d;

// Macro
#define d_max(x,y) ((x)>(y) ? x : y)
#define d_min(x,y) ((x)<(y) ? x : y)
#define d_min3(x,y,z) (d_min(d_min((x),(y)), (z)))
#define d_max3(x,y,z) (d_max(d_max((x),(y)), (z)))
#define d_limit_oo(a,x,y) ((a)>(x)?((a)<(y)?(d_true):(d_false)):(d_false))	// x<a<y ? d_true : d_false
#define d_limit_cc(a,x,y) ((a)>=(x)?((a)<=(y)?(d_true):(d_false)):(d_false))	// x<=a<=y ? d_true : d_false
#define d_limit_co(a,x,y) ((a)>=(x)?((a)<(y)?(d_true):(d_false)):(d_false))	// x<=a<y ? d_true : d_false
#define d_limit_oc(a,x,y) ((a)>(x)?((a)<=(y)?(d_true):(d_false)):(d_false))	// x<a<=y ? d_true : d_false
#define d_ceil(f) (float)(((f)-(int)(f)) ? ( ((f)<0.) ? ((int)(f)) : (((int)(f))+1) ) : ((int)(f)) )
#define d_floor(f) (float)(((f)-(int)(f)) ? ( ((f)<0.) ? ((int)(f-1)) : ((int)f) ) : ((int)(f)) )
#define d_abs(x) ((x) < 0 ? -(x) : (x))
#define d_round(x) (long((x)+((x)>0 ? .5:-.5)))
#define d_rounddown(x) (long(x))
#define d_roundup(x) (long((x)>0?(x)+1:(x)-1))
#define d_clp(x) (((x)>255) ? 255 : ( ((x)< 0) ? 0 : (x)))
#define d_clp_boundary(x, low, high) (((int)(x)>(int)(high)) ? (int)(high) : ( ((int)(x)< (int)(low)) ? (int)(low) : (int)(x)))
#define pixel_width(img) (img->width*(img->bpp>>3))		// real width pixel length in bitmap saving
#define d_get_1d_source(src_2dim)				((void *)(src_2dim[0]))

// limit 4byte
#define d_signed_int_min 0xF0000000	// signed int minimum
#define d_signed_int_max 0x7FFFFFFF	// signed int maximum
#define d_unsigned_int_min 0x00000000	// unsigned int minimum
#define d_unsigned_int_max 0xFFFFFFFF	// unsigned int maximum

// limit 8byte
#define d_signed_double_max 1.7976931348623158e+308 /* max value */
#define d_signed_double_min -SDBLMAX
#define d_unsigned_double_max 0xFFFFFFFFFFFFFF
#define d_unsigned_double_min 0x00000000000000

// Image model
enum IMGMDL_TYPE {IMGMDL_RGB=0, IMGMDL_YUV422, IMGMDL_YUV420, IMGMDL_YUV411};

// Var type
enum SZ_TYPE {SZ_INT=sizeof(int), SZ_BYTE=sizeof(uchar_d), SZ_DOUBLE=sizeof(double)};

/* SIZE STRUCTURE */
typedef struct{
	uint_d width;
	uint_d height;
}SIZE_D;

/* RECTANGLE STRUCTURE */
typedef struct{
	uint_d left;
	uint_d top;
	uint_d right;
	uint_d bottom;
}RECT_D;

typedef struct{
	uint_d x, y;
	uint_d width, height;
}RECT_D2;

/* COLOR REF STRUCTURE */
typedef struct{
	int b;
	int g;
	int r;
}COLOR_D;

/* POINT REF STRUCTURE */
typedef struct{
	int x;
	int y;
}POINT_D;

typedef int POINT_D2[2];

/* IMAGE STRUCTURE */
typedef struct{
	uchar_d **source;
	uint_d width;
	uint_d height;
	RECT_D *roi;	// if not roi mode, it must be NULL.
	ushort_d bpp;		// bit per pixel
	ushort_d channel;	// the number of elements of the color model
	ushort_d model;		// Color Model, Alway 'IMGMDL_RGB'
	short mgrid;		// if -1, it is ignored.
}IMAGE_D;

#ifdef __cplusplus
extern "C"
{
#endif

//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// SETTER ///////////////////////////////////////
/*
 * [함수명]
 *		dinocv_path_maker()
 * [함수 설명]
 *		정수형 변수 i를 파일 이름으로하는 파일 경로를 생성한다.
 * [파라미터]
 *		dst_str : destination string
 *				생성된 경로를 저장할 변수 지정
 *		i : integer variable
 *				반복적으로 지정할 정수형 변수
 *		pre_add_str : pre-add string
 *				파일 경로 지정
 *		post_add_str : post-add string
 *				파일의 확장자를 지정
 * [리턴 타입]
 *		void
 * [사용 예시]
 *		dinocv_path_maker(str, i+1, "F://test/", ".bmp");
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_path_maker(char *dst_str, int i, char *pre_add_str, char *post_add_str);

//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// SETTER ///////////////////////////////////////
/*
 * [함수명]
 *		dinocv_set_rect()
 * [함수 설명]
 *		RECT_D 구조체의 인자값을 각각 입력받고 할당(assign)하여 리턴한다.
 * [파라미터]
 *		l	: left
 *				ex) x
 *		t	: top
 *				ex) x
 *		r	: right
 *				ex) x
 *		b	: bottom
 *				ex) x
 * [리턴 타입]
 *		RECT_D 형태로 함수 내에서 생성한 지역변수를 리턴하므로 리턴과 동시에 값을 복사하여 사용한다.
 *		보통, 함수의 파라미터 인자로 넘겨줄때 주소값(&)으로 캐스팅하여 사용한다.
 * [사용 예시]
 *		RECT_D rt = dinocv_set_rect(0, 0, 320, 240);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	RECT_D dinocv_set_rect(const uint_d l, const uint_d t, const uint_d r, const uint_d b);

/*
 * [함수명]
 *		dinocv_set_size()
 * [함수 설명]
 *		SIZE_D 구조체의 인자값을 각각 입력받고 할당(assign)하여 리턴한다.
 * [파라미터]
 *		width	: SIZE_D 구조체의 width에 할당할 값
 *					ex) x
 *		height	: SIZE_D 구조체의 height에 할당할 값
 *					ex) x
 * [리턴 타입]
 *		SIZE_D 형태로 함수 내에서 생성한 지역변수를 리턴하므로 리턴과 동시에 값을 복사하여 사용한다.
 *		보통, 함수의 파라미터 인자로 넘겨줄때 주소값(&)으로 캐스팅하여 사용한다.
 * [사용 예시]
 *		dinocv_set_size(320, 240);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	SIZE_D dinocv_set_size(const uint_d width, const uint_d height);

/*
 * [함수명]
 *		dinocv_set_color()
 * [함수 설명]
 *		COLOR_D 구조체의 인자값을 각각 입력받고 할당(assign)하여 리턴한다.
 * [파라미터]
 *		r		: Red 색상 값
 *		g		: Green 색상 값
 *		b		: Blue 색상 값
 *
 * [리턴 타입]
 *		COLOR_D 형태로 함수 내에서 생성한 지역변수를 리턴하므로 리턴과 동시에 값을 복사하여 사용한다.
 *		보통, 함수의 파라미터 인자로 넘겨줄때 주소값(&)으로 캐스팅하여 사용한다.
 * [사용 예시]
 *		COLOR_D color = dinocv_set_color(255, 0, 0);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	COLOR_D dinocv_set_color(const int r, const int g, const int b);



//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// GETTER ///////////////////////////////////////
/*
 * [함수명]
 *		dinocv_get_rect()
 * [함수 설명]
 *		IMAGE_D 구조체에 설정된 RECT 정보를 RECT_D 구조체에 가져온다.
 * [파라미터]
 *		image		: 메모리 내에 생성되어있는 IMAGE_D 구조체의 포인터
 *						ex) x
 *		rt			: 호출할 함수의 문맥에서 지역변수로 RECT_D 를 선언하고, 그 포인터를 넘긴다.
 *						ex) x
 * [리턴 타입]
 *		void
 * [사용 예시]
 *		dinocv_get_rect(img, &rt);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	RECT_D dinocv_get_rect(IMAGE_D *image);

/*
 * [함수명]
 *		dinocv_get_width()
 * [함수 설명]
 *		영상의 가로 크기를 가져온다. ROI 필드가 NULL일 경우, image 구조체의 width와 같다.
 * [파라미터]
 *		image		: 메모리 내에 생성되어있는 IMAGE_D 구조체의 포인터
 *						ex) x
 * [리턴 타입]
 *		uint_d 타입의 width 정보가 리턴된다.
 * [사용 예시]
 *		dinocv_get_width(img);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	uint_d dinocv_get_width(IMAGE_D *image);

/*
 * [함수명]
 *		dinocv_get_height()
 * [함수 설명]
 *		영상의 세로 크기를 가져온다. ROI 필드가 NULL일 경우, image 구조체의 height와 같다.
 * [파라미터]
 *		image		: 메모리 내에 생성되어있는 IMAGE_D 구조체의 포인터
 *						ex) x
 * [리턴 타입]
 *		uint_d 타입의 height 정보가 리턴된다.
 * [사용 예시]
 *		dinocv_get_height(img);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	uint_d dinocv_get_height(IMAGE_D *image);



//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// IMAGE CREATE ////////////////////////////////////
/*
 * [함수명]
 *		_dinocv_malloc()
 * [함수 설명]
 *		파라미터를 참조하여 2차원의 메모리 공간을 할당하고 (void **) 그 주소값을 리턴한다.
 * [파라미터]
 *		sz_type	: 동적할당 하고자 하는 메모리 셀의 크기
 *					ex) sizeof(int), sizeof(uchar_d) ...
 *		width	: 동적할당 하고자 하는 영상의 가로 사이즈
 *					ex) 320
 *		height	: 동적할당 하고자 하는 영상의 세로 사이즈
 *					ex) 240
 *		bpp		: bit per pixel
 *					ex) Trucolor Bitmap -> 24, Grayscale Bitmap -> 8 ...
 * [리턴 타입]
 *		void **	: 이차원 포인터 형으로, sz_type으로 넘겨준 형태에 따라 캐스팅하여 전달받는다.
 * [사용 예시]
 *		int **src1 = (int **)_dinocv_malloc(sizeof(int), 320, 240, 8);
 *		uchar_d **src2 = (uchar_d **)_dinocv_malloc(sizeof(uchar_d), 320, 240, 24);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void** _dinocv_malloc(const enum SZ_TYPE sz_type, const uint_d width, const uint_d height, const ushort_d bpp);

/*
 * [함수명]
 *		_dinocv_new()
 * [함수 설명]
 *		파라미터를 참조하여 2차원의 메모리 공간을 할당하고 (void **),
 *		0으로 초기화도 같이 수행하여 그 주소값을 리턴한다.
 * [파라미터]
 *		sz_type	: 동적할당 하고자 하는 메모리 셀의 크기
 *					ex) sizeof(int), sizeof(uchar_d) ...
 *		width	: 동적할당 하고자 하는 영상의 가로 사이즈
 *					ex) 320
 *		height	: 동적할당 하고자 하는 영상의 세로 사이즈
 *					ex) 240
 *		bpp		: bit per pixel
 *					ex) Trucolor Bitmap -> 24, Grayscale Bitmap -> 8 ...
 * [리턴 타입]
 *		void **	: 이차원 포인터 형으로, sz_type으로 넘겨준 형태에 따라 캐스팅하여 전달받는다.
 *					ex) int **src1 = (int **)_dinocv_malloc(sizeof(int), 320, 240, 8);
 *						uchar_d **src2 = (uchar_d **)_dinocv_malloc(sizeof(uchar_d), 320, 240, 24);
 * [사용 예시]
 *		int **src1 = (int **)_dinocv_new(sizeof(int), 320, 240, 8);
 *		uchar_d **src2 = (uchar_d **)_dinocv_new(sizeof(uchar_d), 320, 240, 24);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void** _dinocv_new(const enum SZ_TYPE sz_type, const uint_d width, const uint_d height, ushort_d bpp);

/*
 * [함수명]
 *		dinocv_memset()
 * [함수 설명]
 *		IMAGE_D 구조체의 source를 모두 0으로 초기화한다.
 * [파라미터]
 *		image	: 메모리 내에 생성되어있는 IMAGE_D 구조체의 포인터
 *						ex) x
 * [리턴 타입]
 *		void
 * [사용 예시]
 *		dinocv_memset(img);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_memset(IMAGE_D *img);

/*
 * [함수명]
 *		_dinocv_create_image()
 * [함수 설명]
 *		파라미터를 참조하여 IMAGE_D * 형태의 이미지 구조체를 초기 값으로 할당하여 리턴한다.
 * [파라미터]
 *		size		: 동적할당 하고자 하는 영상의 사이즈
 *						ex) SIZE_D sz;
 *							sz.width	= 320;
 *							sz.height	= 240;
 *		bpp		: bit per pixel
 *					ex) Trucolor Bitmap -> 24, Grayscale Bitmap -> 8 ...
 * [리턴 타입]
 *		IMAGE_D *	: IMAGE_D 구조체의 포인터형으로 동적할당하여 리턴한다.
 * [사용 예시]
 *		IMAGE_D *img = dinocv_create_image(&dinocv_set_size(320, 240), 24);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	IMAGE_D* dinocv_create_image(SIZE_D *size, const ushort_d bpp);



//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// IMAGE INITIALIZE //////////////////////////////////
/*
 * [함수명]
 *		_dinocv_init_image()
 * [함수 설명]
 *		파라미터를 입력받고 image 의 값을 초기화한다.
 * [파라미터]
 *		image		: 초기화할 이미지 구조체
 *						ex) x
 *		size		: 초기화할 영상의 사이즈로 채워진 SIZE_D 구조체
 *						ex) SIZE_D sz;
 *							sz.width	= 320;
 *							sz.height	= 240;
 *		bpp		: bit per pixel
 *					ex) Trucolor Bitmap -> 24, Grayscale Bitmap -> 8 ...
 * [리턴 타입]
 *		void
 * [사용 예시]
 *		_dinocv_init_image(img, &dinocv_set_size(320, 240), 3);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void _dinocv_init_image(IMAGE_D *image, SIZE_D *size, const ushort_d bpp);


//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// IMAGE RELEASE ////////////////////////////////////
/*
 * [함수명]
 *		_dinocv_free()
 * [함수 설명]
 *		어떤 타입의 2차원 포인터든 상관없이 메모리를 해제한다.
 * [파라미터]
 *		free_source	: 메모리를 해제할 포인터
 *						ex) x
 *		ht			: 해제할 2차원 배열의 높이 (height)
 *						ex) 240
 * [리턴 타입]
 *		void
 * [사용 예시]
 *		_dinocv_free(img->source, img->height);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void _dinocv_free(void **free_source);


//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// IMAGE RELEASE ////////////////////////////////////
/*
 * [함수명]
 *		dinocv_release_image()
 * [함수 설명]
 *		전달받은 image 구조체의 모든 메모리를 해제한다.
 * [파라미터]
 *		image		: 메모리를 해제할 IMAGE_D 구조체
 *						ex) x
 * [리턴 타입]
 *		void
 * [사용 예시]
 *		dinocv_release_image(img);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_release_image(IMAGE_D *image);

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// DIMENSION CAHNGE //////////////////////////////////
/*
 * [함수명]
 *		dinocv_get_2dim()
 * [함수 설명]
 *		1차원 이미지 데이터를 2차원 이미지 데이터로 할당하여 리턴한다.
 *		영상처리에서 매 픽셀 참조를 위해 * 연산이 많이 들어가기 때문에, 이를 방지하기 위함
 *
 * [파라미터]
 *		image		: 2차원 바이트배열 포인터를 얻을 이미지 구조체
 *
 * [리턴 타입]
 *		uchar_d **
 *
 * [사용 예시]
 *		uchar_d **src = dinocv_get_2dim(img);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	uchar_d **dinocv_get_2dim(IMAGE_D *img);

//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////// IMAGE COPY /////////////////////////////////////
/*
 * [함수명]
 *		dinocv_copy_image()
 * [함수 설명]
 *		image를 하나 넘겨받아 헤더 정보와 픽셀 데이터 정보를 카피한다.
 *
 * [파라미터]
 *		image	: 원본 이미지 구조체
 *
 * [리턴 타입]
 *		IMAGE_D* 타입의 이미지 구조체. 넘겨준 파라미터 그대로 복사하여 리턴한다.
 *
 * [사용 예시]
 *		IMAGE_D* copy_img = dinocv_copy_image(img);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	IMAGE_D* dinocv_copy_image(IMAGE_D *image);

/*
 * [함수명]
 *		dinocv_copy_image_cpy()
 * [함수 설명]
 *		image를 두개 넘겨받아 헤더 정보와 픽셀 데이터 정보를 카피한다.
 *		따로 메모리를 할당하지 않고 dst 이미지에 값을 그대로 복사한다.
 *
 * [파라미터]
 *		dst : 목적 이미지 구조체
 *		src	: 원본 이미지 구조체
 *
 * [리턴 타입]
 *		void
 *
 * [사용 예시]
 *		dinocv_copy_image(img, copy_img);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_copy_image_cpy(IMAGE_D *dst, IMAGE_D *src);


//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// SET ROI //////////////////////////////////////
/*
 * [함수명]
 *		dinocv_set_roi()
 * [함수 설명]
 *		파라미터로 넘겨받은 image에 rect 정보로 roi를 설정한다.
 *		주의할점은, image에 roi 설정 여부에 관계 없이 roi를 갱신하되
 *		영상 내부의 좌표가 아닌 좌표를 입력받았다면 d_false를 리턴한다.
 *
 * [파라미터]
 *		image	: roi가 설정되었거나 설정되어있지 않은 image 구조체
 *		rect	: roi로 설정할 RECTANGLE 좌표
 *
 * [리턴 타입]
 *		bool_d 타입. roi 설정에 성공하면 d_true(1)를, 실패하면 d_false(0)를 리턴한다.
 *
 * [사용 예시]
 *		bool_d bl = dinocv_set_roi(img, &dinocv_set_rect(10,10,30,30));
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d dinocv_set_roi(IMAGE_D *image, RECT_D *rect);

/*
 * [함수명]
 *		dinocv_reset_roi()
 * [함수 설명]
 *		파라미터로 넘겨받은 image의 roi 정보를 삭제한다.
 *		roi가 설정되어있었다면 roi를 제거(free)하고 초기화(NULL)하고,
 *		설정되어있지 않았다면 그냥 리턴한다.
 *
 * [파라미터]
 *		image	: roi가 설정되었거나 설정되어있지 않은 image 구조체
 *
 * [리턴 타입]
 *		void
 *
 * [사용 예시]
 *		dinocv_reset_roi(img);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_reset_roi(IMAGE_D *image);

#ifdef __cplusplus
}
#endif

#endif
