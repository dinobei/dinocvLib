#include "classifier.h"



CASCADED_DETECTOR_D *load_cascaded_detector(const char *model_name,
							const int sub_window_width,
							const int sub_window_height,
							const int initial_scale_factor_index,
							const float scale_factor,
							const int n_lut_sf,
							const int detection_result_buffer_size,
							int width,
							int height)
{
#ifdef _WIN32
	FILE *fp;
	fopen_s(&fp, model_name, "rt");
#else
	FILE *fp = fopen(model_name, "rt");
#endif
	if(!fp) return NULL;

	CASCADED_DETECTOR_D *cd;
	SIZE_D sz = dinocv_set_size(width, height);
	
	cd = (CASCADED_DETECTOR_D *)malloc(sizeof(CASCADED_DETECTOR_D));
	memset(cd, 0, sizeof(CASCADED_DETECTOR_D));

	// parameter settings
	cd->parm_sub_window_width = sub_window_width;
	cd->parm_sub_window_height = sub_window_height;
	cd->parm_initial_scale_factor_index = initial_scale_factor_index;
	cd->parm_scale_factor = scale_factor;
	cd->parm_n_lut_sf = n_lut_sf;
	cd->parm_detection_result_buffer_size = detection_result_buffer_size;

	// memory allocation
	cd->lut_wsize_consider_sf = (unsigned int *)malloc(sizeof(unsigned int) * n_lut_sf);
	memset(cd->lut_wsize_consider_sf, 0, sizeof(unsigned int) * n_lut_sf);
	cd->lut_hsize_consider_sf = (unsigned int *)malloc(sizeof(unsigned int) * n_lut_sf);
	memset(cd->lut_hsize_consider_sf, 0, sizeof(unsigned int) * n_lut_sf);
	cd->lut_mv_sf = (int *)malloc(sizeof(int) * n_lut_sf);
	memset(cd->lut_mv_sf, 0, sizeof(int) * n_lut_sf);

	// load lut of sf
	float sf=1.;
	int delta=1;
	for(int i = 0 ; i < n_lut_sf ; i++)
	{
		cd->lut_wsize_consider_sf[i]	= (int)d_round(sub_window_width*sf);
		cd->lut_hsize_consider_sf[i]	= (int)d_round(sub_window_height*sf);
		cd->lut_mv_sf[i]				= (int)(delta*sf);
		sf*=scale_factor;
	}

	cd->detection_result = (DETECTION_RESULT *)malloc(sizeof(DETECTION_RESULT));
	memset(cd->detection_result, 0, sizeof(DETECTION_RESULT));
	cd->detection_result->p_rt = (RECT_D *)malloc(sizeof(RECT_D) * detection_result_buffer_size);;
	memset(cd->detection_result->p_rt, 0, sizeof(RECT_D) * detection_result_buffer_size);

	cd->merged_detection_result = (DETECTION_RESULT *)malloc(sizeof(DETECTION_RESULT));
	memset(cd->merged_detection_result, 0, sizeof(DETECTION_RESULT));
	cd->merged_detection_result->p_rt = (RECT_D *)malloc(sizeof(RECT_D) * detection_result_buffer_size);;
	memset(cd->merged_detection_result->p_rt, 0, sizeof(RECT_D) * detection_result_buffer_size);


	


	//is >> cd.n_sc;
#ifdef _WIN32
	fscanf_s(fp, "%d", &cd->n_sc);
#else
	fscanf(fp, "%d", &cd->n_sc);
#endif
	cd->p_sc = (STRONG_CLASSIFIER_D *)malloc(sizeof(STRONG_CLASSIFIER_D) * cd->n_sc);
	memset(cd->p_sc, 0, sizeof(STRONG_CLASSIFIER_D) * cd->n_sc);
	for(int i = 0 ; i < cd->n_sc ; i++)
	{
		//is >> cd->p_sc[i].n_wc >> cd->p_sc[i].threshold;
#ifdef _WIN32
		fscanf_s(fp, "%d %f", &cd->p_sc[i].n_wc, &cd->p_sc[i].threshold);
#else
		fscanf(fp, "%d %f", &cd->p_sc[i].n_wc, &cd->p_sc[i].threshold);
#endif
		cd->p_sc[i].p_wc = (WEAK_CLASSIFIER_D *)malloc(sizeof(WEAK_CLASSIFIER_D) * cd->p_sc[i].n_wc);
		memset(cd->p_sc[i].p_wc, 0, sizeof(WEAK_CLASSIFIER_D) * cd->p_sc[i].n_wc);

		

		for(int j = 0 ; j < cd->p_sc[i].n_wc ; j++)
		{
			//malloc
			cd->p_sc[i].p_wc[j].fhlf.reversed_square_scale_factor = (float *)malloc(sizeof(float)*n_lut_sf);
			memset(cd->p_sc[i].p_wc[j].fhlf.reversed_square_scale_factor, 0, sizeof(float)*n_lut_sf);

			cd->p_sc[i].p_wc[j].fhlf.p_hb = (HAAR_BLOCK_D **)malloc(sizeof(HAAR_BLOCK_D *)*n_lut_sf);
			memset(cd->p_sc[i].p_wc[j].fhlf.p_hb, 0, sizeof(HAAR_BLOCK_D *)*n_lut_sf);

			cd->p_sc[i].p_wc[j].fhlf.area = (int *)malloc(sizeof(int) * n_lut_sf);
			memset(cd->p_sc[i].p_wc[j].fhlf.area, 0, sizeof(int) * n_lut_sf);


			//is >> cd->p_sc[i].p_wc[j].fhlf.n_hb >> cd->p_sc[i].p_wc[j].fhlf.hs._w >> cd->p_sc[i].p_wc[j].fhlf.hs._h;
#ifdef _WIN32
			fscanf_s(fp, "%d %d %d", &cd->p_sc[i].p_wc[j].fhlf.n_hb, &cd->p_sc[i].p_wc[j].fhlf.hs._w, &cd->p_sc[i].p_wc[j].fhlf.hs._h);
#else
			fscanf(fp, "%d %d %d", &cd->p_sc[i].p_wc[j].fhlf.n_hb, &cd->p_sc[i].p_wc[j].fhlf.hs._w, &cd->p_sc[i].p_wc[j].fhlf.hs._h);
#endif
			
			cd->p_sc[i].p_wc[j].fhlf.p_hb[0] = (HAAR_BLOCK_D *)malloc(sizeof(HAAR_BLOCK_D)*cd->p_sc[i].p_wc[j].fhlf.n_hb);
			for(int l = 0 ; l < cd->p_sc[i].p_wc[j].fhlf.n_hb ; l++)
			{
#ifdef _WIN32
				fscanf_s(fp, "%d %d %d %d %d", &cd->p_sc[i].p_wc[j].fhlf.p_hb[0][l].x,
					&cd->p_sc[i].p_wc[j].fhlf.p_hb[0][l].y,
					&cd->p_sc[i].p_wc[j].fhlf.p_hb[0][l].w,
					&cd->p_sc[i].p_wc[j].fhlf.p_hb[0][l].h,
					&cd->p_sc[i].p_wc[j].fhlf.p_hb[0][l].weight);
#else
				fscanf(fp, "%d %d %d %d %d", &cd->p_sc[i].p_wc[j].fhlf.p_hb[0][l].x,
					&cd->p_sc[i].p_wc[j].fhlf.p_hb[0][l].y,
					&cd->p_sc[i].p_wc[j].fhlf.p_hb[0][l].w,
					&cd->p_sc[i].p_wc[j].fhlf.p_hb[0][l].h,
					&cd->p_sc[i].p_wc[j].fhlf.p_hb[0][l].weight);
#endif
				

			}
#ifdef _WIN32
			fscanf_s(fp, "%f %d %d", &cd->p_sc[i].p_wc[j].alpha, &cd->p_sc[i].p_wc[j].polarity, &cd->p_sc[i].p_wc[j].threshold);
#else
			fscanf(fp, "%f %d %d", &cd->p_sc[i].p_wc[j].alpha, &cd->p_sc[i].p_wc[j].polarity, &cd->p_sc[i].p_wc[j].threshold);
#endif

			// setting haar-like feature area
			cd->p_sc[i].p_wc[j].fhlf.area[0] = cd->p_sc[i].p_wc[j].fhlf.p_hb[0][0].w * cd->p_sc[i].p_wc[j].fhlf.p_hb[0][0].h;

			// setting scalable detector data
			cd->p_sc[i].p_wc[j].fhlf.reversed_square_scale_factor[0] = 1.0;
			sf=1.;
			HAAR_SHAPE_D hs = cd->p_sc[i].p_wc[j].fhlf.hs;
			HAAR_BLOCK_D hb = cd->p_sc[i].p_wc[j].fhlf.p_hb[0][0];
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

					if( cx+cw > d_round(cd->parm_sub_window_width*sf) ) // x? ì™?™å ?™ì˜™ adt? ì™??? ì™?™å ?™ì˜™ ? ì™?™å ?™ì˜™è¦‹åº¸ï¿?hlf? ì™??? ì™?™å ?™ì˜™? ì™?™å ?™ì˜™? ì™??? ì™?™å ?™ì˜™? ì™?™å ?™ì˜™? ì™??? ì™??
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

					if( cy+ch > d_round(cd->parm_sub_window_height*sf) ) // y? ì™?™å ?™ì˜™ adt? ì™??? ì™?™å ?™ì˜™ ? ì™?™å ?™ì˜™è¦‹åº¸ï¿?hlf? ì™??? ì‹£ë¤„ì˜™? ì™?™å ?™ì˜™? ì™??? ì™?™å ?™ì˜™? ì™?™å ?™ì˜™? ì™??? ì™??
					{
						if(cy>=adt)
							cy-=adt;
						else
							ch-=hs._h;
					}
				}

				cd->p_sc[i].p_wc[j].fhlf.reversed_square_scale_factor[k] = 1.f/(((float)cw/hb.w)*((float)ch/hb.h));
					
				
				HAAR_LIKE_FEATURE_D thlf;
				create_haar_like_feature(thlf, hs, cx, cy, cw, ch);

				cd->p_sc[i].p_wc[j].fhlf.p_hb[k] = (HAAR_BLOCK_D *)malloc(sizeof(HAAR_BLOCK_D)*cd->p_sc[i].p_wc[j].fhlf.n_hb);
				memcpy(cd->p_sc[i].p_wc[j].fhlf.p_hb[k], thlf.p_hb, sizeof(HAAR_BLOCK_D)*cd->p_sc[i].p_wc[j].fhlf.n_hb);

				// setting haar-like feature area
				cd->p_sc[i].p_wc[j].fhlf.area[k] = cd->p_sc[i].p_wc[j].fhlf.p_hb[k][0].w * cd->p_sc[i].p_wc[j].fhlf.p_hb[k][0].h;
			}
			
		}
	}

	// GUI Support
	cd->n_sc_max = cd->n_sc;

	fclose(fp);
	return cd;
}

