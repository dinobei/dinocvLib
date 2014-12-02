#include "../common/def_os_selector_export.h"
#include "feature.h"

// INDEPENDANT INTERNAL METHODS
float _dinocv_calc_moment(IMAGE_D *image, int p, int q){
	register int i, j, k;
	float m_pq=0, temp;
	const int width = image->width;
	const int height = image->height;
	uchar_d **source = image->source;

	for(i = 0 ; i < height ; i++){
		for(j = 0 ; j < width ; j++){
			temp=1;

			for(k = 0 ; k < p ; k++) temp *= j; // x^q
			for(k = 0 ; k < q ; k++) temp *= i; // y^p
			
			m_pq += (temp*(source[i][j]?1:0));
		}
	}
	return m_pq;
}

// Feature Data Proc
void dinocv_feature_data_alloc(FEATURE_D *feature, int size){
	feature->arr = (float *)malloc(sizeof(float)*size);
	feature->idx=0;
}

void dinocv_feature_data_init(FEATURE_D *feature){
	feature->idx=0;
}

void dinocv_feature_data_free(FEATURE_D *feature){
	free((*feature).arr);
}

void dinocv_feature_data_add(FEATURE_D *feature, float data){
	feature->arr[feature->idx] = data;
	feature->idx++;
}

// Create Moment
MOMENT_D* dinocv_feature_create_moment(IMAGE_D *image){
	MOMENT_D *moment;
	float g00, g10, g01, g20, g02, g11, g30, g03, g21, g12; // moment
	float c00, c20, c02, c11, c30, c03, c21, c12; // centroid moment
	float n20, n02, n11, n30, n03, n21, n12; // regulated centroid moment
	float cx, cy;

	int moment_size = sizeof(MOMENT_D);
	moment = (MOMENT_D *)malloc(moment_size);
	memset(moment, 0, moment_size);

	// 기하학적 모멘트 구하기
	moment->g00 = g00 = _dinocv_calc_moment(image, 0, 0);//면적
	moment->g10 = g10 = _dinocv_calc_moment(image, 1, 0);//중점모멘트 x좌표 구할때 사용
	moment->g01 = g01 = _dinocv_calc_moment(image, 0, 1);//중점모멘트 y좌표 구할때 사용
	moment->g20 = g20 = _dinocv_calc_moment(image, 2, 0);//행분산 구할때 쓰임
	moment->g02 = g02 = _dinocv_calc_moment(image, 0, 2);//열분산 구할때 쓰임
	moment->g11 = g11 = _dinocv_calc_moment(image, 1, 1);//혼합 분산 구할때 쓰임
	moment->g30 = g30 = _dinocv_calc_moment(image, 3, 0);//불변모멘트
	moment->g03 = g03 = _dinocv_calc_moment(image, 0, 3);//불변모멘트
	moment->g21 = g21 = _dinocv_calc_moment(image, 2, 1);//불변모멘트
	moment->g12 = g12 = _dinocv_calc_moment(image, 1, 2);//불변모멘트

	// 중심 모멘트 구하기
	moment->cx = cx = g10 / g00;//중점모멘트 x좌표
	moment->cy = cy = g01 / g00;//중점모멘트 y좌표

	// 중심 모멘트 정규화
	moment->c00 = c00 = g00;
	moment->c20 = c20 = g20 - cx*g10;
	moment->c02 = c02 = g02 - cy*g01;
	moment->c11 = c11 = g11 - cx*g01;
	moment->c30 = c30 = g30 - 3*cx*g20 + 2*cx*cx*g10;
	moment->c03 = c03 = g03 - 3*cy*g02 + 2*cy*cy*g01;
	moment->c21 = c21 = g21 - 2*cx*g11 - cy*g20 + 2*cx*cx*g01;
	moment->c12 = c12 = g12 - 2*cy*g11 - cx*g02 + 2*cy*cy*g10;

	// 정규화된 중심 모멘트
	//fs_pow
#ifdef __PER_NOT_SUPPORT_MATH_H__
	moment->n20 = n20 = c20 / (float)fs_pow(c00, 2.f);
	moment->n02 = n02 = c02 / (float)fs_pow(c00, 2.f);
	moment->n11 = n11 = c11 / (float)fs_pow(c00, 2.f);
	moment->n30 = n30 = c30 / (float)fs_pow(c00, 2.5f);
	moment->n03 = n03 = c03 / (float)fs_pow(c00, 2.5f);
	moment->n21 = n21 = c21 / (float)fs_pow(c00, 2.5f);
	moment->n12 = n12 = c12 / (float)fs_pow(c00, 2.5f);
#else
	moment->n20 = n20 = c20 / (float)pow(c00, 2.f);
	moment->n02 = n02 = c02 / (float)pow(c00, 2.f);
	moment->n11 = n11 = c11 / (float)pow(c00, 2.f);
	moment->n30 = n30 = c30 / (float)pow(c00, 2.5f);
	moment->n03 = n03 = c03 / (float)pow(c00, 2.5f);
	moment->n21 = n21 = c21 / (float)pow(c00, 2.5f);
	moment->n12 = n12 = c12 / (float)pow(c00, 2.5f);
#endif

	return moment;
}

