#include "classifier.h"

CASCADED_DETECTOR_D cd;
char *str;
IMAGE_D *img;

DETECTION_RESULT *_g_detection_result;
DETECTION_RESULT *_g_r_detection_result;

int _gparm_sub_window_width;
int _gparm_sub_window_height;
int _gparm_initial_scale_factor_index;
float _gparm_scale_factor;
int _gparm_n_lut_sf;
int _gparm_detection_result_buffer_size;

unsigned int *lut_wsize_consider_sf = NULL;
unsigned int *lut_hsize_consider_sf = NULL;
int *lut_mv_sf;



bool load_cascaded_detector(const char *model_name,
							const int sub_window_width,
							const int sub_window_height,
							const int initial_scale_factor_index,
							const float scale_factor,
							const int n_lut_sf,
							const int detection_result_buffer_size,
							int width,
							int height)
{
	SIZE_D sz = dinocv_set_size(width, height);
	img = dinocv_create_image(&sz, 8);


	// parameter settings
	_gparm_sub_window_width = sub_window_width;
	_gparm_sub_window_height = sub_window_height;
	_gparm_initial_scale_factor_index = initial_scale_factor_index;
	_gparm_scale_factor = scale_factor;
	_gparm_n_lut_sf = n_lut_sf;
	_gparm_detection_result_buffer_size = detection_result_buffer_size;

	// memory allocation
	if(lut_wsize_consider_sf) free(lut_wsize_consider_sf);
	if(lut_hsize_consider_sf) free(lut_hsize_consider_sf);
	if(lut_mv_sf) free(lut_mv_sf);
	lut_wsize_consider_sf = (unsigned int *)malloc(sizeof(unsigned int) * n_lut_sf);
	memset(lut_wsize_consider_sf, 0, sizeof(unsigned int) * n_lut_sf);
	lut_hsize_consider_sf = (unsigned int *)malloc(sizeof(unsigned int) * n_lut_sf);
	memset(lut_hsize_consider_sf, 0, sizeof(unsigned int) * n_lut_sf);
	lut_mv_sf = (int *)malloc(sizeof(int) * n_lut_sf);
	memset(lut_mv_sf, 0, sizeof(int) * n_lut_sf);

	// load lut of sf
	float sf=1.;
	int delta=1;
	for(int i = 0 ; i < n_lut_sf ; i++)
	{
		lut_wsize_consider_sf[i]	= (int)d_round(sub_window_width*sf);
		lut_hsize_consider_sf[i]	= (int)d_round(sub_window_height*sf);
		lut_mv_sf[i]				= (int)(delta*sf);
		sf*=scale_factor;
	}

	_g_detection_result = (DETECTION_RESULT *)malloc(sizeof(DETECTION_RESULT));
	memset(_g_detection_result, 0, sizeof(DETECTION_RESULT));
	_g_detection_result->p_rt = (RECT_D *)malloc(sizeof(RECT_D) * detection_result_buffer_size);;
	memset(_g_detection_result->p_rt, 0, sizeof(RECT_D) * detection_result_buffer_size);

	_g_r_detection_result = (DETECTION_RESULT *)malloc(sizeof(DETECTION_RESULT));
	memset(_g_r_detection_result, 0, sizeof(DETECTION_RESULT));
	_g_r_detection_result->p_rt = (RECT_D *)malloc(sizeof(RECT_D) * detection_result_buffer_size);;
	memset(_g_r_detection_result->p_rt, 0, sizeof(RECT_D) * detection_result_buffer_size);


	FILE *fp = fopen(model_name, "rt");
	if(!fp) return false;


	//is >> cd.n_sc;
	fscanf(fp, "%d", &cd.n_sc);
	cd.p_sc = (STRONG_CLASSIFIER_D *)malloc(sizeof(STRONG_CLASSIFIER_D) * cd.n_sc);
	memset(cd.p_sc, 0, sizeof(STRONG_CLASSIFIER_D) * cd.n_sc);
	for(int i = 0 ; i < cd.n_sc ; i++)
	{
		//is >> cd.p_sc[i].n_wc >> cd.p_sc[i].threshold;
		fscanf(fp, "%d %f", &cd.p_sc[i].n_wc, &cd.p_sc[i].threshold);
		cd.p_sc[i].p_wc = (WEAK_CLASSIFIER_D *)malloc(sizeof(WEAK_CLASSIFIER_D) * cd.p_sc[i].n_wc);
		memset(cd.p_sc[i].p_wc, 0, sizeof(WEAK_CLASSIFIER_D) * cd.p_sc[i].n_wc);

		

		for(int j = 0 ; j < cd.p_sc[i].n_wc ; j++)
		{
			//malloc
			cd.p_sc[i].p_wc[j].fhlf.reversed_square_scale_factor = (float *)malloc(sizeof(float)*n_lut_sf);
			memset(cd.p_sc[i].p_wc[j].fhlf.reversed_square_scale_factor, 0, sizeof(float)*n_lut_sf);

			cd.p_sc[i].p_wc[j].fhlf.p_hb = (HAAR_BLOCK_D **)malloc(sizeof(HAAR_BLOCK_D *)*n_lut_sf);
			memset(cd.p_sc[i].p_wc[j].fhlf.p_hb, 0, sizeof(HAAR_BLOCK_D *)*n_lut_sf);

			cd.p_sc[i].p_wc[j].fhlf.area = (int *)malloc(sizeof(int) * n_lut_sf);
			memset(cd.p_sc[i].p_wc[j].fhlf.area, 0, sizeof(int) * n_lut_sf);


			//is >> cd.p_sc[i].p_wc[j].fhlf.n_hb >> cd.p_sc[i].p_wc[j].fhlf.hs._w >> cd.p_sc[i].p_wc[j].fhlf.hs._h;
			fscanf(fp, "%d %d %d", &cd.p_sc[i].p_wc[j].fhlf.n_hb, &cd.p_sc[i].p_wc[j].fhlf.hs._w, &cd.p_sc[i].p_wc[j].fhlf.hs._h);
			
			cd.p_sc[i].p_wc[j].fhlf.p_hb[0] = (HAAR_BLOCK_D *)malloc(sizeof(HAAR_BLOCK_D)*cd.p_sc[i].p_wc[j].fhlf.n_hb);
			for(int l = 0 ; l < cd.p_sc[i].p_wc[j].fhlf.n_hb ; l++)
			{
				fscanf(fp, "%d %d %d %d %d", &cd.p_sc[i].p_wc[j].fhlf.p_hb[0][l].x,
					&cd.p_sc[i].p_wc[j].fhlf.p_hb[0][l].y,
					&cd.p_sc[i].p_wc[j].fhlf.p_hb[0][l].w,
					&cd.p_sc[i].p_wc[j].fhlf.p_hb[0][l].h,
					&cd.p_sc[i].p_wc[j].fhlf.p_hb[0][l].weight);

			}
			fscanf(fp, "%f %d %d", &cd.p_sc[i].p_wc[j].alpha, &cd.p_sc[i].p_wc[j].polarity, &cd.p_sc[i].p_wc[j].threshold);

			// setting haar-like feature area
			cd.p_sc[i].p_wc[j].fhlf.area[0] = cd.p_sc[i].p_wc[j].fhlf.p_hb[0][0].w * cd.p_sc[i].p_wc[j].fhlf.p_hb[0][0].h;

			// setting scalable detector data
			cd.p_sc[i].p_wc[j].fhlf.reversed_square_scale_factor[0] = 1.0;
			sf=1.;
			HAAR_SHAPE_D hs = cd.p_sc[i].p_wc[j].fhlf.hs;
			HAAR_BLOCK_D hb = cd.p_sc[i].p_wc[j].fhlf.p_hb[0][0];
			int cx, cy, cw, ch;
			int adt;
			for(int k = 1 ; k < n_lut_sf ; k++)
			{
				sf *= scale_factor;

				// adjust _w, _h multiple
				cx = d_rounddown(hb.x * sf);
				cy = d_rounddown(hb.y * sf);
				cw = d_round(hb.w *sf);
				ch = d_round(hb.h *sf);

				if(((cw%hs._w)!= 0) )
				{		
					adt = hs._w-(cw%hs._w);
					cw += adt;

					if( cx+cw > d_round(_gparm_sub_window_width*sf) ) // x? ì™?™å ?™ì˜™ adt? ì™??? ì™?™å ?™ì˜™ ? ì™?™å ?™ì˜™è¦‹åº¸ï¿?hlf? ì™??? ì™?™å ?™ì˜™? ì™?™å ?™ì˜™? ì™??? ì™?™å ?™ì˜™? ì™?™å ?™ì˜™? ì™??? ì™??
					{
						if(cx>=adt)
							cx-=adt;
						else
							cw-=hs._w;
					}
				}
				if((ch%hs._h != 0) )
				{
					adt = hs._h-(ch%hs._h);
					ch += adt;

					if( cy+ch > d_round(_gparm_sub_window_height*sf) ) // y? ì™?™å ?™ì˜™ adt? ì™??? ì™?™å ?™ì˜™ ? ì™?™å ?™ì˜™è¦‹åº¸ï¿?hlf? ì™??? ì‹£ë¤„ì˜™? ì™?™å ?™ì˜™? ì™??? ì™?™å ?™ì˜™? ì™?™å ?™ì˜™? ì™??? ì™??
					{
						if(cy>=adt)
							cy-=adt;
						else
							ch-=hs._h;
					}
				}

				cd.p_sc[i].p_wc[j].fhlf.reversed_square_scale_factor[k] = 1.f/(((float)cw/hb.w)*((float)ch/hb.h));
					
				
				HAAR_LIKE_FEATURE_D thlf;
				create_haar_like_feature(thlf, hs, cx, cy, cw, ch);

				cd.p_sc[i].p_wc[j].fhlf.p_hb[k] = (HAAR_BLOCK_D *)malloc(sizeof(HAAR_BLOCK_D)*cd.p_sc[i].p_wc[j].fhlf.n_hb);
				memcpy(cd.p_sc[i].p_wc[j].fhlf.p_hb[k], thlf.p_hb, sizeof(HAAR_BLOCK_D)*cd.p_sc[i].p_wc[j].fhlf.n_hb);

				// setting haar-like feature area
				cd.p_sc[i].p_wc[j].fhlf.area[k] = cd.p_sc[i].p_wc[j].fhlf.p_hb[k][0].w * cd.p_sc[i].p_wc[j].fhlf.p_hb[k][0].h;
			}
			
		}
	}

	fclose(fp);
	return true;
}

