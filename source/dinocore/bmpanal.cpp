#include "../common/def_os_selector_export.h"
#include "bmpanal.h"

/*
 * 기능 : bitmap 파일을 읽어들이되 fh와 dib 구조체를 이용한다.
 * 주의 : bpp가 '8' 또는 '24'만 지원된다.
 */
bool_d _dinocv_bitmap_read(char* filename, FILEHEADER_D **fh, DIB_D **dib){
	FILE *infile;
	INFOHEADER_D *ih;
	int real_width;

	//file open
#ifdef _WIN32
	fopen_s(&infile, filename,"rb");
#else
	infile = fopen(filename,"rb");
#endif

	if(infile == NULL)
	{
		// file open error (not exist)
		return d_false;
	}

	//assign file header value
	*fh = (FILEHEADER_D *)malloc(sizeof(FILEHEADER_D));
	memset(*fh, 0, sizeof(FILEHEADER_D));
	fread(*fh, 2, 1, infile);
	fread(&(*fh)->bfSize, 12, 1, infile);

	//memory allocation of dib structure
	*dib = (DIB_D *)malloc(sizeof(DIB_D));
	memset(*dib, 0, sizeof(DIB_D));

	//memory allocation of infomation header
	(*dib)->ih = (INFOHEADER_D *)malloc(sizeof(INFOHEADER_D));
	memset((*dib)->ih, 0, sizeof(INFOHEADER_D));
	ih = (*dib)->ih;
	
	// assign info header value
	fread(ih, sizeof(INFOHEADER_D), 1, infile);

	// RGBQUAD
	if(ih->biBitCount == 0x08) //if Grayscale Image
		fseek(infile, 1024, SEEK_CUR);

	// check biSizeImage Validation
	if(!ih->biSizeImage)
	{
		real_width = ((ih->biWidth* (ih->biBitCount>>3) )+3)&~3;
		ih->biSizeImage = real_width * ih->biHeight;
	}

	// Save only pixel data at pixelData array
	(*dib)->pixelData = (uchar_d *)malloc(ih->biSizeImage);
	memset((*dib)->pixelData, 0, ih->biSizeImage);

	fread((*dib)->pixelData, ih->biSizeImage, 1, infile);

	// complete, file close
	fclose(infile);

	//dinocv_print_bitmap_info(*fh, *dib);

	return d_true;
}

void _dinocv_get_two_dimension_array(IMAGE_D *img, DIB_D *dib)
{
	register int i, k;
	const int width = dib->ih->biWidth;
	const int height = dib->ih->biHeight;
	const int depth = dib->ih->biBitCount>>3;
	const int real_width = ((width*depth)+3)&~3;
	const int blank = real_width - dib->ih->biWidth*depth;
	const int swidth = real_width - blank;
	uchar_d **retArr;

	retArr = img->source;

	k=0;
	for(i = height ; i-- ; )
	{
		memcpy(retArr[i], &dib->pixelData[k], swidth);
		k+=swidth+blank;
	}
}

/*
 * 기능 : dib에 할당된 메모리를 해제한다.
 * 
 */
void _dinocv_dib_free(DIB_D *dib)
{
	if(dib->pixelData)	free(dib->pixelData);
	if(dib->ih)			free(dib->ih);
	if(dib)				free(dib);
}

/*
 * 기능 : FILEHEADER 구조체의 메모리를 해제한다.
 *        현재는 단순히 free만 호출하고있으나 추후에 memmgr 지원을 통해 변경될 수 있음.
 */
void _dinocv_fh_free(FILEHEADER_D *fh)
{
	if(fh) free(fh);
}

/*
 * 기능 : BITMAP의 HEADER 정보를 출력한다.
 *
 */
void dinocv_print_bitmap_info(FILEHEADER_D *fh, DIB_D *dib)
{
	INFOHEADER_D *ih = dib->ih;

	printf(" # BITMAP FILEHEADER\n");
	printf("\tfh->bfType=%hx\n", fh->bfType);
	printf("\tfh->bfSize=%ld\n", fh->bfSize);
	printf("\tfh->bfReserved1=%d\n", fh->bfReserved1);
	printf("\tfh->bfReserved2=%d\n", fh->bfReserved2);
	printf("\tfh->bfOffBits=%ld\n", fh->bfOffBits);

	printf("\n # BITMAP INFOHEADER\n");
	printf("\tih->biSize=%ld\n", ih->biSize);
	printf("\tih->biWidth=%ld\n", ih->biWidth);
	printf("\tih->biHeight=%ld\n", ih->biHeight);
	printf("\tih->biPlanes=%d\n", ih->biPlanes);
	printf("\tih->biBitCount=%d\n", ih->biBitCount);
	printf("\tih->biCompression=%lu\n", ih->biCompression);
	printf("\tih->biSizeImage=%lu\n", ih->biSizeImage);
	printf("\tih->biXPelsPerMeter=%ld\n", ih->biXPelsPerMeter);
	printf("\tih->biYPelsPerMeter=%ld\n", ih->biYPelsPerMeter);
	printf("\tih->biClrUsed=%lu\n", ih->biClrUsed);
	printf("\tih->biClrImportant=%lu\n", ih->biClrImportant);
}

void _dinocv_make_bitmap_header(IMAGE_D *image, FILEHEADER_D *fh, INFOHEADER_D *ih)
{
	ih->biSize			= 40; // if not, extention DIB
	ih->biWidth			= image->width;
	ih->biHeight		= image->height;
	ih->biPlanes		= 1; // always '1'
	ih->biBitCount		= image->bpp;
	ih->biCompression	= 0; // always '1' (means no compression.)
	ih->biSizeImage		= real_width(image)*image->height;
	ih->biXPelsPerMeter	= 0;
	ih->biYPelsPerMeter	= 0;
	ih->biClrUsed		= 0;
	ih->biClrImportant	= 0;

	fh->bfType			= 0x4D42;
	fh->bfOffBits		= (image->bpp==8 ? (1024+54) : 54);
	fh->bfSize			= ih->biSizeImage + fh->bfOffBits;
	fh->bfReserved1		= 0;
	fh->bfReserved2		= 0;
}