// Remove Moment
bool_d dinocv_feature_remove_moment(MOMENT_D *moment){
	free(moment);
	return d_true;
}

int dinocv_feature_get_area(MOMENT_D *moment){
	return (int)moment->g00;
}

POINT_D dinocv_feature_get_centroid(MOMENT_D *moment){
	POINT_D p;
	float area = moment->g00;

	p.x = (int)(moment->g10 / area);
	p.y = (int)(moment->g01 / area);

	return p;
}

float dinocv_feature_get_row_divergence(MOMENT_D *moment){
	return moment->c20/moment->g00;
}

float dinocv_feature_get_column_divergence(MOMENT_D *moment){
	return moment->c02/moment->g00;
}

float dinocv_feature_get_mix_divergence(MOMENT_D *moment){
	return moment->c11/moment->g00;
}

bool_d dinocv_feature_add_invariant_moment(MOMENT_D *moment, FEATURE_D *feature){
	float farr[7]={0.,};
	float g00, g10, g01, g20, g02, g11, g30, g03, g21, g12; // moment
	float c00, c20, c02, c11, c30, c03, c21, c12; // centroid moment
	float n20, n02, n11, n30, n03, n21, n12; // regulated centroid moment
	float cx, cy;

	g00 = moment->g00;
	g10 = moment->g10;
	g01 = moment->g01;
	g20 = moment->g20;
	g02 = moment->g02;
	g11 = moment->g11;
	g30 = moment->g30;
	g03 = moment->g03;
	g21 = moment->g21;
	g12 = moment->g12;
	
	c00 = moment->c00;
	c20 = moment->c20;
	c02 = moment->c02;
	c11 = moment->c11;
	c30 = moment->c30;
	c03 = moment->c03;
	c21 = moment->c21;
	c12 = moment->c12;
	
	n20 = moment->n20;
	n02 = moment->n02;
	n11 = moment->n11;
	n30 = moment->n30;
	n03 = moment->n03;
	n21 = moment->n21;
	n12 = moment->n12;

	cx = moment->cx;
	cy = moment->cy;
	/*
	farr[0] = moment->n20 + moment->n02;
	farr[1] = (moment->n20 - moment->n02)*(moment->n20 - moment->n02) + 4*moment->n11*moment->n11;
	farr[2] = (moment->n30 - 3*moment->n12)*(moment->n30 - 3*moment->n12) + (3*moment->n21 - moment->n03)*(3*moment->n21 - moment->n03);
	farr[3] = (moment->n30 + moment->n12)*(moment->n30 + moment->n12) + (moment->n21 + moment->n03)*(moment->n21 + moment->n03);
	farr[4] = (moment->n30 - 3*moment->n12)*(moment->n30 + moment->n12)*((moment->n30 + moment->n12)*(moment->n30 + moment->n12) - 3*(moment->n21 + moment->n03)*(moment->n21 + moment->n03))
		+ (3*moment->n21 - moment->n03)*(moment->n21 + moment->n03)*(3*(moment->n30 + moment->n12)*(moment->n30 + moment->n12) - (moment->n21 + moment->n03)*(moment->n21 + moment->n03));
	farr[5] = (moment->n20 - moment->n02)*((moment->n30 + moment->n12)*(moment->n30 + moment->n12) - (moment->n21 + moment->n03)*(moment->n21 + moment->n03))
		+ 4*moment->n11*(moment->n30 + moment->n12)*(moment->n21 + moment->n03);
	farr[6] = (3*moment->n21 - moment->n03)*(moment->n30 + moment->n12)*((moment->n30 + moment->n12)*(moment->n30 + moment->n12) - 3*(moment->n21 + moment->n03)*(moment->n21 + moment->n03))
		+ (3*moment->n12 - moment->n30)*(moment->n21 + moment->n03)*(3*(moment->n30 + moment->n12)*(moment->n30 + moment->n12) - (moment->n21 + moment->n03)*(moment->n21 + moment->n03));
	*/
	//*
	farr[0] = n20 + n02;
	farr[1] = (n20 - n02)*(n20 - n02) + 4*n11*n11;
	farr[2] = (n30 - 3*n12)*(n30 - 3*n12) + (3*n21 - n03)*(3*n21 - n03);
	farr[3] = (n30 + n12)*(n30 + n12) + (n21 + n03)*(n21 + n03);
	farr[4] = (n30 - 3*n12)*(n30 + n12)*((n30 + n12)*(n30 + n12) - 3*(n21 + n03)*(n21 + n03))
		+ (3*n21 - n03)*(n21 + n03)*(3*(n30 + n12)*(n30 + n12) - (n21 + n03)*(n21 + n03));
	farr[5] = (n20 - n02)*((n30 + n12)*(n30 + n12) - (n21 + n03)*(n21 + n03))
		+ 4*n11*(n30 + n12)*(n21 + n03);
	farr[6] = (3*n21 - n03)*(n30 + n12)*((n30 + n12)*(n30 + n12) - 3*(n21 + n03)*(n21 + n03))
		+ (3*n12 - n30)*(n21 + n03)*(3*(n30 + n12)*(n30 + n12) - (n21 + n03)*(n21 + n03));
	//*/

	dinocv_feature_data_add(feature, farr[0]);
	dinocv_feature_data_add(feature, farr[1]);
	dinocv_feature_data_add(feature, farr[2]);
	dinocv_feature_data_add(feature, farr[3]);
	dinocv_feature_data_add(feature, farr[4]);
	dinocv_feature_data_add(feature, farr[5]);
	dinocv_feature_data_add(feature, farr[6]);
	return d_true;
}