void release_cascaded_detector()
{
	if(str) free(str);

	dinocv_release_image(img);

	free(lut_wsize_consider_sf);
	free(lut_hsize_consider_sf);
	free(lut_mv_sf);

	free(_g_detection_result->p_rt);
	free(_g_detection_result);
	free(_g_r_detection_result->p_rt);
	free(_g_r_detection_result);


	// Release Cascaded Detector
	for(int i = 0 ; i < cd.n_sc ; i++)
	{
		for(int j = 0 ; j < cd.p_sc[i].n_wc ; j++)
		{
			for(int k = 0 ; k < _gparm_n_lut_sf ; k++)
			{
				free(cd.p_sc[i].p_wc[j].fhlf.p_hb[k]);
			}
			free(cd.p_sc[i].p_wc[j].fhlf.reversed_square_scale_factor);
			free(cd.p_sc[i].p_wc[j].fhlf.p_hb);
			free(cd.p_sc[i].p_wc[j].fhlf.area);
		}
		free(cd.p_sc[i].p_wc);
	}

	free(cd.p_sc);
}

void create_haar_like_feature(HAAR_LIKE_FEATURE_D &hlf,
							  HAAR_SHAPE_D &hs,
							  int x, int y,
							  int w, int h)
{
	hlf.p_hb[0].weight=1;
	hlf.p_hb[0].x = x;
	hlf.p_hb[0].y = y;
	hlf.p_hb[0].w = w;
	hlf.p_hb[0].h = h;


	if(hs._w==2 && hs._h==2)
	{
		hlf.p_hb[1].weight=-2;
		hlf.p_hb[1].x = x+w/hs._w;
		hlf.p_hb[1].y = y;
		hlf.p_hb[1].w = w/hs._w;
		hlf.p_hb[1].h = h/hs._h;

		hlf.p_hb[2].weight=-2;
		hlf.p_hb[2].x = x;
		hlf.p_hb[2].y = y+h/hs._h;
		hlf.p_hb[2].w = w/hs._w;
		hlf.p_hb[2].h = h/hs._h;
	}
	else if(hs._w==1 && hs._h==2)
	{
		hlf.p_hb[1].weight=-2;
		hlf.p_hb[1].x = x;
		hlf.p_hb[1].y = y+h/hs._h;
		hlf.p_hb[1].w = w;
		hlf.p_hb[1].h = h/hs._h;
	}
	else if(hs._w==2 && hs._h==1)
	{
		hlf.p_hb[1].weight=-2;
		hlf.p_hb[1].x = x+w/hs._w;
		hlf.p_hb[1].y = y;
		hlf.p_hb[1].w = w/hs._w;
		hlf.p_hb[1].h = h;
	}
	else if(hs._w==3 && hs._h==1)
	{
		hlf.p_hb[1].weight=-2;
		hlf.p_hb[1].x = x+w/hs._w;
		hlf.p_hb[1].y = y;
		hlf.p_hb[1].w = w/hs._w;
		hlf.p_hb[1].h = h;
	}
	else if(hs._w==1 && hs._h==3)
	{
		hlf.p_hb[1].weight=-2;
		hlf.p_hb[1].x = x;
		hlf.p_hb[1].y = y+h/hs._h;
		hlf.p_hb[1].w = w;
		hlf.p_hb[1].h = h/hs._h;
	}


}

