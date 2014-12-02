#include "../common/def_os_selector_export.h"
#include "imageio.h"

// INDEPENDANT INTERNAL METHODS
#define SUPPORT_EXT_NUM	5
char *gExt_d[SUPPORT_EXT_NUM]={"raw", "bmp", "ppm", "pgm", "yuv"};
#define EXT_RAW 0
#define EXT_BMP 1
#define EXT_PPM 2
#define EXT_PGM 3
#define EXT_YUV 4

/*
 * 기능 : 주어진 파일 이름의 확장자 인덱스를 얻는다.
 *        전역 변수인 gExt_d 배열의 인덱스로서 얻을 수 있다.
 *		지원하지않는 포맷이면 -1을 리턴한다.
 */
int _dinocv_get_ext(char *file_name){
	register unsigned int i;
	char *ext=NULL;
	int ext_idx=-1;
	for(i = 0 ; i < strlen(file_name) ; i++){
		while(file_name[i] == '.'){
			ext = file_name+i+1;
			break;
		}
	}

	for(i = 0 ; i < SUPPORT_EXT_NUM ; i++){
		if(!strcmp(ext, gExt_d[i])){
			ext_idx=i;
			break;
		}
	}

	return ext_idx;
}

/*
 * 기능 : 헤더 없는 이미지를 읽는다. sz(참조형 구조체)와
 *       bpp의 정보를 토대로 이미지를 읽어오되 fopen() 에러일때 NULL을 리턴한다.
 *
 */
IMAGE_D *_dinocv_read_no_header_image(char *file_name, SIZE_D *sz, int bpp){
	register int i;
	FILE *infile;
	uchar_d **malloc_source;
	const int width = sz->width;
	const int height = sz->height;
	IMAGE_D *image;

	/*file open*/
#ifdef _WIN32
	fopen_s(&infile, file_name, "rb");
#else
	infile = fopen(file_name,"rb");
#endif

	if(!infile)
		return NULL;//file open error

	// save only pixel data at malloc_source array
	image = dinocv_create_image(sz, bpp);
	malloc_source = image->source;
	for(i = 0 ; i < height ; i++){
		fread(malloc_source[i], width, 1, infile);
	}

	fclose(infile);
	return image;
}

IMAGE_D *dinocv_load_image(char *file_name, SIZE_D *size, int bpp, enum IMGMDL_TYPE imgmdl_type/* current ignored */){
	IMAGE_D *ret_buf;
	DIB_D *dib;
	FILEHEADER_D *fh;
	SIZE_D sz;

	switch(_dinocv_get_ext(file_name))
	{
	case EXT_RAW:
		if(!size) return NULL;
		ret_buf = _dinocv_read_no_header_image(file_name, size, bpp);
		break;
	case EXT_BMP:
		if(!_dinocv_bitmap_read(file_name, &fh, &dib))
			return NULL;
		sz = dinocv_set_size(dib->ih->biWidth, dib->ih->biHeight);
		ret_buf = dinocv_create_image(&sz, dib->ih->biBitCount);
		_dinocv_get_two_dimension_array(ret_buf, dib);
		
		_dinocv_fh_free(fh);
		_dinocv_dib_free(dib);
		break;
	case EXT_PPM:
		return _dinocv_ppm_read(file_name);
	case EXT_PGM:
		return _dinocv_pgm_read(file_name);
	case EXT_YUV:
		if(!size) return NULL;
		
		if(imgmdl_type == IMGMDL_YUV422)
			return _dinocv_yuv422_read(file_name, size);
	default:
		return NULL;
	}
	
	return ret_buf;
}


bool_d dinocv_save_raw_byte(char *file_name, IMAGE_D *image){
	register int i;
	FILE *fp;
	uchar_d **source = image->source;
	const int width = image->width * image->bpp>>3;
	const int height = image->height;

#ifdef _WIN32
	fopen_s(&fp, file_name, "wb");
#else
	fp = fopen(file_name, "wb");
#endif
	if(!fp)
		return d_false;

	for(i = 0 ; i < height ; i++)
	{
		fwrite(source[i], sizeof(uchar_d)*width, 1, fp);
	}
	fclose(fp);
	return d_true;
}

bool_d dinocv_save_bitmap(char *file_name, IMAGE_D *image){
	register int i;
	FILE *output;
	const int pxwidth = pixel_width(image);
	const int blank = real_width(image)-pxwidth;
	INFOHEADER_D ih;
	FILEHEADER_D fh;
	int *rgbquad;
	const uchar_d padding = 0;

#ifdef _WIN32
	fopen_s(&output, file_name, "wb");
#else
	output = fopen(file_name, "wb");
#endif

	if(output == NULL)
		return d_false;

	// Make Header
	_dinocv_make_bitmap_header(image, &fh, &ih);

	// File Header
	fwrite(&fh, sizeof(ushort_d), 1, output);
	fwrite(&fh.bfSize, sizeof(FILEHEADER_D)-sizeof(int), 1, output);

	// Info Header
	fwrite(&ih, sizeof(INFOHEADER_D), 1, output);
	
	// RGBQUAD
	if(ih.biBitCount == 0x08){
		rgbquad = (int *)malloc(sizeof(int)*256);
		for(i = 0x00 ; i <= 0xFF ; i++){
			rgbquad[i] = ( (i<<16) | (i<<8) | i);
		}
		fwrite(rgbquad, sizeof(int)*256, 1, output);
		free(rgbquad);
	}
	
	// Pixel Data
	for(i = image->height ; i-- ; ){
		fwrite(image->source[i], pxwidth, 1, output);
		if(blank)
			fwrite(&padding, sizeof(uchar_d)*blank, 1, output);
	}

	fclose(output);
	return d_true;
}
