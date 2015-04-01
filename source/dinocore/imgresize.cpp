#include "../common/def_os_selector_export.h"
#include "imgresize.h"

void dinocv_resize_interpolate(IMAGE_D *image)
{
	int i=0, j=0;
	int width, height;
	int pre_pixval = 255;
	uchar_d **source = image->source;

	width = dinocv_get_width(image);
	height = dinocv_get_height(image);

	// horizontal
	for(i=1; i < height-1; i++){
		for(j=1; j < width-1; j++){

			if(source[i][j] == 0 && pre_pixval != 0){
				if(source[i][j-1] == 255 && source[i][j+1] == 255)
					source[i][j] = pre_pixval;
			}

			pre_pixval = source[i][j];
		}

		pre_pixval = 255;
	}


	// vertical
	for(i=1; i < width-1; i++){
		for(j=1; j < height-1; j++){

			if(source[j][i] == 0 && pre_pixval != 0){
				if(source[j-1][i] == 255 && source[j+1][i] == 255)
					source[j][i] = pre_pixval;
			}

			pre_pixval = source[j][i];
		}

		pre_pixval = 255;
	}
}

IMAGE_D *dinocv_resize_crop(IMAGE_D *image, RECT_D *rect){
	register int i, j;
	int left, top, right, bottom;
	int width, height;
	uchar_d **source = image->source;
	uchar_d **crop_source;
	IMAGE_D *ret_img;
	SIZE_D sz;

	left = rect->left; 
	top = rect->top;
	right = rect->right;
	bottom = rect->bottom;
	width = right-left+1;
	height = bottom-top+1;

	if(rect->left < 0 ||
		rect->top < 0 ||
		rect->left > image->width-1 ||
		rect->top > image->height-1 ||
		rect->right > image->width ||
		rect->bottom > image->height ||
		rect->right < 0 ||
		rect->bottom < 0)
	{
		return NULL;
	}

	sz = dinocv_set_size(width,height);
	ret_img = dinocv_create_image(&sz, image->bpp);
	crop_source = ret_img->source;
	if (image->bpp == 24)
	{
		left *= 3;
		for(i = top ; i <= bottom ; i++){
			for(j = left ; j <= right*3 ; j+=3){
				crop_source[i-top][j-left] = source[i][j];
				crop_source[i-top][j-left+1] = source[i][j+1];
				crop_source[i-top][j-left+2] = source[i][j+2];
			}
		}
	}
	else 
	{
		for(i = top ; i <= bottom ; i++){
			for(j = left ; j <= right ; j++){
				crop_source[i-top][j-left] = source[i][j];
			}
		}
	}
	return ret_img;
}

IMAGE_D *dinocv_resize_regulation(IMAGE_D *image){
	register uint_d i, j;
	int new_calc_w, new_calc_h;
	int dif=0;
	double mag=0; // magnification
	int left_supplement=0;
	int orientation=0;
	int width, height;
	uchar_d **source = image->source;
	uchar_d **regulation_source;
	IMAGE_D *reg_img;
	SIZE_D sz = dinocv_set_size(REGULATION_SIZE, REGULATION_SIZE);

	// related resize
	int new_x, new_y;
	int hor_inc, ver_inc;

	width = dinocv_get_width(image);
	height = dinocv_get_height(image);
	RECT_D rt = dinocv_get_rect(image);

	
	reg_img = dinocv_create_image(&sz, 8);
	regulation_source=reg_img->source;

	if(width < height){
		mag = (double)(REGULATION_SIZE-2)/height;
		orientation = HORIZONTAL_CENTER;
		new_calc_h = REGULATION_SIZE-2;
		new_calc_w = (int)(width * mag);

		dif = (REGULATION_SIZE-2)-new_calc_w;
		left_supplement = dif>>1;

		hor_inc = left_supplement;
		ver_inc = 0;
	}
	else{
		mag = (double)(REGULATION_SIZE-2)/width;
		orientation = VERTICAL_CENTER;
		new_calc_h = (int)(height * mag);
		new_calc_w = REGULATION_SIZE-2;

		dif = (REGULATION_SIZE-2)-new_calc_h;
		left_supplement = dif>>1;

		hor_inc = 0;
		ver_inc = left_supplement;
	}

	// Resize
	for(i = rt.top; i < rt.bottom; i++){
		new_y = (int)d_ceil( (i-rt.top) * mag);
		
		new_y = (new_y >= (REGULATION_SIZE-2)) ? (REGULATION_SIZE-2)-1 : new_y;
		new_y = new_y+1+ver_inc;

		for(j = rt.left; j < rt.right; j++){
			new_x = (int)d_ceil( (j-rt.left) * mag);

			new_x = (new_x>=(REGULATION_SIZE-2)) ? (REGULATION_SIZE-2)-1 : new_x;
			
			new_x = new_x+1+hor_inc;
			

			if(regulation_source[new_y][new_x] != 255)
				regulation_source[new_y][new_x] = source[i][j];
		}
	}

	dinocv_resize_interpolate(reg_img);

	return reg_img;
}

IMAGE_D *dinocv_resize_nearest(IMAGE_D * image, int width, int height)
{
	register int i,j;
	int x,y;
	SIZE_D sz = dinocv_set_size(width, height);

	uchar_d **source = image->source;
	IMAGE_D *r_img = dinocv_create_image(&sz, image->bpp);
	uchar_d ** resize_image = r_img->source;
	
	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			x = (int)image->width*j/width;
			y = (int)image->height*i/height;

			resize_image[i][j] = source[y][x];
		}
	}

	return r_img;
}