int get_block_sum(HAAR_BLOCK_D &hb, int **ii, int _x, int _y)
{
	int x, y, w, h;
	int a, b, c, d;

	x = hb.x+_x;
	y = hb.y+_y;
	w = hb.w;
	h = hb.h;

	a = (x<1 || y<1)	? 0 : ii[y-1][x-1];
	b = (y<1)			? 0 : ii[y-1][x+w-1];
	c = (x<1)			? 0 : ii[y+h-1][x-1];
	d = ii[y+h-1][x+w-1];
	return hb.weight*((a+d)-(b+c));
}

int weak_classifier(WEAK_CLASSIFIER_D &wc, int x, int y, int **ii, int sf_idx)
{
	int hlf_val;

	HAAR_BLOCK_D *p_hb = wc.fhlf.p_hb[sf_idx];	

	// get haar-like feature normalization factor
	hlf_val = get_block_sum(p_hb[0], ii, x, y);

	//float mean = (float)hlf_val / (p_hb[0].w * p_hb[0].h);
	float mean = (float)hlf_val / wc.fhlf.area[sf_idx];
	
	//n_calc_hlf++;

	if(mean != 0.)
	{
		// get haar-like feature
		hlf_val += get_block_sum(p_hb[1], ii, x, y);
		if(wc.fhlf.n_hb==3)
			hlf_val += get_block_sum(p_hb[2], ii, x, y);
		
		hlf_val = (int)d_round((hlf_val * 100) * wc.fhlf.reversed_square_scale_factor[sf_idx] * (1./mean));
	}
	else
		hlf_val = 0;
		
	if(wc.polarity>0)
	{
		if(hlf_val < wc.threshold)
			return 1;
	}
	else
	{
		if(hlf_val > wc.threshold)
			return 1;
	}
	return 0;
}

