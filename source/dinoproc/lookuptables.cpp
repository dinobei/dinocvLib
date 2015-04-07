#include "../common/def_os_selector_export.h"
#include "lookuptables.h"

IMAGE_D *dinocv_lut_proc(IMAGE_D *image)
{
	register int i, j;
	int sum;
	IMAGE_D *ret_image;
	uchar_d **source, **ret_source;
	int weight_vector[9] = {1, 8, 64,
		2, 16, 128,
		4, 32, 256};

	if(image == NULL || image->source == NULL || image->bpp != 8)
		return NULL;

	ret_image = dinocv_create_image_by_image(image);
	if(ret_image == NULL)
		return NULL;

	source = image->source;
	ret_source = ret_image->source;
	for(i = 1 ; i < image->height-1 ; i++)
	{
		for(j = 1 ; j < image->width-1 ; j++)
		{
			sum=0;
			sum += (source[i-1][j-1]?1:0) * weight_vector[0];
			sum += (source[i][j-1]?1:0) * weight_vector[3];
			sum += (source[i+1][j-1]?1:0) * weight_vector[6];
			sum += (source[i-1][j]?1:0) * weight_vector[1];
			sum += (source[i][j]?1:0) * weight_vector[4];
			sum += (source[i+1][j]?1:0) * weight_vector[7];
			sum += (source[i-1][j+1]?1:0) * weight_vector[2];
			sum += (source[i][j+1]?1:0) * weight_vector[5];
			sum += (source[i+1][j+1]?1:0) * weight_vector[8];
			if(sum != 511 && sum != 0)
				ret_source[i][j] = 255;
			
		}
	}

	return ret_image;
}
