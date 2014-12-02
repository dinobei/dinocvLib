#include "../common/def_os_selector_export.h"
#include "labeling.h"

// initial point inc/dec value
const POINT_D2 inip[8] = {{1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1}, {0,-1}, {1,-1}};
// move point inc/dec value
const POINT_D2 mvp[8] = {{0,1}, {0,1}, {-1,0}, {-1,0}, {0,-1}, {0,-1}, {1,0}, {1,0}};

int dinocv_fclabeling_tracer(unsigned char **image, POINT_D2 *p, const int initial_pos, int dir, int **label_map, const int C, const int OBJ)
{
	int ipos, iter;
	POINT_D2 old_p;
	int ret_pos=-1;
	int p0, p1;
	
	ipos = (initial_pos<0) ? INITIAL_DIR(dir) : POSITION(initial_pos+2);
	p0 = old_p[0] = (*p)[0];
	p1 = old_p[1] = (*p)[1];
	
	// move to initial search point
	p0 += inip[ipos][0];
	p1 += inip[ipos][1];
	
	iter=MAX_DIRECTION;
	while(iter--){
		if(image[p1][p0] == OBJ){
			label_map[p1][p0] = C;
			ret_pos = POSITION(ipos+4); // set return position.
			break;
		}
		
		// if white pixel, then mark negative integer
		label_map[p1][p0] = -1;
		

		// position change clockwise direction
		ipos = POSITION(ipos+1);
		p0 += mvp[ipos][0];
		p1 += mvp[ipos][1];
	}

	(*p)[0] = p0;
	(*p)[1] = p1;
	
	
	return ret_pos; // isolated pixel
}

void dinocv_fclabeling_contour_tracing(unsigned char **image, const int x, const int y, int dir, LABELINFO_D *label_info, const int C, const int OBJ)
{
	int contour_point=-1;
	int p_contour_point;
	POINT_D2 p;
	POINT_D2 *contour = label_info->contour;
	int T;
	int n_vertex=0;
	int left=0, top=0, right=0, bottom=0;

	label_info->label_map[y][x] = C;
	p[0] = x;
	p[1] = y;

	contour_point = dinocv_fclabeling_tracer(image, &p, contour_point, dir, label_info->label_map, C, OBJ);
	if(contour_point == -1)	return; // isolated pixel

	if(dir == DIR_EXTERNAL)
	{
		contour[n_vertex][0] = p[0];
		contour[n_vertex++][1] = p[1];

		left = right = p[0];
		top = bottom = p[1];
		
	}

	// first output point
	T = contour_point;

	do{
		p_contour_point = contour_point;

		// call tracer
		contour_point = dinocv_fclabeling_tracer(image, &p, contour_point, dir, label_info->label_map, C, OBJ);
		if(dir == DIR_EXTERNAL)
		{
			contour[n_vertex][0] = p[0];
			contour[n_vertex++][1] = p[1];
			
			if(left > p[0]) left = p[0];
			//if(top > p[1]) top = p[1];
			if(right < p[0]) right = p[0];
			if(bottom < p[1]) bottom = p[1];
		}

		// end condition
		if(x == p[0] && y == p[1])						// end condition 1/2
		{
			contour_point = dinocv_fclabeling_tracer(image, &p, contour_point, dir, label_info->label_map, C, OBJ);
			
			if(T==contour_point)						// end condition 2/2
				break;

		}

	}while(1);

	if(dir == DIR_EXTERNAL)
	{
		label_info->contours[C-1] = (POINT_D2 *)realloc(contour, sizeof(POINT_D2) * (n_vertex) );
		label_info->n_vertex[C-1] = n_vertex;

		label_info->rect[C-1].left = left;
		label_info->rect[C-1].top = top;
		label_info->rect[C-1].right = right;
		label_info->rect[C-1].bottom = bottom;
	}
}

LABELINFO_D *dinocv_fclabeling(IMAGE_D *image, int obj_val)
{
	register int i, j;
	int n;
	int *C;
	const int width = image->width;
	const int height = image->height;
	int **label_map;
	uchar_d **source;
	const int OBJ = obj_val, NONOBJ = obj_val?0:255;
	
	if(image->bpp != 8)
		return NULL;

	//initialize
	LABELINFO_D *label_info = _dinocv_labelinfo_create(image);
	label_map = label_info->label_map;
	source = image->source;
	C = &label_info->label_count;
	(*C)=1; // Mark Index
	
	
	// set frame to non_object value
	for(i = 0  ; i < width ; i++)
		source[0][i] = source[height-1][i] = NONOBJ;
	for(i = 0 ; i < height ; i++)
		source[i][0] = source[i][width-1] = NONOBJ;
	
	for(i = 1 ; i < height-1 ; i++){
		for(j = 1 ; j < width-1 ; j++){
			
			if(source[i][j]==OBJ){ // If object pixel
				//////////////////////////////////////////////////////////////////////////////////
				//////////////////////////////////////////////////////////////////////////////////

				if(!label_map[i][j] && source[i-1][j]==NONOBJ){ // If unlabeled && pixel above P is not_object pixel
					label_info->contour = (POINT_D2 *)malloc(sizeof(POINT_D2) * image->width * image->height); // 필요할 때마다 최대 크기로 할당해서 사용하고, tracing안에서 realloc함
					memset(label_info->contour, 0, sizeof(POINT_D2) * image->width * image->height);
					
					dinocv_fclabeling_contour_tracing(source, j, i, DIR_EXTERNAL, label_info, (*C), OBJ);//external contour
					
					label_info->contour = NULL;

					if((*C) >= label_info->predict_label_cnt)
						_dinocv_labelinfo_memory_extension(label_info);

					(*C)++;
				}// End Step 1
				else if(!label_map[i+1][j] && source[i+1][j]==NONOBJ){ // If the pixel below P is unmarked not_object pixel
					// in either case, we proceed to execute contour tracing to find the internal contour containing P,
					// and assign the same label to all the contour pixels.

					// If P is already labeled. In this case, P is also an external contour pixel. -> ignored.
					n = label_map[i][j];
					if(!n){ // If P is not labeled.
						// the preceding point N on the scan line must be labeled.
						n = label_map[i][j-1];
						label_map[i][j] = n;
					}
					// In either case, we proceed to execute contour tracing to find the internal contour containing P,
					// and assign the same label to all the contour pixels.
					dinocv_fclabeling_contour_tracing(source, j, i, DIR_INTERNAL, label_info, n, OBJ);//internal contour

					label_info->n_inner_contour[n-1]++;
				}// End Step 2
				else{
					// below condition NOT EXPLICITLY in paper
					if(!label_map[i][j]){
						// the left neighbor N of P must be a labeled pixel. We assign P the same label as N
						n = label_map[i][j-1];
						label_map[i][j] = n;
					}

				}// End Step 3


				//////////////////////////////////////////////////////////////////////////////////
				//////////////////////////////////////////////////////////////////////////////////
			}
		}
	}
	
	_dinocv_labelinfo_trim(label_info);
	
	return label_info;
}