int strong_classifier(STRONG_CLASSIFIER_D &sc, int x, int y, int **ii, int sf_idx)
{
	float sum_left=0.;
	for(int n = 0 ; n < sc.n_wc ; n++)
	{
		sum_left += sc.p_wc[n].alpha * weak_classifier(sc.p_wc[n], x, y, ii, sf_idx);
	}
	if(sum_left >= sc.threshold)
		return TRUE;
	return FALSE;
}

int cascaded_classifier(int x, int y, int **ii, int sf_idx)
{
	for(int i = 1 ; i <= cd.n_sc ; i++)
	{
		if(!strong_classifier(cd.p_sc[i-1], x, y, ii, sf_idx))
		{
			return cd.n_sc-i; // return FALSE;
		}
	}

	return 0; // return TRUE;
}

DETECTION_RESULT *cascaded_classify(IMAGE_D *img)
{
	//register int x, y;
	//unsigned int width, height;
	
	int n_objects=0;

	//RECT_D rt;

	_g_detection_result->n_objects=0;
	memset(_g_detection_result->p_rt, 0, sizeof(DETECTION_RESULT) * _gparm_detection_result_buffer_size);
	_g_r_detection_result->n_objects=0;
	memset(_g_r_detection_result->p_rt, 0, sizeof(DETECTION_RESULT) * _gparm_detection_result_buffer_size);

	int **ii = make_integral_image(img);
	int sf_idx = _gparm_initial_scale_factor_index;
	int mv = lut_mv_sf[sf_idx];

	//width = lut_wsize_consider_sf[sf_idx];
	//height = lut_hsize_consider_sf[sf_idx];


	/*
	while( width <= img->width && height <= img->height )
	{
		for(y = 0 ; y <= (int)img->height-(int)height ; y+=mv)
		{
			for(x = 0 ; x <= (int)img->width-(int)width ; x+=mv)
			{
				if(cascaded_classifier(x, y, ii, sf_idx))
				{
					rt.left = x;
					rt.top = y;
					rt.right = x+width;
					rt.bottom = y+height;

					_g_detection_result->p_rt[n_objects++] = rt;
				}
			}
		}

		sf_idx++;
		mv = lut_mv_sf[sf_idx];
		width = lut_wsize_consider_sf[sf_idx];
		height = lut_hsize_consider_sf[sf_idx];
	}
	*/
	
	while( lut_wsize_consider_sf[sf_idx] <= img->width && lut_hsize_consider_sf[sf_idx] <= img->height )
	{
		create_swarm(img->width, img->height, NUM_PARTICLES, sf_idx);

		while( NextStage(img, ii, sf_idx, _g_detection_result, &n_objects) )
		{
			;//Sleep(500);
		}
		release_swarm();

		sf_idx++;
	}

	_g_detection_result->n_objects = n_objects;


	// clean up
	_dinocv_free((void **)ii);

	/*
	if(n_objects >= 2)
	{
		merge_rect();
		return _g_r_detection_result;
	}
	*/
	return _g_detection_result;
}

