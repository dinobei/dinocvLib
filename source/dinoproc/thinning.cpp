#include "../common/def_os_selector_export.h"
#include "thinning.h"

void _delete_a(uchar_d **img, uchar_d **timg, int cx, int cy)
{
	int i, j;

	for(i = cy ; i-- ; )
		for(j = cx ; j-- ; )
			if(timg[i][j]){
				img[i][j] = 0;
				timg[i][j] = 0;
			}
}

int _nays(uchar_d **img, int i, int j)
{
	int k,l,N=0;

	for(k=i-1;k<=i+1;k++)
		for(l=j-1;l<=j+1;l++)
			if(k!=i || l!=j)
				if(img[k][l] >=1) N++;

	return N;
}

int _connect(uchar_d **img, int i, int j)
{
	int n=0;

	if( img[i][j+1] >= 1 && img[i-1][j+1] == 0 ) n++;
	if( img[i-1][j+1] >= 1 && img[i-1][j] == 0 ) n++;
	if( img[i-1][j] >= 1 && img[i-1][j-1] == 0 ) n++;
	if( img[i-1][j-1] >= 1 && img[i][j-1] == 0 ) n++;
	if( img[i][j-1] >= 1 && img[i+1][j-1] == 0 ) n++;
	if( img[i+1][j-1] >= 1 && img[i+1][j] == 0 ) n++;
	if( img[i+1][j] >= 1 && img[i+1][j+1] == 0 ) n++;
	if( img[i+1][j+1] >= 1 && img[i][j+1] == 0 ) n++;
	return n;
}

void dinocv_thinning(IMAGE_D *image)
{
	int i, j;
	int again = 1;
	int N;
	uchar_d **timg, **source;
	int width, height;

	source=image->source;
	width = dinocv_get_width(image);
	height = dinocv_get_height(image);

	//timg = (uchar_d **)memmgr_get_image_source(memmgr_regul_image);
	timg = (uchar_d **)malloc(sizeof(uchar_d *)*height);
	memset(timg, 0, sizeof(uchar_d *)*height);
	for(i = 0 ; i < height ; i++){
		timg[i] = (uchar_d *)malloc(sizeof(uchar_d)*width);
		memset(timg[i], 0, sizeof(uchar_d)*width);
	}

	/**/
	//테두리 0으로 만들기
	for(i=0; i<height; i++)
	{
		source[i][0] = 0;
		source[i][width-1] = 0;
	}
	for(j=0; j<width; j++)
	{
		source[0][j] = 0;
		source[height-1][j] = 0;
	}

	//영상의 검은색을 1, 배경을 0로 바꿈.
	for(i=0; i<height; i++)
	{
		for(j=0; j<width; j++)
		{
			if(source[i][j]>0) source[i][j] = 1; //white
			else source[i][j] = 0;                // black
			timg[i][j] = 0;                      // timg 초기화
		}
	}

	while(again)
	{
		again = 0;
		// first sub-iteration
		for(i=0;i<height;i++)
			for(j=0;j<width;j++)
			{
				if(source[i][j] != 1) continue;

				//영상의 중앙 부분을 중심으로 1인 영상의 개수 계산.
				N = _nays(source, i, j);

				if((N>=2 && N<=6) && _connect(source, i,j) == 1)
				{
					if((source[i][j+1]*source[i-1][j]*source[i][j-1]) == 0 && 
						(source[i-1][j]*source[i+1][j]*source[i][j-1])== 0)
					{
						timg[i][j] = 1;
						again = 1;
					}
				}
			}
			_delete_a(source, timg, width, height);
			if(again == 0) break;

			// Second sub-iteration
			for(i=0;i<height;i++)
				for(j=0;j<width;j++)
				{
					if(source[i][j] != 1) continue;

					N = _nays(source, i, j);

					if((N>=2 && N<=6) && _connect(source,i,j) == 1)
					{
						if((source[i-1][j]*source[i][j+1]*source[i+1][j]) == 0 &&
							(source[i][j+1]*source[i+1][j]*source[i][j-1]) == 0)
						{
							timg[i][j] = 1;
							again = 1;
						}
					}
				}
				_delete_a(source, timg, width, height);

	}
	// 1을 0로 0를 255로 다시 환원
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
			if(source[i][j] > 0) 
			{
				source[i][j] = 255;			
			}
			else source[i][j] = 0;
	
	//memmgr_put_image_source(memmgr_regul_image, timg);
	for(i = 0 ; i < height ; i++)
		free(timg[i]);
	free(timg);
}
