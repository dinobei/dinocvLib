#ifndef __BMPANALYSIS_H__
#define __BMPANALYSIS_H__
#include "../common/def_os_selector_header.h"
#include "imgcore.h"

/* BITMAP FILEHEADER */
typedef struct{
	ushort_d bfType;
	ulong_d bfSize;
	ushort_d bfReserved1;
	ushort_d bfReserved2;
	ulong_d bfOffBits;
}FILEHEADER_D;

/* BITMAP INFOHEADER */
typedef struct{
	ulong_d biSize;
	long_d biWidth;
	long_d biHeight;
	ushort_d biPlanes;
	ushort_d biBitCount;
	ulong_d biCompression;
	ulong_d biSizeImage;
	long_d biXPelsPerMeter;
	long_d biYPelsPerMeter;
	ulong_d biClrUsed;
	ulong_d biClrImportant;
}INFOHEADER_D;

/* RGBQUAD TABLE */
typedef struct{
	uchar_d rbgBlue;
	uchar_d rbgGreen;
	uchar_d rgbRed;
	uchar_d rgbReserved;
}RGBTABLE_D;

/* DIB STRUCTURE */
typedef struct{
	INFOHEADER_D *ih;
	uchar_d *pixelData;
}DIB_D;

#define real_width(img) ((pixel_width(img) + 3) & ~3)		// bitmap의 가로 픽셀 길이를 4의 배수로 계산하는 선언문

#ifdef __cplusplus
extern "C"
{
#endif

//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// BITMAP ANALYSIS //////////////////////////////////
/*
 * [함수명]
 *		_dinocv_bitmap_read()
 * [함수 설명]
 *		bitmap 파일 이름을 입력받고
 *		같이 전달받은 fh와 dib 구조체에 메모리 동적할당하여 정보를 읽어들인다.
 *		즉, filename 파일을 열어서 fh와 dib 구조체를 초기화하는것.
 * [파라미터]
 *		file_name	: 읽어들일 파일 이름
 *		fh			: FILEHEADER_D *fh; 형태로 선언된 뒤 함수 호출시 &fh를 넘겨준다. 즉 포인터의 포인터를 넘겨준다.
 *		dib			: fh와 마찬가지로 포인터의 포인터를 넘겨준다.
 *
 * [리턴 타입]
 *		fh와 dib 구조체에 정보를 모두 저장하고나서 d_true를 리턴하며, 그렇지 못했을 경우에는 d_false를 리턴한다.
 * [사용 예시]
 *		FILEHEADER_D *fh;
 *		DIB_D *dib;
 *		_dinocv_bitmap_read("lenna.bmp", &fh, &dib);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d _dinocv_bitmap_read(char* filename, FILEHEADER_D **fh, DIB_D **dib);

/*
 * [함수명]
 *		_dinocv_get_two_dimension_array()
 * [함수 설명]
 *		dib 구조체의 pixel data 부분은 1차원 배열로 되어있는데,
 *		이를 2차원 배열로 변환한 pixel data를 메모리 동적할당하여 리턴한다.
 *
 * [파라미터]
 *		dib	: DIB_D 구조체를 입력받고 해당하는 크기의 2차원 uchar_d 배열을 리턴한다.
 *
 * [리턴 타입]
 *		void
 * [사용 예시]
 *		_dinocv_get_two_dimension_array(dib);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void _dinocv_get_two_dimension_array(IMAGE_D *img, DIB_D *dib);

/*
 * [함수명]
 *		_dinocv_dib_free()
 * [함수 설명]
 *		DIB_D 구조체에 할당된 메모리를 해지한다.
 *
 * [파라미터]
 *		dib	: 할당하고자하는 DIB_D 구조체 포인터
 *
 * [리턴 타입]
 *		void
 * [사용 예시]
 *		_dinocv_dib_free(dib);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void _dinocv_dib_free(DIB_D *dib);

/*
 * [함수명]
 *		_dinocv_fh_free()
 * [함수 설명]
 *		FILEHEADER_D 구조체에 할당된 메모리를 해지한다.
 *
 * [파라미터]
 *		fh	: 할당하고자하는 FILEHEADER_D 구조체 포인터
 *
 * [리턴 타입]
 *		void
 * [사용 예시]
 *		_dinocv_fh_free(dib);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void _dinocv_fh_free(FILEHEADER_D *fh);

/*
 * [함수명]
 *		dinocv_print_bitmap_info()
 * [함수 설명]
 *		FILEHEADER_D 구조체와 DIB_D 구조체를 입력받아 헤더 정보를 필드별로 출력한다.
 *
 * [파라미터]
 *		fh	: 헤더 정보가 입력되어 있는 FILEHEADER_D 구조체
 *		dib	: 헤더 정보가 입력되어 있는 DIB_D 구조체
 * [리턴 타입]
 *		void
 * [사용 예시]
 *		dinocv_print_bitmap_info(fh, dib);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void dinocv_print_bitmap_info(FILEHEADER_D *fh, DIB_D *dib);

/*
 * [함수명]
 *		_dinocv_make_bitmap_header()
 * [함수 설명]
 *		dinocv_save_bitmap() 함수를 호출할때,
 *		IMAGE_D 구조체의 정보로 bitmap 헤더를 만든다.
 *		
 * [파라미터]
 *		image	: 비트맵 헤더 정보를 만들고자 하는 IMAGE_D 구조체
 *		fh		: 파라미터로 전달받은 비트맵 파일헤더 정보를 저장할 구조체
 *		dib		: 파라미터로 전달받은 비트맵 인포헤더 정보를 저장할 구조체
 * [리턴 타입]
 *		void
 * [사용 예시]
 *		FILEHEADER_D fh;
 *		INFOHEADER_D ih;
 *		_dinocv_make_bitmap_header(img, &fh, &ih);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	void _dinocv_make_bitmap_header(IMAGE_D *image, FILEHEADER_D *fh, INFOHEADER_D *ih);

#ifdef __cplusplus
}
#endif

#endif