int **make_integral_image(IMAGE_D *img)
{
	register unsigned int i, j;
	int current_value;
	int **iimg = (int **)_dinocv_malloc(SZ_INT, img->width, img->height, img->bpp);

	iimg[0][0]=img->source[0][0];

	current_value=img->source[0][0];
	for(i = 1 ; i < img->width ; i++)
	{
		current_value += img->source[0][i];
		iimg[0][i] = current_value;
	}
	current_value=img->source[0][0];
	for(i = 1 ; i < img->height ; i++)
	{
		current_value += img->source[i][0];
		iimg[i][0] = current_value;
	}

	for(i = 1 ; i < img->height ; i++)
	{
		for(j = 1 ; j < img->width ; j++)
		{
			iimg[i][j] = iimg[i][j-1]+iimg[i-1][j]+img->source[i][j]-iimg[i-1][j-1];
		}
	}
	return iimg;
}

void merge_rect()
{
	//return;
	int n_objects = _g_detection_result->n_objects;
	register int i, j;
	int *labels, *counter;
	int group_id=0;
	int center_x, center_y, search_area;
	int cx, cy;

	RECT_D *prt = _g_detection_result->p_rt;
	RECT_D *r_prt = _g_r_detection_result->p_rt;


	labels = (int *)malloc(sizeof(int)*n_objects);
	memset(labels, 0, sizeof(int)*n_objects);

	for(i = 0 ; i < n_objects ; i++)
	{
		if(labels[i])
			continue;
		labels[i] = ++group_id;
			
		for(j = 0 ; j < n_objects ; j++)
		{	
			if(!labels[j]) // ?„ìž¬ ê·¸ë£¹ ?„ì´?”ê? ë°œê¸‰?˜ì–´?ˆì? ?Šì? RECTë§?ê²€??
			{

				center_x = (prt[i].left+prt[i].right)>>1;
				center_y = (prt[i].top+prt[i].bottom)>>1;
				search_area = (int)((prt[i].right-prt[i].left)*0.4f);
				//search_area = ((vrt[i].right-vrt[i].left)<<5)>>7;

				cx = (prt[j].left+prt[j].right)>>1;
				cy = (prt[j].top+prt[j].bottom)>>1;

				if(d_limit_cc(cx, center_x-search_area, center_x+search_area) &&
					d_limit_cc(cy, center_y-search_area, center_y+search_area))
				{
					labels[j] = group_id; // ê°™ì? ê·¸ë£¹ id ë°œê¸‰
				}
			}
		}
	}

	_g_r_detection_result->n_objects = group_id;

	counter = (int *)malloc(sizeof(int)*group_id);
	memset(counter, 0, sizeof(int)*group_id);

	for(i = 0 ; i < group_id ; i++)
	{
		for(j = 0 ; j < n_objects ; j++)
		{
			if(labels[j] == i+1)
			{
				r_prt[i].left += prt[j].left;
				r_prt[i].top += prt[j].top;
				r_prt[i].right += prt[j].right;
				r_prt[i].bottom += prt[j].bottom;

				counter[i]++;
			}
		}
		r_prt[i].left /= counter[i];
		r_prt[i].top /= counter[i];
		r_prt[i].right /= counter[i];
		r_prt[i].bottom /= counter[i];
	}
	
	free(labels);
	free(counter);

}