IMAGE_D* dinocv_resize_bilinear(IMAGE_D * image, int width, int height)
{
	register int i,j;
	uint_d x1, y1, x2, y2;
	double rx, ry, p, q, temp;

	uchar_d **source = image->source;
	SIZE_D sz = dinocv_set_size(width, height);
	IMAGE_D *r_img = dinocv_create_image(&sz, image->bpp);
	uchar_d ** resize_image = r_img->source;

	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			rx = (double)image->width * j/width;
			ry = (double)image->height * i/height;

			x1 = (int)rx;
			y1 = (int)ry;

			x2 = x1 + 1;
			if(x2 == image->width)
				x2 = image->width - 1;
			y2 = y1 + 1;
			if(y2 == image->height)
				y2 = image->height - 1;

			p = rx - x1;
			q = ry - y1;

			temp = (1.0-p) * (1.0-q) * source[y1][x1] + p*(1.0-q)*source[y1][x2] + (1.0-p)*q*source[y2][x1] + p*q*source[y2][x2];

			resize_image[i][j] = (temp>255.) ? 0xFF : ((temp<0.) ? 0x00 : (uchar_d)temp);
		}
	}
	return r_img;
}

void ZoomOut(unsigned char *dest, unsigned char *src, int width, int height, int sx1, int sy1, int sx2, int sy2, int dx1, int dy1, int dx2, int dy2 )
{
	int  i,j;   // loop counter
	unsigned char r_upperleft,  g_upperleft,  b_upperleft;
	unsigned char r_upperright, g_upperright, b_upperright;
	unsigned char r_lowerleft,  g_lowerleft,  b_lowerleft;
	unsigned char r_lowerright, g_lowerright, b_lowerright;
	unsigned int srcoffset, srcoffset2, destoffset;
	int  r, g, b;
	int  ix, iy,   // 정수부
		px, py,   // 실수부 누적 = (1-t)에 해당됨.
		ppx, ppy,   // 1-px, 1-py, = t 에 해당됨.
		sw, dw,   // source width / height
		sh, dh;   // destination width / heigt
	int  pxpy,   // px * py
		ppxpy,   // ppx * py
		pxppy,   // px * ppy
		ppxppy;   // ppx * ppy
	int  dx, dy;   // differenctial

	sw = sx2-sx1;  // 소스는 1 픽셀씩 부족하게 할 것. 왜냐면.. 다음 픽셀값과 보간하다가 +1 에 의해 address overflow 발생 가능성이 있음.
	sh = sy2-sy1;
	dw = dx2-dx1+1;
	dh = dy2-dy1+1;
	dx = sw*256 /dw;
	dy = sh*256 /dh;
	iy = 0; // 정수부.
	py = 0; // 실수부 누적

	for (j=0; j<dh; j++)
	{
		py += dy;
		iy += (py>>8);
		py &= 0xFF;
		ppy = 256-py;     // (1-dy)  -  (1-알파)를 계산하기 위함.
		ix=0;
		px=0;
		destoffset = ((dy1+j)*width + dx1) * 3;
		srcoffset = ((sy1+iy)*height + sx1) * 3;
		for (i=0; i<dw*3; i+=3)
		{
			px += dx;
			ix += (px>>8);
			px &= 0xFF;
			ppx = 256-px;    // (1-dx)  -  (1-알파)를 계산하기 위함.
			pxpy = px*py;
			ppxpy = ppx*py;
			pxppy = px*ppy;
			ppxppy = ppx*ppy;
			srcoffset2 = ix*3 + srcoffset;
			r_upperleft = src[ srcoffset2 +2 ];
			g_upperleft = src[ srcoffset2 +1 ];
			b_upperleft = src[ srcoffset2 +0 ];
			r_upperright = src[ srcoffset2 +5 ];
			g_upperright = src[ srcoffset2 +4 ];
			b_upperright = src[ srcoffset2 +3 ];
			srcoffset2 += width*3 ;
			r_lowerleft = src[ srcoffset2 +2 ];
			g_lowerleft = src[ srcoffset2 +1 ];
			b_lowerleft = src[ srcoffset2 +0 ];
			r_lowerright = src[ srcoffset2 +5 ];
			g_lowerright = src[ srcoffset2 +4 ];
			b_lowerright = src[ srcoffset2 +3 ];
			r = ( (r_upperleft*ppxppy + r_upperright*pxppy ) + (r_lowerleft*ppxpy + r_lowerright*pxpy) ) >>16;
			g = ( (g_upperleft*ppxppy + g_upperright*pxppy ) + (g_lowerleft*ppxpy + g_lowerright*pxpy) ) >>16;
			b = ( (b_upperleft*ppxppy + b_upperright*pxppy ) + (b_lowerleft*ppxpy + b_lowerright*pxpy) ) >>16;
			//put_pixeloffset(dest, destoffset+i, r, g, b );  // i 는 3씩 증가한다. 픽셀당 3바이트 이기 때문.
			dest[destoffset+i+2] = r;
			dest[destoffset+i+1] = g;
			dest[destoffset+i  ] = b;
		}
	}
}
