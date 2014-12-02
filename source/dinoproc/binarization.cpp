#include "../common/def_os_selector_export.h"
#include "binarization.h"

void _bin_make_image(IMAGE_D *image, int th){
	register uint_d i, j;
	const int high = th>=0?255:0;
	const int low = th>=0?0:255;
	th		= th>=0?th:-th;
	uchar_d **source = image->source;
	RECT_D rt = dinocv_get_rect(image);

	for(i = rt.top ; i < rt.bottom ; i++){
		for(j = rt.left ; j < rt.right ; j++){
			source[i][j] = source[i][j] > th ? high : low;
		}
	}
}

int _bin_repeat(IMAGE_D *image){
	int th =128;
	int oldth;//이전 임계값 저장
	int underSum = 0;
	int upperSum = 0;
	int upperCnt = 0;
	int underCnt = 0;
	int upperAvg = 0;
	int underAvg = 0;
	register uint_d i, j;
	uchar_d **source = image->source;

	RECT_D rt = dinocv_get_rect(image);

	//반복적 방법에 의한 임계값 결정
	while(1){
		upperSum = upperCnt = underSum = underCnt = 0;

		for(i = rt.top ; i < rt.bottom ; i++){
			for(j = rt.left ; j < rt.right ; j++){
				if(source[i][j] >= th){
					upperSum += source[i][j];
					upperCnt++;
				}
				else{
					underSum += source[i][j];
					underCnt++;
				}
			}
		}

		if(upperCnt && underCnt)
		{
			upperAvg = upperSum/upperCnt;
			underAvg = underSum/underCnt;
		}
		else
		{
			upperAvg = underAvg = 0;
		}
		oldth = th;
		th = (upperAvg + underAvg) >> 1;

		if(oldth == th)
			break;
	}

	return th;
}

int _bin_otsu(IMAGE_D *image){
	double hist[256] = {0,}, chist[256] = {0,}, cxhist[256] = {0,};
	double costMax = 0, mb, mo, cost;
	int breadth;
	register uint_d i, j;
	int threshold = 0, retThreshold=0;
	uchar_d **source = image->source;
	RECT_D rt = dinocv_get_rect(image);

	breadth = (rt.right-rt.left)*(rt.bottom-rt.top);

	// make histogram
	for(i = rt.top ; i < rt.bottom ; i++)
	{
		for(j = rt.left ; j < rt.right ; j++)
		{
			hist[source[i][j]]++;
		}
	}

	// normalize
	for(i = 0 ; i < 256 ; i++)
	{
		hist[i] /= breadth;
	}
	
	// make 0- and 1-st cumulative histogram;
	chist[0] = hist[0];
	cxhist[0] = 0;
	for(i=1; i<256; i++)
	{
		chist[i] = chist[i-1] + hist[i] ;       //0-th cumuatlive histogram ;
		cxhist[i] = cxhist[i-1] + i * hist[i] ; //1-th cumulative histogram ;
	};

	while(threshold != 256)
	{
		//배경(mb)과 객체(mo) 픽셀들의 그레이스케일 레벨 평균값
		mb = cxhist[threshold]/chist[threshold];
		mo = (cxhist[255]-cxhist[threshold])/(1-chist[threshold]);

		//비용 함수 초기화
		cost = chist[threshold]*(1-chist[threshold])*(mb-mo)*(mb-mo) ;

		if(cost > costMax){
			costMax = cost;
			retThreshold = threshold;
		}

		threshold++;
	}
	
	return retThreshold;
}


int dinocv_binarization(IMAGE_D *image, int bntype)
{
	int (*bin[2]) (IMAGE_D *) = {_bin_repeat, _bin_otsu};
	int th=-1;

	int idx = (bntype&0xFF00)>>8;

	// bintype 구조 : FF FF FF FF 중에 0~15까지는 실제 bintype으로 쓰고, 16~31까지는 threshold로 사용
	// BIN_REPEAT	: repeat binarizatioin
	// BIN_OTSU		: otsu's method
	// BIN_SIMPLE	: simple binarization by user input threshold
	
	if(!idx)
	{
		th = bin[bntype](image);
		_bin_make_image(image, th);
	}
	else
	{
		_bin_make_image(image, ((bntype&0xFF00)>>8));
		th = ((bntype&0xFF00)>>8);
	}

	return th;
}

void _bin_make_image_with_image(IMAGE_D *to_img, IMAGE_D *from_img, int th){
	register uint_d i, j;
	const int high = th>=0?255:0;
	const int low = th>=0?0:255;
	int to_width, to_height;
	int from_width, from_height;
	uchar_d **to_source, **from_source;
	int gap_w, gap_h;

	th	= th>=0?th:-th; /* if variable 'th' is negative value, it makes reversed make image */
	RECT_D to_rt = dinocv_get_rect(to_img);
	RECT_D from_rt = dinocv_get_rect(from_img);
	to_width = to_rt.right-to_rt.left;
	to_height = to_rt.bottom-to_rt.top;
	from_width = from_rt.right-from_rt.left;
	from_height = from_rt.bottom-from_rt.top;
	to_source = to_img->source;
	from_source = from_img->source;
	gap_w = from_rt.left-to_rt.left;
	gap_h = from_rt.top-to_rt.top;
	if(from_width != to_height ||
		from_height != to_height){
			return;
	}
	
	for(i = to_rt.top ; i < to_rt.bottom ; i++){
		for(j = to_rt.left ; j < to_rt.right ; j++){
			to_source[i][j] = from_source[i+gap_h][j+gap_w] > th ? high : low;
		}
	}
}

bool_d dinocv_reverse_image(IMAGE_D *image)
{
	register uint_d i, j;
	const int depth = image->bpp>>3;
	uchar_d **source = image->source;
	RECT_D rt = dinocv_get_rect(image);

	switch(depth){
	case 1:
		for(i = rt.top ; i < rt.bottom ; i++){
			for(j = rt.left ; j < rt.right ; j++){
				source[i][j] = 255-source[i][j];
			}
		}
		break;
	case 3:
		rt.right = (rt.right-rt.left)*depth;
		for(i = rt.top ; i < rt.bottom ; i++){
			for(j = rt.left ; j < rt.right ; j+=depth){
				source[i][j]	=	255-source[i][j];
				source[i][j+1]	=	255-source[i][j+1];
				source[i][j+2]	=	255-source[i][j+2];
			}
		}
		break;
	default:
		return d_false;
	}

	return d_true;
}