// related PSO

int _local_stage;
//POINT_D corn;

double *currentScore;
double *pbestScore;
double gbestScore;

POINT_D *pbestPosition;
int gbestIndex;

POINT_D *currentVelocity;
POINT_D *currentPosition;

int _g_pos_max_x;
int _g_pos_max_y;
int _g_num_particles;

void create_swarm(int pos_max_x, int pos_max_y, int num_particles, int sf_idx)
{
	currentScore = (double *)malloc(sizeof(double) * num_particles);
	pbestScore = (double *)malloc(sizeof(double) * num_particles);

	pbestPosition = (POINT_D *)malloc(sizeof(POINT_D) * num_particles);

	currentVelocity = (POINT_D *)malloc(sizeof(POINT_D) * num_particles);
	currentPosition = (POINT_D *)malloc(sizeof(POINT_D) * num_particles);


	_g_pos_max_x = pos_max_x;
	_g_pos_max_y = pos_max_y;
	_g_num_particles = num_particles;

	INIT_WELL512();
	//srand((unsigned int) time(NULL));
	//rand();

	_local_stage = 0;
	gbestIndex = INT_MAX;
	gbestScore = INT_MAX;	// DBL_MAX;
	//corn.x = POSITION_MAX_X / 2; // (rand() % (POSITION_MAX_X - CORN_IMAGE_WIDTH)) + CORN_IMAGE_WIDTH / 2;
	//corn.y = POSITION_MAX_Y / 2; // (rand() % (POSITION_MAX_Y - CORN_IMAGE_HEIGHT)) + CORN_IMAGE_HEIGHT / 2;

	for(int i = 0; i < NUM_PARTICLES; i++)
	{
		currentVelocity[i].x = WELLRNG512_limit(-VELOCITY_MAX_X, VELOCITY_MAX_X);
		currentVelocity[i].y = WELLRNG512_limit(-VELOCITY_MAX_Y, VELOCITY_MAX_Y);
		currentPosition[i].x =  WELLRNG512_limit(0, _g_pos_max_x-lut_wsize_consider_sf[sf_idx]-1);
		currentPosition[i].y = WELLRNG512_limit(0, _g_pos_max_y-lut_hsize_consider_sf[sf_idx]-1);
		pbestScore[i] = INT_MAX;	// DBL_MAX;
	}

	
}

