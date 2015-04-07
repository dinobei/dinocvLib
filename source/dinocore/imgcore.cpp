#include "../common/def_os_selector_export.h"
#include "imgcore.h"

void dinocv_path_maker(char *dst_str, int i, char *pre_add_str, char *post_add_str)
{
#ifdef _WIN32
	sprintf_s(dst_str, _MAX_PATH, "%s%d%s", pre_add_str, i, post_add_str);
#else
	sprintf(dst_str, "%s%d%s", pre_add_str, i, post_add_str);
#endif
}

RECT_D dinocv_set_rect(const uint_d l, const uint_d t, const uint_d r, const uint_d b)
{
	RECT_D r_rt;
	r_rt.left		= l;
	r_rt.top		= t;
	r_rt.right		= r;
	r_rt.bottom		= b;
	return r_rt;
}

SIZE_D dinocv_set_size(const uint_d width, const uint_d height)
{
	SIZE_D r_sz;
	r_sz.width=width;
	r_sz.height=height;
	return r_sz;
}

COLOR_D dinocv_set_color(const int r, const int g, const int b)
{
	COLOR_D r_clr;
	r_clr.r=r;
	r_clr.g=g;
	r_clr.b=b;
	return r_clr;
}

RECT_D dinocv_get_rect(IMAGE_D *image)
{
	RECT_D r_rt;
	if(image->roi)
		return *(image->roi);

	r_rt.left=0;
	r_rt.top=0;
	r_rt.right = image->width;
	r_rt.bottom = image->height;
	return r_rt;
}

uint_d dinocv_get_width(IMAGE_D *image)
{
	if(image->roi)
		return image->roi->right-image->roi->left;
	return image->width;
}

uint_d dinocv_get_height(IMAGE_D *image)
{
	if(image->roi)
		return image->roi->bottom-image->roi->top;
	return image->height;
}

SIZE_D dinocv_get_size(IMAGE_D *image)
{
	SIZE_D sz;
	sz.width = dinocv_get_width(image);
	sz.height = dinocv_get_height(image);

	return sz;
}

void** _dinocv_malloc(const enum SZ_TYPE sz_type, const uint_d width, const uint_d height, const ushort_d bpp)
{
	register unsigned int i, adt=0;
	void **ret_buf;
	void *source;
	int width_size;

	width_size = sz_type * width * (bpp>>3);
	ret_buf = (void **)malloc(sizeof(void *) * height);
	source = (void *)malloc(width_size * height);
	
	for(i = 0 ;i < height ; i++)
	{
		ret_buf[i]=(uchar_d *)source+adt;
		adt+=width_size;
	}

	return ret_buf;
}

void** _dinocv_new(const enum SZ_TYPE sz_type, const uint_d width, const uint_d height, ushort_d bpp)
{
	register unsigned int i;
	int adt=0;
	void **ret_buf;
	void *source;
	int width_size;

	width_size = sz_type * width * (bpp>>3);
	ret_buf = (void **)malloc(sizeof(void *) * height);
	source = (void *)malloc(width_size * height);
	memset(source, 0, width_size * height);
	
	for(i = 0 ;i < height ; i++)
	{
		ret_buf[i]=(uchar_d *)source+adt;
		adt+=width_size;
	}

	return ret_buf;
}

void dinocv_memset(IMAGE_D *img)
{
	memset(img->source[0], 0, img->height * img->width * (img->bpp>>3));
}

IMAGE_D* dinocv_create_image(SIZE_D *size, const ushort_d bpp)
{
	IMAGE_D *ret_buf;

	// error check
	if(	size->width <= 0 ||
		size->height <= 0)
	{
		return NULL;
	}

	ret_buf = (IMAGE_D *)malloc(sizeof(IMAGE_D));

	// initialize image
	_dinocv_init_image(ret_buf, size, bpp);

	// allocation source memory
	ret_buf->source = (uchar_d **)_dinocv_new(SZ_BYTE, size->width, size->height, bpp);
	
	return ret_buf;
}

IMAGE_D* dinocv_create_image_by_image(IMAGE_D *org_img)
{
	IMAGE_D *ret_buf;
	SIZE_D size;

	if(org_img == NULL)
	{
		return NULL;
	}
	size = dinocv_get_size(org_img);
	ret_buf = (IMAGE_D *)malloc(sizeof(IMAGE_D));

	// initialize image
	_dinocv_init_image(ret_buf, &size, org_img->bpp);

	// allocation source memory
	ret_buf->source = (uchar_d **)_dinocv_new(SZ_BYTE, size.width, size.height, org_img->bpp);

	return ret_buf;
}

