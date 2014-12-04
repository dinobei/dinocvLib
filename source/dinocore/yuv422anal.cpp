#include "../common/def_os_selector_export.h"
#include "yuv422anal.h"



IMAGE_D *_dinocv_yuv422_read(char *filename, SIZE_D *size)
{
	register unsigned int i, j;
	FILE *infile;
	uchar_d *yuv_src;
	IMAGE_D *img = dinocv_create_image(size, 24);
	uchar_d *rgb_src = (uchar_d *)d_get_1d_source(img->source);
	int y1, u, y2, v;
	int r, g, b;

	yuv_src = (uchar_d *)malloc(sizeof(uchar_d) * size->width * size->height * 2);
	memset(yuv_src, 0, sizeof(uchar_d) * size->width * size->height * 2);

	//file open
#ifdef _WIN32
	fopen_s(&infile, filename,"rb");
#else
	infile = fopen(filename,"rb");
#endif

	if(infile == NULL) return NULL;

#ifdef _WIN32
	fread_s(yuv_src, size->width*size->height*2, size->width*size->height*2, 1, infile);
#else
	fread(yuv_src, size->width*size->height*2, 1, infile);
#endif
	
	for(i = 0, j=0 ; i < size->height*size->width*2 ; i+=4, j+=6)
	{
		y1 = yuv_src[i];
		u = yuv_src[i+1];
		y2 = yuv_src[i+2];
		v = yuv_src[i+3];
		
		dinocv_conv_yuv2rgb(y1, u, v, &r, &g, &b);
		rgb_src[j] = (uchar_d)b;
		rgb_src[j+1] = (uchar_d)g;
		rgb_src[j+2] = (uchar_d)r;

		dinocv_conv_yuv2rgb(y1, u, v, &r, &g, &b);
		rgb_src[j+3] = (uchar_d)b;
		rgb_src[j+4] = (uchar_d)g;
		rgb_src[j+5] = (uchar_d)r;
	}

	free(yuv_src);
	fclose(infile);

	return img;
}