void release_swarm()
{
	free(currentScore);
	free(pbestScore);

	free(pbestPosition);
	free(currentVelocity);
	free(currentPosition);
}


bool NextStage(IMAGE_D *img, int **ii, int sf_idx, DETECTION_RESULT *detection_result, int *n_objects)
{
	//IMAGE_D *result = dinocv_load_image(INPUT_FILE_NAME, &dinocv_set_size(img->width, img->height), 24, IMGMDL_RGB);
	int numCorrectDetection;

	RECT_D rt;

	if(_local_stage == 0)
	{
		/*
		DrawParticles(result, sf_idx);
		dinocv_save_bitmap("result.bmp", result);
		dinocv_release_image(result);
		printf("draw particles\n");
		getchar();
		*/
		

		_local_stage++;
		return true;
	}

	numCorrectDetection = EvaluatePosition(ii, sf_idx);
	CalculateVelocity();
	CalculatePosition();

	/*
	DrawParticles(result, sf_idx);
	dinocv_save_bitmap("result.bmp", result);
	dinocv_release_image(result);
	printf("draw particles\n");
	getchar();
	*/
	
	_local_stage++;

	//if(gbestScore < CORN_IMAGE_WIDTH)
	//printf("numCorrectDetection=%d\n", numCorrectDetection);
	if(numCorrectDetection > 0)
	{
		/*
		result = dinocv_load_image(INPUT_FILE_NAME, &dinocv_set_size(img->width, img->height), 24, IMGMDL_RGB);		
		
		dinocv_draw_rect(result, &dinocv_set_rect(pbestPosition[gbestIndex].x, pbestPosition[gbestIndex].y,
			pbestPosition[gbestIndex].x+lut_wsize_consider_sf[sf_idx], pbestPosition[gbestIndex].y+lut_hsize_consider_sf[sf_idx]),
			&dinocv_set_color(255,0,0), 3);
		dinocv_save_bitmap("result.bmp", result);
		dinocv_release_image(result);
		*/

		rt.left = pbestPosition[gbestIndex].x;
		rt.top = pbestPosition[gbestIndex].y;
		rt.right = rt.left + lut_wsize_consider_sf[sf_idx];
		rt.bottom = rt.top + lut_hsize_consider_sf[sf_idx];
		_g_detection_result->p_rt[(*n_objects)++] = rt;

		printf(" @ Coordinates\t=(%d, %d)\t\tStage=%d\n", pbestPosition[gbestIndex].x, pbestPosition[gbestIndex].y, (int)(--_local_stage) );
		return false;
	}
	else if(_local_stage > NUM_STAGE)
	{
		printf(" # Face was not found !!\n");
		return false;
	}

	return true;
}


