#include "../common/def_os_selector_export.h"
#include "convexhull.h"

void _dinocv_swap_point(POINT_D *pt1, POINT_D *pt2){
	POINT_D tp;

	memcpy(&tp, pt1, sizeof(POINT_D));
	memcpy(pt1, pt2, sizeof(POINT_D));
	memcpy(pt2, &tp, sizeof(POINT_D));
}

PTARRAY_D *dinocv_graham_scan(PTARRAY_D *arr){
	int i;
	int cnt = arr->cnt;
	POINT_D *points = arr->ptarr;
	int miny, minidx;

	miny=0x7FFFFFFF;
	minidx=0;
	for(i = 1 ; i <= cnt ; i++){
		if(points[i].y > miny){
			miny = points[i].y;
			minidx=i;
		}
	}
	
	// minidx not must be zero
	// points[1] with the point with the lowest y-coordinate
	_dinocv_swap_point(&points[minidx], &points[1]);
	
	PTARRAY_D *ret = (PTARRAY_D *)malloc(sizeof(PTARRAY_D));
	return ret;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
int **_xiwi;
int **_yiwi;

void dinocv_init_dinohull(int width, int height, int nop)
{
	register int i;
	_xiwi = (int **)malloc(sizeof(int *)*(width+1));
	for(i = 0 ; i < width+1 ; i++){
		_xiwi[i] = (int *)malloc(sizeof(int)*nop);
	}
	_yiwi = (int **)malloc(sizeof(int *)*(height+1));
	for(i = 0 ; i < height+1 ; i++){
		_yiwi[i] = (int *)malloc(sizeof(int)*nop);
	}
}

void dinocv_uninit_dinohull(int width, int height)
{
	register int i;

	for(i = 0 ; i < height ; i++)
		free(_xiwi[i]);
	free(_xiwi);

	for(i = 0 ; i < width ; i++)
		free(_yiwi[i]);
	free(_yiwi);
}

void dinocv_replacement_idxnval(int **riwi, int *coorArray, int len, int max)
{
	register int i, cidx;
	int iwi;// Index within Index

	// use 0 index for current index.
	for(i = 0 ; i <= max ; i++)
		riwi[i][0]=1;

	// index-value replacement
	for(i = 0 ; i < len ; i++){
		iwi = coorArray[i]; // Index into Index

		cidx = riwi[iwi][0]++;
		riwi[iwi][cidx] = i;
	}
}



bool_d _dinocv_calc_inner_extreme_point(int **iwi,int *ascending, const int mainIdx, int cond1, int cond2, POINT_D2 *sv, POINT_D2 *ev, POINT_D2 *ret){
	register int i, n, a;
	int subIdx = !mainIdx;// X_COOR <-> Y_COOR

	int right = (*sv)[mainIdx]>(*ev)[mainIdx] ? (*sv)[mainIdx] : (*ev)[mainIdx];
	int left = (*sv)[mainIdx]<(*ev)[mainIdx] ? (*sv)[mainIdx] : (*ev)[mainIdx];
	int bottom = (*sv)[subIdx]>(*ev)[subIdx] ? (*sv)[subIdx] : (*ev)[subIdx];
	int top = (*sv)[subIdx]<(*ev)[subIdx] ? (*sv)[subIdx] : (*ev)[subIdx];
	int curIdx;
	int extr, extr_bk; // extreme value; max or min

	while(1)
	{
#define GET_INNER_RIGHT()	while(iwi[--right][0] == 1)
#define GET_INNER_LEFT()	while(iwi[++left][0] == 1)

		if(cond1 == MAXIMUM)	GET_INNER_RIGHT();
		else					GET_INNER_LEFT();
		
		if(right <=left) return d_false;
		curIdx = cond1 == MAXIMUM ? right : left;
		
		n = iwi[curIdx][0];

		extr_bk = extr = cond2==MAXIMUM ? SINTMIN : SINTMAX;
		for(i = 1 ; i < n ; i++){
			a = ascending[iwi[curIdx][i]];
			
			if(d_limit_oo(a, top, bottom) && ((cond2==MAXIMUM)?(extr<a):(extr>a)))
				extr = a;			
		}
		if(extr == extr_bk)
			continue;
		break;
	}

	(*ret)[mainIdx] = curIdx;
	(*ret)[subIdx] = extr;
	return d_true;
}

void _dinocv_partial_zone_init(int *ascending, int **iwi, POINT_D2 *minPt, POINT_D2 *maxPt, int mainIdx, int incdec, int startIdx)
{
	register int i, k;
	int idx;
	int max, min;
	int n;
	int subIdx = mainIdx?X_COOR:Y_COOR;

	k=startIdx;
	while(iwi[k][0]==1) // k is left most idx
		k+=incdec;

	(*minPt)[mainIdx] = (*maxPt)[mainIdx] = k;

	n = iwi[k][0];// (the number of value of coor is k) + 1

	idx = iwi[k][1];
	max = min = ascending[idx];
	for(i = 2 ; i < n ; i++)
	{
		idx = iwi[k][i];
		if(ascending[idx] > max)
			max = ascending[idx];
		if(ascending[idx] < min)
			min = ascending[idx];
	}
	(*minPt)[subIdx] = min;
	(*maxPt)[subIdx] = max;

}

POINT_D2 *_dinocv_zoneinit(int *xasc, int *yasc, int **_xiwi, int **_yiwi, int nop, int width, int height)
{
	POINT_D2 *zone = (POINT_D2 *)malloc(sizeof(POINT_D2)*8);
	
	_dinocv_partial_zone_init(yasc, _xiwi, &zone[0], &zone[7], X_COOR, -1, width-1);
	_dinocv_partial_zone_init(xasc, _yiwi, &zone[2], &zone[1], Y_COOR, 1, 0);
	_dinocv_partial_zone_init(yasc, _xiwi, &zone[3], &zone[4], X_COOR, 1, 0);
	_dinocv_partial_zone_init(xasc, _yiwi, &zone[5], &zone[6], Y_COOR, -1, height-1);
	return zone;
}


bool_d _dinocv_is_convex_vertex(POINT_D2  a, POINT_D2 b, POINT_D2 p, int zone){
	float sx, sy, ex, ey, ycoor;
	
	sx = (float)a[X_COOR];
	sy = (float)a[Y_COOR];
	ex = (float)b[X_COOR];
	ey = (float)b[Y_COOR];
	ycoor = (ey-sy)/(ex-sx)*(p[X_COOR]-sx) + sy;

	if(zone <= B_ZONE){
		if(ycoor > p[Y_COOR])
			return d_true;
	}
	else if(ycoor < p[Y_COOR])
			return d_true;
	return d_false;
}


int dinocv_dinohull(int *xasc, int *yasc, int nop, POINT_D2 *chl, int width, int height)
{
	register int zn;
	POINT_D2 *zone, sv, ev, m, n, new_m;
	int mainIdx, subIdx, cond1, cond2, chl_idx=0;
	const int mnLut[5] = {MAXIMUM, MINIMUM, MINIMUM, MAXIMUM, MAXIMUM};

	// STEP 1. Sort(replacement index & value)
	dinocv_replacement_idxnval(_xiwi, xasc, nop, width);
	dinocv_replacement_idxnval(_yiwi, yasc, nop, height);

	// STEP 2. Calculation Extreme Point(Init Zone Array)
	zone = _dinocv_zoneinit(xasc, yasc, _xiwi, _yiwi, nop, width, height);

#define PUSH(x)		(memcpy(chl[chl_idx++], (x), sizeof(POINT_D2)))
#define STACKTOP	chl[chl_idx-1]

	// STEP 3. Iterate Each Zone.
	mainIdx=Y_COOR; chl_idx=0;
	for(zn = 0 ; zn < 8 ; zn+=2){
		// set main coordinates
		subIdx = mainIdx;
		mainIdx = !mainIdx;

		// set sv, ev
		memcpy(sv ,zone[zn], sizeof(POINT_D2));
		memcpy(ev ,zone[zn+1], sizeof(POINT_D2));

		// push sv
		if(chl_idx == 0 || memcmp(STACKTOP, sv, sizeof(POINT_D2)))
			PUSH(sv);
		if(!memcmp(sv, ev, sizeof(POINT_D2))) continue;
		
		// get each zone condition
		cond1 = mnLut[(zn>>1)];
		cond2 = mnLut[(zn>>1)+1];

		/* STEP 3-1. Calc Zone-Edge(m, n) */
		if(!_dinocv_calc_inner_extreme_point(mainIdx?_yiwi:_xiwi, mainIdx?xasc:yasc, mainIdx, cond1, cond2, &sv, &ev, &m))
		{
			PUSH(ev);
			continue;
		}
		_dinocv_calc_inner_extreme_point(mainIdx?_xiwi:_yiwi, mainIdx?yasc:xasc, subIdx, cond2, cond1, &sv, &ev, &n);
		
		// Step 3-2. Iterate m checking & renewal
		while(1){
			if(!memcmp(m, n, sizeof(POINT_D2)))// if m==n
			{
				if(_dinocv_is_convex_vertex(sv, ev, m, zn))
						PUSH(m);
				PUSH(ev);
				break; // end condition
			}
			
			if(!_dinocv_calc_inner_extreme_point(mainIdx==X_COOR?_xiwi:_yiwi, mainIdx==X_COOR?yasc:xasc,
				mainIdx, cond1, cond2, &m, &ev, &new_m))
			{
				PUSH(ev);
				break;
			}

			if(_dinocv_is_convex_vertex(sv, ev, m, zn) && !_dinocv_is_convex_vertex(sv, m, new_m, zn))
				PUSH(m);
			memcpy(sv, m, sizeof(POINT_D2));
			memcpy(m, new_m, sizeof(POINT_D2)); // renewal m.
		}
	}

	if(!memcmp(zone[0], zone[7], sizeof(POINT_D2)))
		chl_idx--;

	return chl_idx;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/* the result vector; indicates whether a point belongs to the hull*/
int *belongs_to_hull;

/* output routines: */
void print_array(int *pt, int n, int *x, int *y)
{
	long j;

	printf("Array with %d points:\n", n );
	for (j=0; j<n; j++)
		printf("point %d = (%d,%d)\n", pt[j], (int)x[pt[j]], (int)y[pt[j]] );
	printf("\n");
}

void print_hull(const int n, int *x, int *y)
{
	int j;

	printf("Points of the convex hull:\n");
	for (j=0; j<n; j++)
		if (belongs_to_hull[j])
			printf(" (%d,%d)\n", (int)x[j], (int)y[j] );
}

#define cross(p,a,b) \
	/* return crossproduct of vectors p-a and b-a */ \
	((x[a]-x[p])*(y[b]-y[p]) - (y[a]-y[p])*(x[b]-x[p]))

#define leftturn(a,b,c) \
	/*true iff point c is lefthand of vector through points (a,b) */ \
	(cross(c,a,b)>0.0)

int *delete_right(int *pt, int *x, int *y, int *num, int p1, int p2)
{
	int j;
	int leftcnt;
	int *left;
	int n = *num;

	/* delete all points in pt located right from line p1p2: */
	left = (int *) malloc( n * sizeof(int) );
	left[0]=p1; left[1]=p2;
	leftcnt = 2;
	for (j=0; j<*num; j++)
		if (!(pt[j]==p1 || pt[j]==p2))   /*p1 and p2 already in left[]*/
			if (leftturn(p1,p2,pt[j])) /* point j is lefthand to vector p1p2 */
				left[leftcnt++] = pt[j];

	*num = leftcnt;
	return left;
}


void inithull( int *pt, int *x, int *y, int n, int *minx, int *maxx, int **upper, int **lower)
{
	int p1, p2;
	int i;

	*upper = (int *)malloc(n*sizeof(int));
	*lower = (int *)malloc(n*sizeof(int));

	/* determine points p1,p2 with minimal and maximal x coordinate: */
	p1 = p2 = pt[0];   /* init. p1,p2 to first point */
	for (i=1; i<n; i++) {   /* seq. search for minimum / maximum */
		if (x[pt[i]] < x[p1])   p1 = i;
		if (x[pt[i]] > x[p2])   p2 = i;
	}

	belongs_to_hull = (int *)malloc(sizeof(int)*n);
	memset(belongs_to_hull, 0, sizeof(int)*n);

	belongs_to_hull[p1] = 1;
	belongs_to_hull[p2] = 1;
	*minx = p1; *maxx = p2;
}


int pivotize(int *pt, int n, int *x, int *y)    /* n>=3 is assumed */
	/* as pivot, select the point in pt[]-{p1,p2} with maximal
	* cross_prod( pivot-p1, p2-p1 )
	*/
{
	int i, p1=pt[0], p2=pt[1];
	int pivotpos = 2;
	double maxcross = cross(pt[2], p1, p2);
	for (i=3; i<n; i++) {        /* sequential maximization */
		double newcross = cross(pt[i], p1, p2);
		if (newcross > maxcross) {
			maxcross = newcross;
			pivotpos = i;
		}
	}
	return pt[pivotpos];
}


void dinocv_quickhull(int *pt, int n, int *x, int *y)
{
	/* DC step: select pivot point from pt */
	int pivot;
	int p1=pt[0], p2=pt[1];
	int *left1, *left2, leftcnt1, leftcnt2;

	/* DC step: select any pivot point from pt.
	We have p1==pt[0],p2==pt[1] */
	if (n==2) return;

	if (n==3) { 
		/* one point (beyond old p1,p2) must belong to hull */
		belongs_to_hull[pt[2]] = 1;      /* saves a recursive call */
		return;
	}

	pivot = pivotize(pt, n, x, y);

	belongs_to_hull[pivot] = 1;
	leftcnt1 = n;

	left1 = delete_right( pt, x, y, &leftcnt1, p1, pivot );

	dinocv_quickhull(left1, leftcnt1, x, y);
	leftcnt2 = n;

	left2 = delete_right(pt, x, y, &leftcnt2, pivot, p2);
	dinocv_quickhull(left2, leftcnt2, x, y);
}

