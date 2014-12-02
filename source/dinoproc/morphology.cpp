#include "../common/def_os_selector_export.h"
#include "morphology.h"

void dinocv_morphology_gray_erosion(IMAGE_D *image){
	uchar_d **copy_source, **source;
	int i, j, m, n;
	int l, t, r, b;
	int pmin;
	int width, height;

	//copy_source = (uchar_d **)memmgr_get_image_source(memmgr_image);
	//memcpy(copy_source, source, 2000);
	source = image->source;
	width = dinocv_get_width(image);
	height = dinocv_get_height(image);
	RECT_D rt = dinocv_get_rect(image);
	l = rt.left;
	t = rt.top;
	r = rt.right;
	b = rt.bottom;


	copy_source = (uchar_d **)_dinocv_malloc(SZ_BYTE, width, height, image->bpp);

	for(i = t ; i < b ; i++){
		memcpy(copy_source[i-t], source[i]+l, sizeof(uchar_d)*width);
	}
	
	for(i = t+1 ; i < b-1 ; i++){
		for(j = l+1 ; j < r-1 ; j++){
			pmin=255;

			for(m = -1 ; m <= 1 ; m++){
				for(n = -1 ; n <= 1 ; n++){
					pmin = copy_source[i+m-t][j+n-l] < pmin ? copy_source[i+m-t][j+n-l] : pmin;
				}
			}
			source[i][j] = pmin;
		}
	}
	//memmgr_put_image_source(memmgr_image, copy_source);
	_dinocv_free((void **)copy_source);

}

void dinocv_morphology_gray_dilation(IMAGE_D *image){
	uchar_d **copy_source, **source;
	int i, j, m, n;
	int l, t, r, b;
	int pmax;
	int width, height;

	//copy_source = (uchar_d **)memmgr_get_image_source(memmgr_image);
	//memcpy(copy_source, source, 2000);
	source = image->source;
	width = dinocv_get_width(image);
	height = dinocv_get_height(image);
	RECT_D rt = dinocv_get_rect(image);
	l = rt.left;
	t = rt.top;
	r = rt.right;
	b = rt.bottom;

	copy_source = (uchar_d **)_dinocv_malloc(SZ_BYTE, width, height, image->bpp);
	

	
	for(i = t ; i < b ; i++){
		memcpy(copy_source[i-t], source[i]+l, sizeof(uchar_d)*width);
	}

	for(i = t+1 ; i < b-1 ; i++){
		for(j = l+1 ; j < r-1 ; j++){
			pmax=0;

			for(m = -1 ; m <= 1 ; m++){
				for(n = -1 ; n <= 1 ; n++){
					pmax = copy_source[i+m-t][j+n-l] > pmax ? copy_source[i+m-t][j+n-l] : pmax;
				}
			}
			source[i][j] = pmax;
		}
	}
	//memmgr_put_image_source(memmgr_image, copy_source);
	_dinocv_free((void **)copy_source);
}

void dinocv_morphology_gray_opening(IMAGE_D *image){
	dinocv_morphology_gray_erosion(image);
	dinocv_morphology_gray_dilation(image);
}

void dinocv_morphology_gray_closing(IMAGE_D *image){
	dinocv_morphology_gray_dilation(image);
	dinocv_morphology_gray_erosion(image);
}

void dinocv_morphology_bin_erosion(IMAGE_D *image){
	uchar_d **copy_source, **source;
	int i, j;
	int l, t, r, b;
	int width, height;

	source = image->source;
	width = dinocv_get_width(image);
	height = dinocv_get_height(image);
	RECT_D rt = dinocv_get_rect(image);
	l = rt.left;
	t = rt.top;
	r = rt.right;
	b = rt.bottom;


	copy_source = (uchar_d **)_dinocv_malloc(SZ_BYTE, width, height, image->bpp);

	for(i = t ; i < b ; i++)
	{
		memcpy(copy_source[i-t], source[i]+l, sizeof(uchar_d)*width);
	}
	
	for(i = t+1 ; i < b-1 ; i++){
		for(j = l+1 ; j < r-1 ; j++){
			if(copy_source[i][j] != 0){
				if(copy_source[i-1][j-1] == 0 ||
					copy_source[i-1][j] == 0 ||
					copy_source[i-1][j+1] == 0 ||
					copy_source[i][j-1] == 0 ||
					copy_source[i][j+1] == 0 ||
					copy_source[i+1][j-1] == 0 ||
					copy_source[i+1][j] == 0 ||
					copy_source[i+1][j+1] == 0)
				{
					source[i][j]=0;
				}
			}
		}
	}
	_dinocv_free((void **)copy_source);
}

void dinocv_morphology_bin_dilation(IMAGE_D *image){
	uchar_d **copy_source, **source;
	int i, j;
	int l, t, r, b;
	int width, height;

	source = image->source;
	width = dinocv_get_width(image);
	height = dinocv_get_height(image);
	RECT_D rt = dinocv_get_rect(image);
	l = rt.left;
	t = rt.top;
	r = rt.right;
	b = rt.bottom;


	copy_source = (uchar_d **)_dinocv_malloc(SZ_BYTE, width, height, image->bpp);

	for(i = t ; i < b ; i++)
	{
		memcpy(copy_source[i-t], source[i]+l, sizeof(uchar_d)*width);
	}
	
	for(i = t+1 ; i < b-1 ; i++){
		for(j = l+1 ; j < r-1 ; j++){
			if(copy_source[i][j] == 0){
				if(copy_source[i-1][j-1] != 0 ||
					copy_source[i-1][j] != 0 ||
					copy_source[i-1][j+1] != 0 ||
					copy_source[i][j-1] != 0 ||
					copy_source[i][j+1] != 0 ||
					copy_source[i+1][j-1] != 0 ||
					copy_source[i+1][j] != 0 ||
					copy_source[i+1][j+1] != 0)
				{
					source[i][j]=255;
				}
			}
		}
	}
	_dinocv_free((void **)copy_source);
}

void dinocv_morphology_bin_opening(IMAGE_D *image){
	dinocv_morphology_bin_erosion(image);
	dinocv_morphology_bin_dilation(image);
}

void dinocv_morphology_bin_closing(IMAGE_D *image){
	dinocv_morphology_bin_dilation(image);
	dinocv_morphology_bin_erosion(image);
}