int EvaluatePosition(int **ii, int sf_idx)
{
	int sub_width = lut_wsize_consider_sf[sf_idx];
	int sub_height = lut_hsize_consider_sf[sf_idx];
	int numCorrectDetection=0;

	for(int i = 0; i < NUM_PARTICLES; i++)
	{
		// Evaluate
		//currentScore[i] = abs(currentPosition[i].x - corn.x) + abs(currentPosition[i].y - corn.y);
		if(currentPosition[i].x > _g_pos_max_x-sub_width)
			currentPosition[i].x = _g_pos_max_x-sub_width-1;
		if(currentPosition[i].y > _g_pos_max_y-sub_height)
			currentPosition[i].y = _g_pos_max_y-sub_height-1;

		currentScore[i] = cascaded_classifier(currentPosition[i].x, currentPosition[i].y, ii, sf_idx);
		numCorrectDetection += currentScore[i] == 0. ? 1 : 0;
		// Renew pbest
		if(pbestScore[i] > currentScore[i])
		{
			pbestScore[i] = currentScore[i];
			pbestPosition[i] = currentPosition[i];

			// Renew gbest
			if(gbestScore > currentScore[i])
			{
				gbestScore = currentScore[i];
				gbestIndex = i;
			}
		}
		//printf("%d ", (int)currentScore[i]);
	}
	//printf("\n");
	
	return numCorrectDetection;
}


void CalculateVelocity()
{
	srand((unsigned int) time(NULL));

	for(int i = 0; i < NUM_PARTICLES; i++)
	{
		currentVelocity[i].x = (LONG) (INERTIA_COEFFICIENT * currentVelocity[i].x
			+ PARTICLE_COEFFICIENT * ((double) rand() / RAND_MAX) * (pbestPosition[i].x - currentPosition[i].x)
			+ SWARM_COEFFICIENT * ((double) rand() / RAND_MAX) * (pbestPosition[gbestIndex].x - currentPosition[i].x));

		currentVelocity[i].y = (LONG) (INERTIA_COEFFICIENT * currentVelocity[i].y
			+ PARTICLE_COEFFICIENT * ((double) rand() / RAND_MAX) * (pbestPosition[i].y - currentPosition[i].y)
			+ SWARM_COEFFICIENT * ((double) rand() / RAND_MAX) * (pbestPosition[gbestIndex].y - currentPosition[i].y));

		if(currentVelocity[i].x > VELOCITY_MAX_X)
			currentVelocity[i].x = VELOCITY_MAX_X;
		else if(currentVelocity[i].x < -VELOCITY_MAX_X)
			currentVelocity[i].x = -VELOCITY_MAX_X;
		
		if(currentVelocity[i].y > VELOCITY_MAX_Y)
			currentVelocity[i].y = VELOCITY_MAX_Y;
		else if(currentVelocity[i].y < -VELOCITY_MAX_Y)
			currentVelocity[i].y = -VELOCITY_MAX_Y;
	}
}


void CalculatePosition()
{
	for(int i = 0; i < NUM_PARTICLES; i++)
	{
		currentPosition[i].x += currentVelocity[i].x;
		currentPosition[i].y += currentVelocity[i].y;

		if(currentPosition[i].x < 0)
			currentPosition[i].x = 0;
		else if(currentPosition[i].x > _g_pos_max_x)
			currentPosition[i].x = _g_pos_max_x -  1;

		if(currentPosition[i].y < 0)
			currentPosition[i].y = 0;
		else if(currentPosition[i].y > _g_pos_max_y)
			currentPosition[i].y = _g_pos_max_y -  1;
	}
}

void DrawParticles(IMAGE_D *img, int sf_idx)
{
	int l,t,r,b;

	int local_width = lut_wsize_consider_sf[sf_idx];
	int local_height = lut_hsize_consider_sf[sf_idx];

	for(int i = 0; i < _g_num_particles; i++)
	{
		l = currentPosition[i].x;
		t = currentPosition[i].y;
		r = currentPosition[i].x + local_width;
		b = currentPosition[i].y + local_height;
		dinocv_draw_rect(img, &dinocv_set_rect(l,t,r,b), &dinocv_set_color(0,0,255), 1);
	}
}
