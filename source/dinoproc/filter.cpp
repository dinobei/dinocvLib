#include "../common/def_os_selector_export.h"
#include "filter.h"

void dinocv_filter_mean(IMAGE_D *image)
{
	register int i, j;
	int w = image->width;
	int h = image->height;

	IMAGE_D *cpy = dinocv_copy_image(image);
	uchar_d **ptr1 = image->source;
	uchar_d **ptr2 = cpy->source;

	int temp;
	for(i = 1 ; i < h-1 ; i++)
	{
		for(j = 1 ; j < w-1 ; j++)
		{
			temp = ptr2[i-1][j-1]	+	ptr2[i-1][j]	+	ptr2[i-1][j+1]	+
					ptr2[i][j-1]	+	ptr2[i][j]		+	ptr2[i][j+1]	+
					ptr2[i+1][j-1]	+	ptr2[i+1][j]	+	ptr2[i+1][j+1];

			ptr1[i][j]=(uchar_d)d_clp(temp/9. + 0.5);
		}
	}
	dinocv_release_image(cpy);
}

void dinocv_filter_weight_mean(IMAGE_D *image)
{
	register int i, j;
	int w = image->width;
	int h = image->height;

	IMAGE_D *cpy = dinocv_copy_image(image);
	uchar_d **ptr1 = image->source;
	uchar_d **ptr2 = cpy->source;

	int temp;
	for(i = 1 ; i < h-1 ; i++)
	{
		for(j = 1 ; j < w-1 ; j++)
		{
			temp = ptr2[i-1][j-1]		+	(ptr2[i-1][j]<<1)		+	ptr2[i-1][j+1]		+
					(ptr2[i][j-1]<<1)	+	(ptr2[i][j]<<2)		+	2*(ptr2[i][j+1]<<1)		+
					ptr2[i+1][j-1]		+	(ptr2[i+1][j]<<1)		+	ptr2[i+1][j+1];

			ptr1[i][j]=(uchar_d)d_clp((float)(temp>>4) + 0.5);
		}
	}
	dinocv_release_image(cpy);
}


void dinocv_filter_median(IMAGE_D *img)
{
	register int i, j;

	int w = img->width;
	int h = img->height;
	IMAGE_D *img_cpy;
	uchar_d **source = img->source;
	 
	
	//이미지 카피
	img_cpy = dinocv_copy_image(img);
	uchar_d **cpy = img_cpy->source;

	int m[9];

	for(i = 1 ; i < h-1 ; i++)
	{
		for(j = 0 ; j < w-1 ; j++)
		{
			m[0] = cpy[i-1][j-1];
			m[1] = cpy[i-1][j];
			m[2] = cpy[i-1][j+1];
			m[3] = cpy[i][j-1];
			m[4] = cpy[i][j];
			m[5] = cpy[i][j+1];
			m[6] = cpy[i+1][j-1];
			m[7] = cpy[i+1][j];
			m[8] = cpy[i+1][j+1];

			dinocv_sort_insert(m, 9);

			source[i][j]=m[4];
		}
	}
	
	dinocv_release_image(img_cpy);
}
