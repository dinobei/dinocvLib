#include "../common/def_os_selector_export.h"
#include "imgconv.h"


IMAGE_D* dinocv_conv_8to24(IMAGE_D *image)
{
	register int i, j, k;
	const int width = image->width;
	const int height = image->height;
	IMAGE_D *truclrimg;
	uchar_d **src, **dst;
	SIZE_D sz = dinocv_set_size(width, height);

	if(image->bpp == 24)
	{
		return NULL;// this image is truecolor.
	}
	truclrimg = dinocv_create_image(&sz, 24);
	src = image->source;
	dst = truclrimg->source;
	for(i = 0 ; i < height ; i++){
		k=0;
		for(j = 0 ; j < width ; j++){
			dst[i][k] = dst[i][k+1] = dst[i][k+2] = src[i][j];
			k+=3;
		}
	}
	return truclrimg;
}

void dinocv_conv_8to24_cpy(IMAGE_D *image, IMAGE_D *tciimg)
{
	register int i, j, k;
	const int width = image->width;
	const int height = image->height;
	uchar_d **src, **tci;

	if(image->bpp == 24)
	{
		return;// this image is truecolor.
	}

	src = image->source;
	tci = tciimg->source;
	for(i = 0 ; i < height ; i++){
		k=0;
		for(j = 0 ; j < width ; j++){
			tci[i][k] = tci[i][k+1] = tci[i][k+2] = src[i][j];
			k+=3;
		}
	}
}

IMAGE_D* dinocv_conv_24to8(IMAGE_D *image)
{
	register int i, j, k;
	const int width = image->width;
	const int height = image->height;
	const int swidth = pixel_width(image);
	IMAGE_D *gsiimg;
	uchar_d **gsi, **src;
	SIZE_D sz = dinocv_set_size(width, height);
	
	if(image->bpp != 24)
	{
		return NULL;// This image is not truecolor.
	}


	/* gray scale formula
	 * pixelData = (Red*306 + Green*601 + Blue*117) >> 10
	 */
	gsiimg = dinocv_create_image(&sz, 8);
	src = image->source;
	gsi = gsiimg->source;
	for(i = 0 ; i < height ; i++){
		k=0;
		for(j = 0 ; j < swidth ; j+=3){
			gsi[i][k++] = (src[i][j]*306 + src[i][j+1]*601 + src[i][j+2]*117) >> 10;
		}
	}
	
	return gsiimg;
}

void dinocv_conv_24to8_cpy(IMAGE_D *image, IMAGE_D *gsiimg)
{
	register int i, j, k;
	//const int width = image->width;
	const int height = image->height;
	const int swidth = pixel_width(image);
	uchar_d **gsi, **src;
	
	
	if(image->bpp != 24)
	{
		return; // This file is not truecolor.
	}

	src = image->source;
	gsi = gsiimg->source;
	for(i = 0 ; i < height ; i++){
		k=0;
		for(j = 0 ; j < swidth ; j+=3){
			gsi[i][k++] = (src[i][j]*306 + src[i][j+1]*601 + src[i][j+2]*117) >> 10;
		}
	}
}

HSV_COLOR_D dinocv_conv_rgb2hsv(RGB_COLOR_D rgb)
{
    HSV_COLOR_D hsv;
    unsigned char rgb_min, rgb_max;
    rgb_min = d_min3(rgb.r, rgb.g, rgb.b);
    rgb_max = d_max3(rgb.r, rgb.g, rgb.b);
    hsv.val = rgb_max;
    if (hsv.val == 0) {
        hsv.hue = hsv.sat = 0;
        return hsv;
    }
    hsv.sat = 255*(long_d)(rgb_max - rgb_min)/hsv.val;
    if (hsv.sat == 0) {
        hsv.hue = 0;
        return hsv;
    }
    /* Compute hue */
    if (rgb_max == rgb.r) {
        hsv.hue = 0 + 43*(rgb.g - rgb.b)/(rgb_max - rgb_min);
    } else if (rgb_max == rgb.g) {
        hsv.hue = 85 + 43*(rgb.b - rgb.r)/(rgb_max - rgb_min);
    } else /* rgb_max == rgb.b */ {
        hsv.hue = 171 + 43*(rgb.r - rgb.g)/(rgb_max - rgb_min);
    }
    return hsv;
}

void dinocv_conv_rgb2hsi(double r, double g, double b,
							double *h, double *s, double *i)
{
	double angle;

	*i = (r+g+b)/3.;
	if( (r==g) && (g==b) )
	{
		*s = 0;
		*h = 0;
	}
	else
	{
		*s = 1. - (d_min3(r,g,b)/(*i));

		angle = (float)((r-g)+(r-b))/(2.*sqrt((r-g)*(r-g) + (r-b)*(g-b)));
		*h = acos(angle) * (180.f/PI);

		if(b>g) *h = 360.-*h;
	}
	*h /= 360.;
}


void dinocv_conv_yuv2rgb(int y, int u, int v, int *r, int *g, int *b)
{
	const int Y_SECTION = (y-16)*76284;
	const int U_MINUS_128 = u-128;
	const int V_MINUS_128 = v-128;
	*b = ( Y_SECTION + 132252*U_MINUS_128 ) >> 16;
	*g = ( Y_SECTION -  53281*V_MINUS_128	-  25625*U_MINUS_128 ) >> 16;
	*r = ( Y_SECTION + 104595*V_MINUS_128 ) >> 16;

	*b = d_clp(*b);
	*g = d_clp(*g);
	*r = d_clp(*r);
}

void dinocv_conv_rgb2yuv(int r, int g, int b, int *y, int *u, int *v)
{
	*y = (int)(0.257*r + 0.504*g + 0.098*b + 16);
	*u = (int)(-0.148*r - 0.291*g + 0.439*b + 128);
	*v = (int)(0.439*r - 0.368*g - 0.007*b + 128);
}