bool_d dinocv_feature_add_projection(IMAGE_D *image, int xsplit, int ysplit, FEATURE_D *feature){
	register int i, j;
	const int width = image->width;
	const int height = image->height;
	uchar_d **source = image->source;
	int *prj_arr, prj_idx, split_idx;
	float tval;

	if(width%xsplit || height%ysplit)
		return d_false;

	prj_idx = sizeof(int)*(width+height);
	prj_arr = (int *)malloc(prj_idx);
	memset(prj_arr, 0, prj_idx);
	prj_idx = 0;

	// Horizontal Acc
	for(i = height ; i-- ; ){
		for(j = width ; j-- ; ){
			prj_arr[prj_idx] += source[i][j]?1:0;
		}
		prj_idx++;
	}

	// Vertical Acc
	for(j = width ; j-- ; ){
		for(i = height ; i-- ; ){
			prj_arr[prj_idx] += source[i][j]?1:0;
		}
		prj_idx++;
	}


	// Split Value
	split_idx=0;
	while(split_idx < width){
		tval=0.;
		for(i = xsplit ; i-- ; )
			tval += prj_arr[split_idx++];
		tval = tval*(1.f/xsplit)*(1.f/width);
		dinocv_feature_data_add(feature, tval);
	}

	while(split_idx < width+height){
		tval=0.;
		for(i = ysplit ; i-- ; )
			tval += prj_arr[split_idx++];
		tval = tval*(1.f/ysplit)*(1.f/height);
		dinocv_feature_data_add(feature, tval);
	}

	free(prj_arr);
	return d_true;
}

bool_d dinocv_feature_add_profile(IMAGE_D *image, FEATURE_D *feature){
	/*
	int i, j;
	float val=0.;
	int idx=0;
	float arr[120]={0,};
	int k=0;
	int sum=0;
	float tmp[12] = {0.,};


	//상 프로파일
	for(i = 0 ; i < width ; i++){
		for(j = 0 ; j < height ; j++){
			if(source[j][i]){
				arr[idx++] = (float)j;
				//j = height;
				break;
			}
		}
		if(j == height)
			arr[idx++] = (float)height-1;
			
	}

	//우 프로파일
	for(i = 0 ; i < height ; i++){
		for(j = width-1 ; j >= 0 ; j--){
			if(source[i][j]){
				arr[idx++] = (float)width-j;
				//j=-1;
				break;
			}
		}
		if(j == -1)
			arr[idx++] = 0;
	}
	
	//하 프로파일
	for(i = 0 ; i < width ; i++){
		for(j = height-1 ; j >= 0 ; j--){
			if(source[j][i]){
				arr[idx++] = (float)height-j;
				//j = -1;
				break;
			}
		}
		if(j == -1)
			arr[idx++] = 0;
	}

	//좌 프로파일
	for(i = 0 ; i < height ; i++){
		for(j = 0 ; j < width ; j++){
			if(source[i][j]){
				arr[idx++] = (float)j;
				break;
			}
		}
		if(j == width)
			arr[idx++] = (float)width-1;
	}
	*/






	/* 상, 하, 좌, 우 프로파일값을 보기위한 임시 코드
	system("cls");
	//상 0 30
	//우 30 60
	//하 60 90
	//좌 90 120
	for(i = 0 ; i < 30 ; i++)
		printf("arr[%d] : %f\n", i, arr[i]);
	getchar();
	*/


	/* 15개씩 평균내서 차원축소(width == 30pixel)
	sum=0;	idx=0;	k=0;
	for(i = 0 ; i < 8 ; i++){
		for(j = 0 ; j < width/2 ; j++){
			sum +=arr[idx++];
		}
		prof[k++]=sum/(width/2);
	}
	*/





	/*
	sum=0; idx=0; k=0;

	for(i = 0 ; i < 12 ; i++){
		for(j = 0 ; j < 10 ; j++){
			sum += (int)arr[idx++];
		}
		//tmp[k++] = sum/10.0f;
		tmp[k++] = sum/300.0f;
		sum=0;
	}

	for(i = 0 ; i < 12 ; i++)
		feature_data_add(feature, tmp[i]);
	*/
	return d_true;
}