void release_cascaded_detector(CASCADED_DETECTOR_D *cd)
{
	// Release Look Up Table of width for each scale
	if(cd->lut_wsize_consider_sf)
		free(cd->lut_wsize_consider_sf);

	// Release Look Up Table of height for each scale
	if(cd->lut_hsize_consider_sf)
		free(cd->lut_hsize_consider_sf);

	// Release Look Up Table of scale factor (related SWO movement)
	if(cd->lut_mv_sf)
		free(cd->lut_mv_sf);

	// Release Result Structure
	if(cd->detection_result)
	{
		if(cd->detection_result->p_rt)
			free(cd->detection_result->p_rt);
		free(cd->detection_result);
	}

	if(cd->merged_detection_result)
	{
		if(cd->merged_detection_result->p_rt)
			free(cd->merged_detection_result->p_rt);
		free(cd->merged_detection_result);
	}

	// Release Cascaded Detector
	for(int i = 0 ; i < cd->n_sc_max ; i++)
	{
		for(int j = 0 ; j < cd->p_sc[i].n_wc ; j++)
		{
			for(int k = 0 ; k < cd->parm_n_lut_sf ; k++)
			{
				free(cd->p_sc[i].p_wc[j].fhlf.p_hb[k]);
			}
			free(cd->p_sc[i].p_wc[j].fhlf.reversed_square_scale_factor);
			free(cd->p_sc[i].p_wc[j].fhlf.p_hb);
			free(cd->p_sc[i].p_wc[j].fhlf.area);
		}
		free(cd->p_sc[i].p_wc);
	}

	free(cd->p_sc);
	cd = NULL;
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

int cascaded_classifier(CASCADED_DETECTOR_D *cd, int x, int y, int **ii, int sf_idx)
{
	for(int i = 1 ; i <= cd->n_sc ; i++)
	{
		if(!strong_classifier(cd->p_sc[i-1], x, y, ii, sf_idx))
		{
			return FALSE;
		}
	}

	return TRUE;
}

void cascaded_classify(CASCADED_DETECTOR_D *cd, IMAGE_D *img, int **ii)
{
	register int x, y;
	unsigned int width, height;
	
	int n_objects=0;

	RECT_D rt;

	cd->detection_result->n_objects=0;
	memset(cd->detection_result->p_rt, 0, cd->parm_detection_result_buffer_size);
	cd->merged_detection_result->n_objects=0;
	memset(cd->merged_detection_result->p_rt, 0, cd->parm_detection_result_buffer_size);

	int sf_idx = cd->parm_initial_scale_factor_index;
	int mv = cd->lut_mv_sf[sf_idx];

	width = cd->lut_wsize_consider_sf[sf_idx];
	height = cd->lut_hsize_consider_sf[sf_idx];

	while( width <= img->width && height <= img->height )
	{
		for(y = 0 ; y <= (int)img->height-(int)height ; y+=mv)
		{
			for(x = 0 ; x <= (int)img->width-(int)width ; x+=mv)
			{
				if(cascaded_classifier(cd, x, y, ii, sf_idx))
				{
					rt.left = x;
					rt.top = y;
					rt.right = x+width;
					rt.bottom = y+height;

					cd->detection_result->p_rt[n_objects++] = rt;
				}
			}
		}

		sf_idx++;
		mv = cd->lut_mv_sf[sf_idx];
		width = cd->lut_wsize_consider_sf[sf_idx];
		height = cd->lut_hsize_consider_sf[sf_idx];
	}

	cd->detection_result->n_objects = n_objects;


	merge_rect(cd, n_objects);
	
	return;
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

void merge_rect(CASCADED_DETECTOR_D *cd, int n_objects)
{
	register int i, j;
	int *labels, *counter;
	int group_id=0;
	int center_x, center_y, search_area;
	int cx, cy;

	RECT_D *prt = cd->detection_result->p_rt;
	RECT_D *r_prt = cd->merged_detection_result->p_rt;

	if(n_objects <= 1)
	{
		memcpy(r_prt, prt, sizeof(RECT_D) * cd->detection_result->n_objects);
		cd->merged_detection_result->n_objects = cd->detection_result->n_objects;
	}


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

	cd->merged_detection_result->n_objects = group_id;

	counter = (int *)malloc(sizeof(int)*group_id);
	memset(counter, 0, sizeof(int)*group_id);

	/* average of all rectangle
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
	*/

	/* selection minimum size rectangle */
	unsigned int minimum_length;
	for(i = 0 ; i < group_id ; i++)
	{
		minimum_length=UINT_MAX;
		for(j = 0 ; j < n_objects ; j++)
		{
			if(labels[j] == i+1)
			{
				if(minimum_length > prt[j].right-prt[j].left)
				{
					minimum_length  = prt[j].right-prt[j].left;
					r_prt[i].left = prt[j].left;
					r_prt[i].top = prt[j].top;
					r_prt[i].right = prt[j].right;
					r_prt[i].bottom = prt[j].bottom;
				}
			}
		}



	}

	free(labels);
	free(counter);

}

void cascaded_classify_with_pso(CASCADED_DETECTOR_D *cd, RECT_D *rt, IMAGE_D *img, int **ii, int num_particles, int num_stage)
{
	int n_objects=0;

	cd->detection_result->n_objects=0;
	memset(cd->detection_result->p_rt, 0, cd->parm_detection_result_buffer_size);
	cd->merged_detection_result->n_objects=0;
	memset(cd->merged_detection_result->p_rt, 0, cd->parm_detection_result_buffer_size);

	int sf_idx = cd->parm_initial_scale_factor_index;
	int mv = cd->lut_mv_sf[sf_idx];

	unsigned int width = (*rt).right - (*rt).left-1;
	unsigned int height = (*rt).bottom - (*rt).top-1;
	create_swarm(cd, (*rt).left, (*rt).top, (*rt).right, (*rt).bottom, num_particles, num_stage,  sf_idx);
	while( cd->lut_wsize_consider_sf[sf_idx] < width && cd->lut_hsize_consider_sf[sf_idx] < height )
	{
		if(init_swarm(cd, (*rt).left, (*rt).top, (*rt).right, (*rt).bottom, num_particles, num_stage,  sf_idx))
		{
			while( NextStage(cd, ii, sf_idx, cd->detection_result, &n_objects, rt) );
		}

		sf_idx++;
	}

	release_swarm(cd->pso_parm);

	cd->detection_result->n_objects = n_objects;


	/**/
	//if(n_objects > 1)
		merge_rect(cd, n_objects);
}

int cascaded_classifier_with_pso(CASCADED_DETECTOR_D *cd, int x, int y, int **ii, int sf_idx)
{
	for(int i = 1 ; i <= cd->n_sc ; i++)
	{
		if(!strong_classifier(cd->p_sc[i-1], x, y, ii, sf_idx))
		{
			return cd->n_sc-i;
		}
	}

	return 0;
}

bool create_swarm(CASCADED_DETECTOR_D *cd, int pos_min_x, int pos_min_y, int pos_max_x, int pos_max_y, int num_particles, int num_stage, int sf_idx)
{
	if(!cd)
		return false;

	PSO_PARM_D *pso_parm = (PSO_PARM_D *)malloc(sizeof(PSO_PARM_D));
	memset(pso_parm, 0, sizeof(PSO_PARM_D));	

	pso_parm->currentScore = (double *)malloc(sizeof(double) * num_particles);
	memset(pso_parm->currentScore, 0, sizeof(double) * num_particles);
	pso_parm->pbestScore = (double *)malloc(sizeof(double) * num_particles);
	memset(pso_parm->pbestScore, 0, sizeof(double) * num_particles);
	pso_parm->pbestPosition = (POINT_D *)malloc(sizeof(POINT_D) * num_particles);
	memset(pso_parm->pbestPosition, 0, sizeof(POINT_D) * num_particles);
	pso_parm->currentVelocity = (POINT_D *)malloc(sizeof(POINT_D) * num_particles);
	memset(pso_parm->currentVelocity, 0, sizeof(POINT_D) * num_particles);
	pso_parm->currentPosition = (POINT_D *)malloc(sizeof(POINT_D) * num_particles);
	memset(pso_parm->currentPosition, 0, sizeof(POINT_D) * num_particles);

	INIT_WELL512();

	cd->pso_parm = pso_parm;
	return true;
}

bool init_swarm(CASCADED_DETECTOR_D *cd, int pos_min_x, int pos_min_y, int pos_max_x, int pos_max_y, int num_particles, int num_stage, int sf_idx)
{
	if(!cd || !cd->pso_parm)
		return false;

	PSO_PARM_D *pso_parm = cd->pso_parm;

	pso_parm->pos_max_x = pos_max_x;
	pso_parm->pos_max_y = pos_max_y;
	pso_parm->num_particles = num_particles;
	pso_parm->num_stage = num_stage;

	pso_parm->_local_stage = 0;
	pso_parm->gbestIndex = INT_MAX;
	pso_parm->gbestScore = DBL_MAX;

	for(int i = 0; i < num_particles; i++)
	{
		pso_parm->currentVelocity[i].x = WELLRNG512_limit(-VELOCITY_MAX_X, VELOCITY_MAX_X);
		pso_parm->currentVelocity[i].y = WELLRNG512_limit(-VELOCITY_MAX_Y, VELOCITY_MAX_Y);
		pso_parm->currentPosition[i].x =  WELLRNG512_limit(pos_min_x, pos_max_x - cd->lut_wsize_consider_sf[sf_idx]-1);
		pso_parm->currentPosition[i].y = WELLRNG512_limit(pos_min_y, pos_max_y - cd->lut_hsize_consider_sf[sf_idx]-1);
		pso_parm->pbestScore[i] = DBL_MAX;
	}

	return true;
}

void release_swarm(PSO_PARM_D *pso_parm)
{
	free(pso_parm->currentScore);
	free(pso_parm->pbestScore);

	free(pso_parm->pbestPosition);
	free(pso_parm->currentVelocity);
	free(pso_parm->currentPosition);

	free(pso_parm);
}


bool NextStage(CASCADED_DETECTOR_D *cd, int **ii, int sf_idx, DETECTION_RESULT *detection_result, int *n_objects, RECT_D *boundary)
{
	//IMAGE_D *result = dinocv_load_image(INPUT_FILE_NAME, &dinocv_set_size(img->width, img->height), 24, IMGMDL_RGB);
	PSO_PARM_D *pso_parm = cd->pso_parm;
	int numCorrectDetection;

	RECT_D rt;

	if(pso_parm->_local_stage == 0)
	{
		/*
		DrawParticles(result, sf_idx);
		dinocv_save_bitmap("result.bmp", result);
		dinocv_release_image(result);
		printf("draw particles\n");
		getchar();
		*/

		pso_parm->_local_stage++;
		return true;
	}

	numCorrectDetection = EvaluatePosition(cd, ii, sf_idx);
	CalculateVelocity(cd->pso_parm);
	CalculatePosition(cd->pso_parm);

	/*
	DrawParticles(result, sf_idx);
	dinocv_save_bitmap("result.bmp", result);
	dinocv_release_image(result);
	printf("draw particles\n");
	getchar();
	*/
	
	pso_parm->_local_stage++;

	//if(gbestScore < CORN_IMAGE_WIDTH)
	//printf("numCorrectDetection=%d\n", numCorrectDetection);
	if(numCorrectDetection > pso_parm->numCorrectDetection)
	{
		/*
		result = dinocv_load_image(INPUT_FILE_NAME, &dinocv_set_size(img->width, img->height), 24, IMGMDL_RGB);		
		
		dinocv_draw_rect(result, &dinocv_set_rect(pbestPosition[gbestIndex].x, pbestPosition[gbestIndex].y,
			pbestPosition[gbestIndex].x+lut_wsize_consider_sf[sf_idx], pbestPosition[gbestIndex].y+lut_hsize_consider_sf[sf_idx]),
			&dinocv_set_color(255,0,0), 3);
		dinocv_save_bitmap("result.bmp", result);
		dinocv_release_image(result);
		*/

		rt.left = pso_parm->pbestPosition[pso_parm->gbestIndex].x;
		rt.top = pso_parm->pbestPosition[pso_parm->gbestIndex].y;
		rt.right = rt.left + cd->lut_wsize_consider_sf[sf_idx];
		rt.bottom = rt.top + cd->lut_hsize_consider_sf[sf_idx];

		if(rt.left >= boundary->left &&
			rt.top >= boundary->top &&
			rt.right <= boundary->right &&
			rt.bottom <= boundary->bottom)
		{
			cd->detection_result->p_rt[(*n_objects)++] = rt;
		}

		//printf(" @ Coordinates\t=(%d, %d)\t\tStage=%d\n", pso_parm->pbestPosition[pso_parm->gbestIndex].x, pso_parm->pbestPosition[pso_parm->gbestIndex].y, (int)(--pso_parm->_local_stage) );
		return false;
	}
	else if(pso_parm->_local_stage > pso_parm->num_stage)
	{
		//printf(" # Object was not found !!\n");
		return false;
	}

	return true;
}


int EvaluatePosition(CASCADED_DETECTOR_D *cd, int **ii, int sf_idx)
{
	PSO_PARM_D *pso_parm = cd->pso_parm;
	int sub_width = cd->lut_wsize_consider_sf[sf_idx];
	int sub_height = cd->lut_hsize_consider_sf[sf_idx];
	int numCorrectDetection=0;

	for(int i = 0; i < pso_parm->num_particles; i++)
	{
		// Evaluate
		if(pso_parm->currentPosition[i].x > pso_parm->pos_max_x-sub_width)
			pso_parm->currentPosition[i].x = pso_parm->pos_max_x-sub_width-1;
		if(pso_parm->currentPosition[i].y > pso_parm->pos_max_y-sub_height)
			pso_parm->currentPosition[i].y = pso_parm->pos_max_y-sub_height-1;

		pso_parm->currentScore[i] = cascaded_classifier_with_pso(cd, cd->pso_parm->currentPosition[i].x, cd->pso_parm->currentPosition[i].y, ii, sf_idx);
		numCorrectDetection += pso_parm->currentScore[i] == 0. ? 1 : 0;
		// Renew pbest
		if(pso_parm->pbestScore[i] > pso_parm->currentScore[i])
		{
			pso_parm->pbestScore[i] = pso_parm->currentScore[i];
			pso_parm->pbestPosition[i] = pso_parm->currentPosition[i];

			// Renew gbest
			if(pso_parm->gbestScore > pso_parm->currentScore[i])
			{
				pso_parm->gbestScore = pso_parm->currentScore[i];
				pso_parm->gbestIndex = i;
			}
		}
	}
	
	return numCorrectDetection;
}


void CalculateVelocity(PSO_PARM_D *pso_parm)
{
	srand((unsigned int) time(NULL));

	for(int i = 0; i < pso_parm->num_particles; i++)
	{
		pso_parm->currentVelocity[i].x = (long_d) (INERTIA_COEFFICIENT * pso_parm->currentVelocity[i].x
			+ PARTICLE_COEFFICIENT * ((double) rand() / RAND_MAX) * (pso_parm->pbestPosition[i].x - pso_parm->currentPosition[i].x)
			+ SWARM_COEFFICIENT * ((double) rand() / RAND_MAX) * (pso_parm->pbestPosition[pso_parm->gbestIndex].x - pso_parm->currentPosition[i].x));

		pso_parm->currentVelocity[i].y = (long_d) (INERTIA_COEFFICIENT * pso_parm->currentVelocity[i].y
			+ PARTICLE_COEFFICIENT * ((double) rand() / RAND_MAX) * (pso_parm->pbestPosition[i].y - pso_parm->currentPosition[i].y)
			+ SWARM_COEFFICIENT * ((double) rand() / RAND_MAX) * (pso_parm->pbestPosition[pso_parm->gbestIndex].y - pso_parm->currentPosition[i].y));

		if(pso_parm->currentVelocity[i].x > VELOCITY_MAX_X)
			pso_parm->currentVelocity[i].x = VELOCITY_MAX_X;
		else if(pso_parm->currentVelocity[i].x < -VELOCITY_MAX_X)
			pso_parm->currentVelocity[i].x = -VELOCITY_MAX_X;
		
		if(pso_parm->currentVelocity[i].y > VELOCITY_MAX_Y)
			pso_parm->currentVelocity[i].y = VELOCITY_MAX_Y;
		else if(pso_parm->currentVelocity[i].y < -VELOCITY_MAX_Y)
			pso_parm->currentVelocity[i].y = -VELOCITY_MAX_Y;
	}
}


void CalculatePosition(PSO_PARM_D *pso_parm)
{
	for(int i = 0; i < pso_parm->num_particles; i++)
	{
		pso_parm->currentPosition[i].x += pso_parm->currentVelocity[i].x;
		pso_parm->currentPosition[i].y += pso_parm->currentVelocity[i].y;

		if(pso_parm->currentPosition[i].x < 0)
			pso_parm->currentPosition[i].x = 0;
		else if(pso_parm->currentPosition[i].x > pso_parm->pos_max_x)
			pso_parm->currentPosition[i].x = pso_parm->pos_max_x -  1;

		if(pso_parm->currentPosition[i].y < 0)
			pso_parm->currentPosition[i].y = 0;
		else if(pso_parm->currentPosition[i].y > pso_parm->pos_max_y)
			pso_parm->currentPosition[i].y = pso_parm->pos_max_y -  1;
	}
}

void DrawParticles(CASCADED_DETECTOR_D *cd, IMAGE_D *img, int sf_idx)
{
	PSO_PARM_D *pso_parm = cd->pso_parm;
	int l,t,r,b;

	int local_width = cd->lut_wsize_consider_sf[sf_idx];
	int local_height = cd->lut_hsize_consider_sf[sf_idx];

	for(int i = 0; i < cd->pso_parm->num_particles; i++)
	{
		l = pso_parm->currentPosition[i].x;
		t = pso_parm->currentPosition[i].y;
		r = pso_parm->currentPosition[i].x + local_width;
		b = pso_parm->currentPosition[i].y + local_height;
		dinocv_draw_rect(img, &dinocv_set_rect(l,t,r,b), &dinocv_set_color(0,0,255), 1);
	}
}

void candidate_list_update(CASCADED_DETECTOR_D *cd, IMAGE_D *img, LIST_D *cl, LIST_D *tl, int **ii, int n_particles, int n_stages)
{
	RECT_D *result_rt_arr;
	int x, y;

	int cc = cl->cnt;
	for(int ci = cc-1 ; ci >= 0 ; ci--)
	{
		// Searching Area with PSO & Renewing data (if exist, miss_cnt++, else detection_cnt++)
		CANDIDATE_OBJECT *co = (CANDIDATE_OBJECT *)soc_list_get_idx_data(cl, ci);
		cascaded_classify_with_pso(cd,
			&dinocv_set_rect(
			(uint_d)d_clp_boundary(co->x-co->w, 0, img->width-1), (uint_d)d_clp_boundary(co->y-co->h, 0, img->height-1),
			(uint_d)d_clp_boundary(co->x+co->w, 0, img->width-1), (uint_d)d_clp_boundary(co->y+co->h, 0, img->height-1)
			),
			img, ii, n_particles, n_stages);

		result_rt_arr = cd->merged_detection_result->p_rt;

		double min_length = MAX_TRACKING_LENGTH;
		for(int i = 0 ; i < cd->merged_detection_result->n_objects ; i++)
		{
			x = (result_rt_arr[i].left+result_rt_arr[i].right)>>1;
			y = (result_rt_arr[i].top+result_rt_arr[i].bottom)>>1;

			double length;// = sqrt(pow(co->x-x, 2)+pow(co->y-y, 2) );
			length = ( d_abs(co->x-x) + d_abs(co->y-y) ) >> 1;
			if(length < min_length)
			{
				min_length = length;
				co->x = x;
				co->y = y;
				co->w = (result_rt_arr[i].right - result_rt_arr[i].left);
				co->h = (result_rt_arr[i].bottom - result_rt_arr[i].top);
				co->detection_cnt++;
			}
		}
		if(min_length >= MAX_TRACKING_LENGTH)
		{
			if(++co->miss_cnt > CANDIDATE_MISS_MAX)
			{
				free((CANDIDATE_OBJECT *)soc_list_del_idx_data(cl, ci));
				printf("\t - candidate object deleted..(%d)\n", cl->cnt);
			}
		}
		else if(co->detection_cnt >= CANDIDATE_CNT_MAX) // send to tracking list (detection_cnt >= CANDIDATE_CNT_MAX)
		{
			// remove overlapping object
			CANDIDATE_OBJECT *co = (CANDIDATE_OBJECT *)soc_list_del_idx_data(cl, ci);
			TRACKING_OBJECT *to = (TRACKING_OBJECT *)malloc(sizeof(TRACKING_OBJECT));
			memset(to, 0, sizeof(TRACKING_OBJECT));
			to->x_sum = to->x_arr[0] = co->x;
			to->y_sum = to->y_arr[0] = co->y;
			to->w_sum = to->w_arr[0] = co->w;
			to->h_sum = to->h_arr[0] = co->h;
			to->pos_cidx = 1;
			to->length_cidx = 1;

			to->len_division_cnt=1;
			to->pos_division_cnt=1;

			free(co);

			soc_list_add_head(tl, to);

			printf(" + tracking object added..(%d), and current candidate object is (%d)\n", tl->cnt, cl->cnt);

		}
	}
}

void tracking_list_update(CASCADED_DETECTOR_D *cd, IMAGE_D *img, LIST_D *cl, LIST_D *tl, int **ii, int n_particles, int n_stages)
{
	RECT_D *result_rt_arr = cd->merged_detection_result->p_rt;
	double min_length;
	int x, y;

	for(int ti = tl->cnt-1 ; ti >= 0 ; ti--)
	{
		// Searching Area with PSO & Renewing data (if exist, miss_cnt++, else detection_cnt++)
		TRACKING_OBJECT *to = (TRACKING_OBJECT *)soc_list_get_idx_data(tl, ti);
		cascaded_classify_with_pso(cd,
			&dinocv_set_rect(
			d_clp_boundary(TO_GET_AVG_X(to) - TO_GET_AVG_W(to), 0, img->width-1), d_clp_boundary(TO_GET_AVG_Y(to) - TO_GET_AVG_H(to), 0, img->height-1),
			d_clp_boundary(TO_GET_AVG_X(to) + TO_GET_AVG_W(to), 0, img->width-1), d_clp_boundary(TO_GET_AVG_Y(to) + TO_GET_AVG_H(to), 0, img->height-1)
			),
			img, ii, n_particles, n_stages);

		min_length = MAX_TRACKING_LENGTH;
		for(int i = 0 ; i < cd->merged_detection_result->n_objects ; i++)
		{
			x = (result_rt_arr[i].left + result_rt_arr[i].right)>>1;
			y = (result_rt_arr[i].top + result_rt_arr[i].bottom)>>1;

			double length;// = sqrt(pow(TO_GET_AVG_X(to)-x, 2)+pow(TO_GET_AVG_Y(to)-y, 2) );
			length = ( d_abs(TO_GET_AVG_X(to)-x) + d_abs(TO_GET_AVG_Y(to)-y) ) >> 1;

			if(length < min_length)
			{
				min_length = length;
				// delete previous value
				to->x_sum -= to->x_arr[to->pos_cidx];
				to->y_sum -= to->y_arr[to->pos_cidx];
				to->w_sum -= to->w_arr[to->length_cidx];
				to->h_sum -= to->h_arr[to->length_cidx];

				// add current result to array
				to->x_sum += to->x_arr[to->pos_cidx] = x;
				to->y_sum += to->y_arr[to->pos_cidx] = y;
				to->w_sum += to->w_arr[to->length_cidx] = (result_rt_arr[i].right - result_rt_arr[i].left);
				to->h_sum += to->h_arr[to->length_cidx] = (result_rt_arr[i].bottom - result_rt_arr[i].top);

				if(to->len_division_cnt < TRACKING_LENGTH_ARR_MAX) to->len_division_cnt++;
				if(to->pos_division_cnt < TRACKING_POS_ARR_MAX) to->pos_division_cnt++;
				to->length_cidx = (++to->length_cidx) % TRACKING_LENGTH_ARR_MAX;
				to->pos_cidx = (++to->pos_cidx) % TRACKING_POS_ARR_MAX;

				to->miss_cnt=0;

				to->cur_x = x;
				to->cur_y = y;
			}
		}
		if(min_length == MAX_TRACKING_LENGTH)
		{
			if(++to->miss_cnt > TRACKING_MISS_MAX)
			{
				free((TRACKING_OBJECT *)soc_list_del_idx_data(tl, ti));
				printf(" - tracking object deleted..(%d)\n", tl->cnt);
			}
		}
	}

	for(int ti = 0 ; ti < tl->cnt-1 ; ti++)
	{
		for(int ti2 = ti+1 ; ti2 < tl->cnt ; ti2++)
		{
			if(ti != ti2)
			{
				TRACKING_OBJECT *to = (TRACKING_OBJECT *)soc_list_get_idx_data(tl, ti);
				if(to->cur_x > 0)
				{
					TRACKING_OBJECT *to2 = (TRACKING_OBJECT *)soc_list_get_idx_data(tl, ti2);

					if(to2->cur_x > 0 &&
						d_limit_cc(to->cur_x / to2->cur_x, 0.8, 1.2) &&
						d_limit_cc(to->cur_y / to2->cur_y, 0.8, 1.2))
					{
						to2->cur_x = -1;
					}
				}
			}
		}
	}
	for(int ti = tl->cnt-1 ; ti >= 0 ; ti--)
	{
		TRACKING_OBJECT *to = (TRACKING_OBJECT *)soc_list_get_idx_data(tl, ti);
		if(to->cur_x < 0)
			free((TRACKING_OBJECT *)soc_list_del_idx_data(tl, ti));
	}
}
void push_candidate(LIST_D *cl, RECT_D *rt)
{
	// push candidate list
	CANDIDATE_OBJECT *co = (CANDIDATE_OBJECT *)malloc(sizeof(CANDIDATE_OBJECT));
	memset(co, 0, sizeof(CANDIDATE_OBJECT));
	co->x = (rt->left + rt->right)>>1;
	co->y = (rt->top + rt->bottom)>>1;
	co->w = rt->right - rt->left;
	co->h = rt->bottom - rt->top;
	co->detection_cnt++;
	soc_list_add_head(cl, (void *)co);
	printf("\t + candidate object added..(%d)\n", cl->cnt);
}

void candidate_add(CASCADED_DETECTOR_D *cd, IMAGE_D *img, LIST_D *cl, LIST_D *tl, int **ii, int n_particles, int n_stages)
{
	RECT_D *result_rt_arr;
	int x, y;

	// Swipe TO Area
	for(int ti = 0 ; ti < tl->cnt ; ti++)
	{
		TRACKING_OBJECT *to = (TRACKING_OBJECT *)soc_list_get_idx_data(tl, ti);
		int t = (uint_d)(TO_GET_AVG_Y(to)-((TO_GET_AVG_H(to)>>1)));
		int b = (uint_d)(TO_GET_AVG_Y(to)+((TO_GET_AVG_H(to)>>1)));
		int l = (uint_d)(TO_GET_AVG_X(to)-((TO_GET_AVG_W(to)>>1)));
		int w = (uint_d)(TO_GET_AVG_W(to));
		if(t < 0) t = 0;
		if(b > img->height) b = img->height;
		for(t ; t < b ; t++)
		{
			memset(&img->source[t][l], 0, sizeof(uchar_d) * w);
		}
	}

	// Swipe CO Area
	for(int ci = 0 ; ci < cl->cnt ; ci++)
	{
		CANDIDATE_OBJECT *co = (CANDIDATE_OBJECT *)soc_list_get_idx_data(cl, ci);
		int t = co->y-(co->h>>1);
		int b = co->y+(co->h>>1);
		int l = co->x-(co->w>>1);
		int w = co->w;
		for(t ; t < b ; t++)
		{
			memset(&img->source[t][l], 0, sizeof(uchar_d) * w);
		}
	}

	// Cascaded classify with PSO
	cascaded_classify_with_pso(cd, &dinocv_set_rect(0, 0, img->width, img->height),
		img, ii, n_particles, n_stages);
	

	result_rt_arr = cd->merged_detection_result->p_rt;
	for(int j = 0 ; j < cd->merged_detection_result->n_objects ; j++)
	{
		// Push this object to candidate list when current object not overlapped with object of tracking list
		x=((result_rt_arr[j].left + result_rt_arr[j].right)>>1);
		y=((result_rt_arr[j].top + result_rt_arr[j].bottom)>>1);

		
		for(int ci = 0 ; ci < cl->cnt ; ci++)
		{
			CANDIDATE_OBJECT *co = (CANDIDATE_OBJECT *)soc_list_get_idx_data(cl, ci);
			double length;// = sqrt(pow(co->x-x, 2)+pow(co->y-y, 2) );
			length = ( d_abs(co->x-x) + d_abs(co->y-y) ) >> 1;

			if((int)length < (co->w>>1) )
				return;
		}

		for(int ti = 0 ; ti < tl->cnt ; ti++)
		{
			TRACKING_OBJECT *to = (TRACKING_OBJECT *)soc_list_get_idx_data(tl, ti);
			double length;// = sqrt(pow(TO_GET_AVG_X(to)-x, 2)+pow(TO_GET_AVG_Y(to)-y, 2) );
			length = ( d_abs(TO_GET_AVG_X(to)-x) + d_abs(TO_GET_AVG_Y(to)-y) ) >> 1;

			if((int)length < (TO_GET_AVG_W(to)>>1) )
				return;
		}

		push_candidate(cl, &result_rt_arr[j]);
	}
}

void cascaded_classify_with_tpso(IMAGE_D *img, CASCADED_DETECTOR_D *cd, LIST_D *cl, LIST_D *tl, int n_particles, int n_stages)
{
	int **ii = make_integral_image(img);

	candidate_list_update(cd, img, cl, tl, ii, n_particles, n_stages);
	tracking_list_update(cd, img, cl, tl, ii, n_particles, n_stages);
	candidate_add(cd, img, cl, tl, ii, n_particles, n_stages);

	_dinocv_free((void **)ii);
}

void cascaded_classify_with_swo(CASCADED_DETECTOR_D *cd, IMAGE_D *img)
{
	int **ii = make_integral_image(img);
	cascaded_classify(cd, img, ii);
	_dinocv_free((void **)ii);
}