void _dinocv_init_image(IMAGE_D *image, SIZE_D *size, const ushort_d bpp)
{
	image->width = size->width;
	image->height = size->height;
	image->roi=NULL;
	image->mgrid = -1;
	image->bpp = bpp;
	image->channel = 3; // 일단은 bitmap만 지원하므로 무조건 3을 할당.
	image->model = IMGMDL_RGB;
}

void _dinocv_free(void **free_source)
{
	if(free_source[0])
	{
		free(free_source[0]);
		free_source[0] = NULL;
	}
	if(free_source)
	{
		free(free_source);
		free_source = NULL;
	}
}

void dinocv_release_image(IMAGE_D *image)
{
	uchar_d **source=NULL;
	RECT_D *rroi=NULL;
	
	if(image)
	{
		source = image->source;
		rroi = image->roi;
	}

	if(rroi)
	{	
		free(rroi);
		rroi=NULL;
	}
	if(source)
	{
		free(source[0]);
		free(source);
		source=NULL;
	}
	if(image)
	{
		free(image);
		image=NULL;
	}
}

IMAGE_D* dinocv_copy_image(IMAGE_D *image)
{
	const int width = image->width;
	const int height = image->height;
	const int depth = image->bpp>>3;
	int sz_cpy;
	IMAGE_D *ret_img;
	uchar_d **ret_source, **source = image->source;
	RECT_D *roi = image->roi;
	SIZE_D sz = dinocv_set_size(width, height);

	// copy equal size image
	ret_img = dinocv_create_image(&sz, image->bpp);
	// copy equal mgrid
	ret_img->mgrid = image->mgrid;

	// copy equal roi
	if(roi)
	{
		ret_img->roi = (RECT_D *)malloc(sizeof(RECT_D));
		memcpy(ret_img->roi, roi, sizeof(RECT_D));
	}
	else
	{
		ret_img->roi=NULL;
	}
	
	// copy equal source
	ret_source = ret_img->source;
	sz_cpy = sizeof(uchar_d)*width*height*depth;
	memcpy(ret_source[0], source[0], sz_cpy);
	return ret_img;
}

void dinocv_copy_image_cpy(IMAGE_D *dst, IMAGE_D *src)
{
	const int width = src->width;
	const int height = src->height;
	const int depth = src->bpp>>3;
	int sz_cpy;
	
	uchar_d **ret_source, **source = src->source;
	RECT_D *roi = src->roi;

	// copy equal mgrid
	dst->mgrid = src->mgrid;

	// copy equal roi
	if(roi)
	{
		dst->roi = (RECT_D *)malloc(sizeof(RECT_D));
		memcpy(dst->roi, roi, sizeof(RECT_D));
	}
	else
	{
		dst->roi=NULL;
	}
	
	// copy equal source
	ret_source = dst->source;
	sz_cpy = sizeof(uchar_d)*width*height*depth;
	memcpy(ret_source[0], source[0], sz_cpy);
}

bool_d dinocv_set_roi(IMAGE_D *image, RECT_D *rect)
{
	RECT_D *roi;
	unsigned int left, top, right, bottom;

	left = 0;
	top = 0;
	right = image->width;
	bottom = image->height;

	if(left		>	rect->left		||
		top		>	rect->top		||
		right	<	rect->right		||
		bottom	<	rect->bottom	||
		right-left <= 0				||
		bottom-top <= 0				)
	{
		return d_false;
	}

	
	if(image->roi)	// backup the value if roi exist.
	{
		roi = image->roi;
	}
	else			// malloc if not exist.
	{
		roi = (RECT_D *)malloc(sizeof(RECT_D));
	}

	roi->left = rect->left;
	roi->top = rect->top;
	roi->right = rect->right;
	roi->bottom = rect->bottom;

	image->roi = roi;
	return d_true;
}

void dinocv_reset_roi(IMAGE_D *image)
{
	if(image->roi)
	{
		free(image->roi);
		image->roi = NULL;
	}
}

uchar_d *dinocv_get_1d_source(IMAGE_D *image)
{
	return (uchar_d *)image->source;
}
