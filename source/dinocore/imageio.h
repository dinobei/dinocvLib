#ifndef __IMAGEIO_H__
#define __IMAGEIO_H__
#include "../common/def_os_selector_header.h"
#include "imgcore.h"
#include "bmpanal.h"
#include "ppmanal.h"
#include "yuv422anal.h"

#ifdef __cplusplus
extern "C"
{
#endif

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// LOAD ////////////////////////////////////////
/*
 * [함수명]
 *		dinocv_load_image()
 * [함수 설명]
 *		파일 이름과 사이즈, bpp를 입력받아서( BMP 일떄는 size와 bpp 파라미터를 무시) image 구조체를 생성하여 리턴한다.
 *		파일의 확장자가 bmp일때와 raw일 때를 구분하여 각각에 맞게 처리한다.
 *		확장자를 분석하고 확장자에 맞는 loading 함수를 호출하여 IMAGE_D *형 이미지를 생성한다.
 *		현재 제공되는 확장자는 .raw파일과 .bmp파일이며 bpp는 '8' 또는 '24'만 지원한다.
 * [파라미터]
 *		file_name	: 읽어들일 파일 이름
 *		size		: 읽어들일 영상의 사이즈정보. 헤더가 없는 raw 파일일때만 유효하다.
 *		bpp			: bit per pixel. 헤더가 없는 raw 파일일때만 유효하다.
 *		imgmdl_type	: 색모델의 타입. IMGMDL_TYPE으로 선언되어있는 enum 타입의 변수이다.
 *
 * [리턴 타입]
 *		IMAGE_D * 타입의 구조체 주소를 리턴한다. 요청한 파일이 없거나 정보가 잘못되었을떄는 NULL을 리턴한다.
 * [사용 예시]
 *		dinocv_load_image("lenna.bmp", NULL, NULL, IMGMDL_RGB);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	IMAGE_D* dinocv_load_image(char *file_name, SIZE_D *size, int bpp, enum IMGMDL_TYPE imgmdl_type/* current ignored */);


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// SAVE ////////////////////////////////////////
/*
 * [함수명]
 *		dinocv_save_raw_byte()
 * [함수 설명]
 *		영상의 raw 데이터만 저장한다.
 *		roi 모드에 상관없이 영상의 전체가 저장된다.
 *		따라서 roi 모드가 설정되어있더라도 그 부분만 저장되는 것이 아닌 전체가 저장된다.
 * [파라미터]
 *		file_name	: 저장할 파일 이름
 *		image		: 저장할 이미지 구조체
 * [리턴 타입]
 *		bool_d 타입을 리턴하는데 제대로 저장했다면 d_true를 리턴하고 그렇지 않을 때는 d_false를 리턴한다.
 * [사용 예시]
 *		dinocv_save_raw_byte("test.raw", img);
 *
 */

#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d dinocv_save_raw_byte(char *file_name, IMAGE_D *image);

/*
 * [함수명]
 *		dinocv_save_bitmap()
 * [함수 설명]
 *		bmp 파일로 저장한다.
 * [파라미터]
 *		file_name	: 저장할 파일 이름
 *		image		: 저장할 이미지 구조체
 * [리턴 타입]
 *		bool_d 타입을 리턴하는데 제대로 저장했다면 d_true를 리턴하고 그렇지 않을 때는 d_false를 리턴한다.
 * [사용 예시]
 *		dinocv_save_bitmap("test.bmp", img);
 *
 */
#if defined(_WIN32)
extern "C" DINOBEI_DLLTYPE
#endif
	bool_d dinocv_save_bitmap(char *file_name, IMAGE_D *image);

#ifdef __cplusplus
}
#endif

#endif