LABELINFO_D *_dinocv_labelinfo_create(IMAGE_D *image)
{
	LABELINFO_D *label_info;
	label_info = (LABELINFO_D *)malloc(sizeof(LABELINFO_D));
	memset(label_info, 0, sizeof(LABELINFO_D));
	label_info->label_map = (int **)_dinocv_new(SZ_INT, image->width, image->height, 8);
	label_info->rect = NULL;
	label_info->label_count = 0;
	label_info->height = image->height;

	label_info->predict_label_cnt = INITIAL_LABEL_CNT;
	label_info->contours = (POINT_D2 **)malloc(sizeof(POINT_D2 *) * INITIAL_LABEL_CNT);
	memset(label_info->contours, 0, sizeof(POINT_D2 *) * INITIAL_LABEL_CNT);
	label_info->contour = NULL;

	label_info->n_vertex = (int *)malloc(sizeof(int) * INITIAL_LABEL_CNT);
	memset(label_info->n_vertex, 0, sizeof(int) * INITIAL_LABEL_CNT);
	label_info->n_inner_contour = (int *)malloc(sizeof(int) * INITIAL_LABEL_CNT);
	memset(label_info->n_inner_contour, 0, sizeof(int) * INITIAL_LABEL_CNT);

	label_info->rect = (RECT_D *)malloc(sizeof(RECT_D) * INITIAL_LABEL_CNT);
	memset(label_info->rect, 0, sizeof(RECT_D) * INITIAL_LABEL_CNT);

	return label_info;
}

void dinocv_labelinfo_delete(LABELINFO_D *label_info)
{
	int i;

	if(!label_info)
		return;

	_dinocv_free((void **)label_info->label_map);
	if(label_info->rect) free(label_info->rect);

	if(label_info->contours)
	{
		for(i = 0 ; i < label_info->label_count ; i++)
		{
			free(label_info->contours[i]);
			
		}
		free(label_info->contours);
	}
	if(label_info->contour) free(label_info->contour);
	if(label_info->n_vertex) free(label_info->n_vertex);
	if(label_info->n_inner_contour) free(label_info->n_inner_contour);
	

	free(label_info);
}

void _dinocv_labelinfo_memory_extension(LABELINFO_D *label_info)
{	
	int extension_sz = label_info->predict_label_cnt + EXTENSION_SIZE;

	label_info->contours = (POINT_D2 **)realloc(label_info->contours, sizeof(POINT_D2 *) * extension_sz);
	memset(label_info->contours + label_info->predict_label_cnt, 0, sizeof(POINT_D2 *) * EXTENSION_SIZE);
	label_info->n_inner_contour = (int *)realloc(label_info->n_inner_contour, sizeof(int) * extension_sz);
	memset(label_info->n_inner_contour + label_info->predict_label_cnt, 0, sizeof(int) * EXTENSION_SIZE);
	label_info->n_vertex = (int *)realloc(label_info->n_vertex, sizeof(int) * extension_sz);
	memset(label_info->n_vertex + label_info->predict_label_cnt, 0, sizeof(int) * EXTENSION_SIZE);
	label_info->rect = (RECT_D *)realloc(label_info->rect, sizeof(RECT_D) * extension_sz);
	memset(label_info->rect + label_info->predict_label_cnt, 0, sizeof(RECT_D) * EXTENSION_SIZE);

	label_info->predict_label_cnt = extension_sz;
}

void _dinocv_labelinfo_trim(LABELINFO_D *label_info)
{
	int label_count = label_info->label_count;
	label_info->contours = (POINT_D2 **)realloc(label_info->contours, sizeof(POINT_D2 *) * label_count );
	label_info->n_inner_contour = (int *)realloc(label_info->n_inner_contour, sizeof(int) * label_count );
	label_info->n_vertex = (int *)realloc(label_info->n_vertex, sizeof(int) * label_count );
	label_info->rect = (RECT_D *)realloc(label_info->rect, sizeof(RECT_D) * label_count );
}
