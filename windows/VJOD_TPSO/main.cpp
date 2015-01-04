#include <windows.h>
#include <shlobj.h>
#include <Vfw.h>
#include <dinocvlib.h>
#include "Classifier.h"
#include "resource.h"
#include "res_id.h"
#include "dinotime.h"
#include <Strsafe.h>

#include <CommCtrl.h>
#include "Shlwapi.h"

#define WIDTH					640
#define HEIGHT					480
#define NUMBER_OF_MODELS		10

#define INITIAL_PSO_PARTICLES	15
#define INITIAL_PSO_STAGES		5

#define MIN_PARTICLES	5
#define MAX_PARTICLES	50
#define MIN_STAGES		1
#define MAX_STAGES		30

#define INITIAL_COLOR_RED		255
#define	INITIAL_COLOR_GREEN		0
#define INITIAL_COLOR_BLUE		0
#define INITIAL_COLOR_THICK		5



CASCADED_DETECTOR_D *cd[NUMBER_OF_MODELS];

LRESULT CALLBACK WndProc(HWND,UINT, WPARAM, LPARAM);
BOOL CALLBACK SettingDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);

void init_avg_time_calculator();


#define N_LUT_SF					25 // 영상 크기에 따라 자동으로 선택할 수 있도록
#define INITIAL_SCALE_FACTOR_INDEX	3 // (1.25)^INITIAL_SCALE_FACTOR_INDEX

HWND hWndMain;
HINSTANCE g_hInst;
LPCTSTR lpszClass=TEXT("VJOD_TPSO");

HWND hCaptureWindow;
BITMAPINFO biVFW;

IMAGE_D *_gimg, *_ggray;

// UI
HWND hBtnSetting, hBtnDownStage, hBtnUpStage,
	hRBtnSWO, hRBtnPSO,
	hTrackParticles, hTrackStages,
	hModelList,
	hStaticRed, hStaticGreen, hStaticBlue, hStaticThick;

bool isPSO;
TCHAR proc_time[50];
int _gCurrentSelectModel=-1;
int _gNumberOfModel;

typedef struct od_parameters od_parameters;
struct od_parameters{
	char model_file_name[_MAX_PATH];
	int subwindow_width;
	int subwindow_height;
	int classification_methods;
	float scale_factor;
	int initial_scale_factor_index;
};

typedef struct CANDIDATE_OBJECT CANDIDATE_OBJECT;
struct CANDIDATE_OBJECT
{
	int detection_cnt;
	int x, y, w, h;
	int miss_cnt;
};




od_parameters _godparm;

const int N=50;
int division;
int time_arr[N]={0,};
int cpt=0;
int npt=0;
int total_time=0;
float avg_time;
RECT _grt; // for fps display
RECT _grt_particles; // for number of particles display
RECT _grt_stages; // for number of stages display

int _global_particles[NUMBER_OF_MODELS];
int _global_stage[NUMBER_OF_MODELS];

LIST_D *tracking_list[NUMBER_OF_MODELS];
LIST_D *candidate_list[NUMBER_OF_MODELS];

void cascaded_classify_with_tpso(IMAGE_D *gray)
{
	int **ii;
	ii = make_integral_image(_ggray);
}


LRESULT CALLBACK CallbackOnFrame(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	int **ii;
	RECT_D *result_rt_arr;
	int result_rt_cnt;
	for(int i = HEIGHT-1, j=0 ; i >= 0 ; i--, j++)
	{
		memcpy(_gimg->source[i], lpVHdr->lpData + (j*WIDTH * 3), sizeof(BYTE)*WIDTH*3);
	}

	
	dinocv_conv_24to8_cpy(_gimg, _ggray);

	DWatch watch;
	watch.Start();

	ii = make_integral_image(_ggray);
	time_arr[cpt]=0;
	
	/* Each Models & Objects */
	for(int iter = 0 ; iter < _gNumberOfModel ; iter++)
	{
		/* Candidate list Update */
		int cc = candidate_list[iter]->cnt;
		for(int ci = cc-1 ; ci >= 0 ; ci--)
		{
			// Searching Area with PSO & Renewing data (if exist, miss_cnt++, else detection_cnt++)
			CANDIDATE_OBJECT *co = (CANDIDATE_OBJECT *)soc_list_get_idx_data(candidate_list[iter], ci);
			cascaded_classify_with_pso(cd[iter],
				&dinocv_set_rect(
				(UINT)d_clp_boundary(co->x-co->w, 0, _ggray->width-1), (UINT)d_clp_boundary(co->y-co->h, 0, _ggray->height-1),
				(UINT)d_clp_boundary(co->x+co->w, 0, _ggray->width-1), (UINT)d_clp_boundary(co->y+co->h, 0, _ggray->height-1)
				),
				_ggray, ii, _global_particles[iter], _global_stage[iter]);

			result_rt_arr = cd[iter]->merged_detection_result->n_objects <= 1 ? cd[iter]->detection_result->p_rt :
				cd[iter]->merged_detection_result->p_rt;
			result_rt_cnt = cd[iter]->merged_detection_result->n_objects <= 1 ? cd[iter]->detection_result->n_objects :
				cd[iter]->merged_detection_result->n_objects;
				
			double min_length = MAX_TRACKING_LENGTH;
			for(int i = 0 ; i < result_rt_cnt ; i++)
			{
				int x2 = (result_rt_arr[i].left+result_rt_arr[i].right)>>1;
				int y2 = (result_rt_arr[i].top+result_rt_arr[i].bottom)>>1;

				double length = sqrt(pow(co->x-x2, 2)+pow(co->y-y2, 2) );
				if(length < min_length)
				{
					min_length = length;			 
					co->x = x2;
					co->y = y2;
					co->w = (result_rt_arr[i].right - result_rt_arr[i].left);
					co->h = (result_rt_arr[i].bottom - result_rt_arr[i].top);
					co->detection_cnt++;
				}
			}
			if(min_length == MAX_TRACKING_LENGTH)
			{
				if(++co->miss_cnt > CANDIDATE_MISS_MAX)
				{
					free((CANDIDATE_OBJECT *)soc_list_del_idx_data(candidate_list[iter], ci));
					printf("\t - candidate object deleted..(%d)\n", candidate_list[iter]->cnt);
				}
			}

			// send to tracking list (detection_cnt >= CANDIDATE_CNT_MAX)
			if(co->detection_cnt >= CANDIDATE_CNT_MAX)
			{
				// remove overlapping object
				CANDIDATE_OBJECT *co = (CANDIDATE_OBJECT *)soc_list_del_idx_data(candidate_list[iter], ci);
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

				soc_list_add_head(tracking_list[iter], to);

				printf(" + tracking object added..(%d), and current candidate object is (%d)\n", tracking_list[iter]->cnt, candidate_list[iter]->cnt);

			}
		}		
		
		// Tracking list Update
		int tc = tracking_list[iter]->cnt;
		for(int ti = tc-1 ; ti >= 0 ; ti--)
		{
			// Searching Area with PSO & Renewing data (if exist, miss_cnt++, else detection_cnt++)
			TRACKING_OBJECT *to = (TRACKING_OBJECT *)soc_list_get_idx_data(tracking_list[iter], ti);
			int tpso_search_width = TO_GET_AVG_W(to);//TO_GET_AVG_W(to) < TPSO_SEARCH_AREA_LEFT ? TPSO_SEARCH_AREA_LEFT : TO_GET_AVG_W(to) ;
			int tpso_search_height = TO_GET_AVG_H(to);
			cascaded_classify_with_pso(cd[iter],
				&dinocv_set_rect(
				d_clp_boundary(TO_GET_AVG_X(to)-tpso_search_width, 0, _ggray->width-1), d_clp_boundary(TO_GET_AVG_Y(to)-tpso_search_height, 0, _ggray->height-1),
				d_clp_boundary(TO_GET_AVG_X(to)+tpso_search_width, 0, _ggray->width-1), d_clp_boundary(TO_GET_AVG_Y(to)+tpso_search_height, 0, _ggray->height-1)
				),
				_ggray, ii, _global_particles[iter], _global_stage[iter]);

			result_rt_arr = cd[iter]->merged_detection_result->n_objects == 0 ? cd[iter]->detection_result->p_rt :
				cd[iter]->merged_detection_result->p_rt;
			result_rt_cnt = cd[iter]->merged_detection_result->n_objects == 0 ? cd[iter]->detection_result->n_objects :
				cd[iter]->merged_detection_result->n_objects;

			double min_length = MAX_TRACKING_LENGTH;
			for(int i = 0 ; i < result_rt_cnt ; i++)
			{
				int x2 = (result_rt_arr[i].left + result_rt_arr[i].right)>>1;
				int y2 = (result_rt_arr[i].top + result_rt_arr[i].bottom)>>1;

				double length = sqrt(pow(TO_GET_AVG_X(to)-x2, 2)+pow(TO_GET_AVG_Y(to)-y2, 2) );
				if(length < min_length)
				{
					min_length = length;
					// delete previous value
					to->x_sum -= to->x_arr[to->pos_cidx];
					to->y_sum -= to->y_arr[to->pos_cidx];
					to->w_sum -= to->w_arr[to->length_cidx];
					to->h_sum -= to->h_arr[to->length_cidx];

					// add current result to array
					to->x_sum += to->x_arr[to->pos_cidx] = x2;
					to->y_sum += to->y_arr[to->pos_cidx] = y2;
					to->w_sum += to->w_arr[to->length_cidx] = (result_rt_arr[i].right - result_rt_arr[i].left);
					to->h_sum += to->h_arr[to->length_cidx] = (result_rt_arr[i].bottom - result_rt_arr[i].top);

					if(to->len_division_cnt < TRACKING_LENGTH_ARR_MAX) to->len_division_cnt++;
					if(to->pos_division_cnt < TRACKING_POS_ARR_MAX) to->pos_division_cnt++;
					to->length_cidx = (++to->length_cidx) % TRACKING_LENGTH_ARR_MAX;
					to->pos_cidx = (++to->pos_cidx) % TRACKING_POS_ARR_MAX;

					to->miss_cnt=0;

					to->cur_x = x2;
					to->cur_y = y2;
				}
			}
			if(min_length == MAX_TRACKING_LENGTH)
			{
				if(++to->miss_cnt > TRACKING_MISS_MAX)
				{
					free((TRACKING_OBJECT *)soc_list_del_idx_data(tracking_list[iter], ti));
					printf(" - tracking object deleted..(%d)\n", tracking_list[iter]->cnt);
				}
			}
		}

		tc = tracking_list[iter]->cnt;
		for(int ti = 0 ; ti < tc-1 ; ti++)
		{
			for(int ti2 = ti+1 ; ti2 < tc ; ti2++)
			{
				if(ti != ti2)
				{
					TRACKING_OBJECT *to = (TRACKING_OBJECT *)soc_list_get_idx_data(tracking_list[iter], ti);
					if(to->cur_x > 0)
					{
						TRACKING_OBJECT *to2 = (TRACKING_OBJECT *)soc_list_get_idx_data(tracking_list[iter], ti2);
					
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
		for(int ti = tc-1 ; ti >= 0 ; ti--)
		{
			TRACKING_OBJECT *to = (TRACKING_OBJECT *)soc_list_get_idx_data(tracking_list[iter], ti);
			if(to->cur_x < 0)
				free((TRACKING_OBJECT *)soc_list_del_idx_data(tracking_list[iter], ti));
		}
		
		// Add Candidate List
		if(isPSO)
		{
			// Swipe CO & TO Area
			tc = tracking_list[iter]->cnt;
			for(int ti = 0 ; ti < tc ; ti++)
			{
				TRACKING_OBJECT *to = (TRACKING_OBJECT *)soc_list_get_idx_data(tracking_list[iter], ti);
				int t = (uint_d)(TO_GET_AVG_Y(to)-((TO_GET_AVG_H(to)>>1)));
				int b = (uint_d)(TO_GET_AVG_Y(to)+((TO_GET_AVG_H(to)>>1)));
				int l = (uint_d)(TO_GET_AVG_X(to)-((TO_GET_AVG_W(to)>>1)));
				int w = (uint_d)(TO_GET_AVG_W(to));
				if(t < 0) t = 0;
				if(b > _gimg->height) b = _gimg->height;
				for(t ; t < b ; t++)
				{
					memset(&_ggray->source[t][l], 0, sizeof(uchar_d) * w);
				}
			}

			cc = candidate_list[iter]->cnt;
			for(int ci = 0 ; ci < cc ; ci++)
			{
				CANDIDATE_OBJECT *co = (CANDIDATE_OBJECT *)soc_list_get_idx_data(candidate_list[iter], ci);
				int t = co->y-(co->h>>1);
				int b = co->y+(co->h>>1);
				int l = co->x-(co->w>>1);
				int w = co->w;
				for(t ; t < b ; t++)
				{
					memset(&_ggray->source[t][l], 0, sizeof(uchar_d) * w);
				}
			}


			cascaded_classify_with_pso(cd[iter], &dinocv_set_rect(0, 0, _ggray->width, _ggray->height),
				_ggray, ii, _global_particles[iter], _global_stage[iter]);
		}
		else
		{
			cascaded_classify(cd[iter], _ggray, ii);
		}
		
		if(isPSO)
		{
			result_rt_arr = cd[iter]->merged_detection_result->n_objects == 0 ? cd[iter]->detection_result->p_rt :
				cd[iter]->merged_detection_result->p_rt;
			result_rt_cnt = cd[iter]->merged_detection_result->n_objects == 0 ? cd[iter]->detection_result->n_objects :
				cd[iter]->merged_detection_result->n_objects;
			for(int j = 0 ; j < result_rt_cnt ; j++)
			{
				//dinocv_draw_rect(_gimg, &result_rt_arr[j], &cd[iter]->color, 5);

				int no_add = 0;

				// Push this object to candidate list when current object not overlapped with object of tracking list
				int x2=((result_rt_arr[j].left + result_rt_arr[j].right)>>1);
				int y2=((result_rt_arr[j].top + result_rt_arr[j].bottom)>>1);

				int cc = candidate_list[iter]->cnt;
				for(int ci = 0 ; ci < cc ; ci++)
				{
					CANDIDATE_OBJECT *co = (CANDIDATE_OBJECT *)soc_list_get_idx_data(candidate_list[iter], ci);
				
					double length = sqrt(pow(co->x-x2, 2)+pow(co->y-y2, 2) );

					if((int)length < (co->w>>1) )
					{
						no_add++;
						ci=cc;
					}
				}
				int tc = tracking_list[iter]->cnt;
				for(int ti = 0 ; ti < tc ; ti++)
				{
					TRACKING_OBJECT *to = (TRACKING_OBJECT *)soc_list_get_idx_data(tracking_list[iter], ti);
					double length = sqrt(pow(TO_GET_AVG_X(to)-x2, 2)+pow(TO_GET_AVG_Y(to)-y2, 2) );

					if((int)length < (TO_GET_AVG_W(to)>>1) )
					{
						no_add++;
						ti=tc;
					}
				}

				if(!no_add)
				{
					// push candidate list
					CANDIDATE_OBJECT *co = (CANDIDATE_OBJECT *)malloc(sizeof(CANDIDATE_OBJECT));
					memset(co, 0, sizeof(CANDIDATE_OBJECT));
					co->x = (result_rt_arr[j].left + result_rt_arr[j].right)>>1;
					co->y = (result_rt_arr[j].top + result_rt_arr[j].bottom)>>1;
					co->w = result_rt_arr[j].right - result_rt_arr[j].left;
					co->h = result_rt_arr[j].bottom - result_rt_arr[j].top;
					co->detection_cnt++;
					soc_list_add_head(candidate_list[iter], (void *)co);
					printf("\t + candidate object added..(%d)\n", candidate_list[iter]->cnt);
				}
			}
		}

		/*
		printf(" # %d\n", candidate_list[iter]->cnt);
		for(int j = 0 ; j < candidate_list[iter]->cnt ; j++)
		{
			CANDIDATE_OBJECT *co = (CANDIDATE_OBJECT *)soc_list_get_idx_data(candidate_list[iter], j);
			dinocv_draw_rect(_gimg, &dinocv_set_rect(co->x-(co->w>>1), co->y-(co->h>>1), co->x+(co->w>>1), co->y+(co->h>>1)), &cd[iter]->color, 5);
		}
		*/
		
		//printf(" # %d\n", tracking_list[iter]->cnt);
		
	}
	watch.End();

	// display rect
	if(isPSO) // PSO Scanning
	{
		//IMAGE_D *timg = dinocv_conv_8to24(_ggray);
		//dinocv_copy_image_cpy(_gimg, timg);
		//dinocv_release_image(timg);
		for(int iter = 0 ; iter < _gNumberOfModel ; iter++)
		{
			for(int j = 0 ; j < tracking_list[iter]->cnt ; j++)
			{
				TRACKING_OBJECT *to = (TRACKING_OBJECT *)soc_list_get_idx_data(tracking_list[iter], j);

				dinocv_draw_rect(_gimg, &dinocv_set_rect(
					(uint_d)(TO_GET_AVG_X(to)-((TO_GET_AVG_W(to)>>1))*cd[iter]->lr) ,
					(uint_d)(TO_GET_AVG_Y(to)-((TO_GET_AVG_H(to)>>1))*cd[iter]->tb) ,
					(uint_d)(TO_GET_AVG_X(to)+((TO_GET_AVG_W(to)>>1))*cd[iter]->lr) ,
					(uint_d)(TO_GET_AVG_Y(to)+((TO_GET_AVG_H(to)>>1))*cd[iter]->tb)
					),
					&cd[iter]->color, cd[iter]->thick);
			}
		}

		/*
		for(int iter = 0 ; iter <_gNumberOfModel ; iter++)
		{
			for(int j = 0 ; j < candidate_list[iter]->cnt ; j++)
			{
				CANDIDATE_OBJECT *co = (CANDIDATE_OBJECT *)soc_list_get_idx_data(candidate_list[iter], j);

				float rate = (float)co->detection_cnt/CANDIDATE_CNT_MAX;
				dinocv_draw_rect(_gimg, &dinocv_set_rect(
					co->x-(co->w>>1) * cd[iter]->lr,
					co->y-(co->h>>1) * cd[iter]->tb,
					co->x+(co->w>>1) * cd[iter]->lr,
					co->y+(co->h>>1) * cd[iter]->tb
					),
					&dinocv_set_color(rate*255, 0, 0), cd[iter]->thick);
				
			}
		}
		*/
	}
	else // SWO Scanning
	{
		for(int iter = 0 ; iter < _gNumberOfModel ; iter++)
		{
			for(int j = 0 ; j < cd[iter]->merged_detection_result->n_objects ; j++)
			{
				
				dinocv_draw_rect(_gimg, &dinocv_set_rect(
					cd[iter]->merged_detection_result->p_rt[j].left,
					cd[iter]->merged_detection_result->p_rt[j].top,
					cd[iter]->merged_detection_result->p_rt[j].right,
					cd[iter]->merged_detection_result->p_rt[j].bottom),
					&cd[iter]->color, cd[iter]->thick);
			}
		}
	}

	time_arr[cpt] += (int)watch.GetDurationMilliSecond();

	total_time += time_arr[cpt];
	npt = (cpt+1)%N;
	if(division < N) division++;
	avg_time = (float)total_time/division;
	total_time -= time_arr[npt];
	cpt=npt;
	StringCchPrintf(proc_time, sizeof(proc_time)/sizeof(TCHAR), TEXT("%3.1lf msec  fps=%3.1lf"), avg_time, 1000./avg_time);
	SetDlgItemText(hWndMain, ID_STATIC_PROC_TIME, proc_time);

	for(int i = HEIGHT-1, j=0 ; i >= 0 ; i--, j++)
	{
		memcpy(lpVHdr->lpData + (j*WIDTH * 3), _gimg->source[i], sizeof(BYTE)*WIDTH*3);
	}
	
	// clean up
	_dinocv_free((void **)ii);

	return (LRESULT)true;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{

	// these next few lines create and attach a console
	// to this process.  note that each process is only allowed one console.
	/**/
	FILE *in;
	AllocConsole() ;
	AttachConsole( GetCurrentProcessId() ) ;
	freopen_s(&in, "CON", "w", stdout);
	printf("HELLO!!! I AM THE CONSOLE!" ) ;
	
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;

	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)GetStockObject(COLOR_WINDOW+1);
	WndClass.hCursor=LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon=LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW|CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd=CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while(GetMessage(&Message, NULL, 0, 0)){
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT crt;
	TCHAR wstrPath[MAX_PATH]={NULL, };
	char str[_MAX_PATH];

	switch(iMessage){
	case WM_CREATE:
		InitCommonControls();
		hWndMain=hWnd;
		
		SetRect(&_grt, 450, 535, 480+80, 535+25);
		SetRect(&_grt_particles, 80, 565, 80, 25);
		SetRect(&_grt_stages, 80, 595, 80, 25);

		// Adjust Main Window Size
		SetRect(&crt, 0, 0, 1100, 630);
		AdjustWindowRect(&crt, WS_OVERLAPPEDWINDOW, FALSE);
		SetWindowPos(hWnd, NULL, 0, 0, crt.right-crt.left, crt.bottom-crt.top,
			SWP_NOMOVE|SWP_NOZORDER);


		// Stage Up/Down Button and Static for Stage Display by text
		CreateWindow(TEXT("button"), TEXT("↓"), WS_CHILD|WS_VISIBLE,
			10, 535, 25, 25, hWnd, (HMENU)ID_BTNDOWNSTAGE, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("↑"), WS_CHILD|WS_VISIBLE,
			45, 535, 25, 25, hWnd, (HMENU)ID_BTNUPSTAGE, g_hInst, NULL);
		CreateWindow(TEXT("static"), TEXT("detector not loaded."), WS_CHILD|WS_VISIBLE,
			80, 535, 150, 25, hWnd, (HMENU)ID_STATICSTAGE, g_hInst, NULL);

		// For Selection Scan Methods
		hRBtnSWO=CreateWindow(TEXT("button"), TEXT("SWO"), WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON,
			240, 535, 80, 25, hWnd, (HMENU)ID_RBTN_SWO, g_hInst, NULL);
		hRBtnPSO=CreateWindow(TEXT("button"), TEXT("TPSO"), WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON,
			320, 535, 80, 25, hWnd, (HMENU)ID_RBTN_PSO, g_hInst, NULL);
		CheckRadioButton(hWnd, ID_RBTN_SWO, ID_RBTN_PSO, ID_RBTN_PSO);
		isPSO = true;

		CreateWindow(TEXT("static"), TEXT("0.0msec"), WS_CHILD|WS_VISIBLE,
			450, 535, 150, 25, hWnd, (HMENU)ID_STATIC_PROC_TIME, g_hInst, NULL);
		
		CreateWindow(TEXT("button"), TEXT("↓"), WS_CHILD|WS_VISIBLE,
			10, 565, 25, 25, hWnd, (HMENU)ID_BTN_N_PARTICLES_DOWN, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("↑"), WS_CHILD|WS_VISIBLE,
			45, 565, 25, 25, hWnd, (HMENU)ID_BTN_N_PARTICLES_UP, g_hInst, NULL);
		CreateWindow(TEXT("static"), TEXT("15"), WS_CHILD|WS_VISIBLE,
			80, 565, 80, 25, hWnd, (HMENU)ID_STATIC_NUM_PARTICLES, g_hInst, NULL);
		hTrackParticles = CreateWindow(TRACKBAR_CLASS, NULL, WS_CHILD|WS_VISIBLE,
			170, 565, 400, 30, hWnd, NULL, g_hInst, NULL);
		SendMessage(hTrackParticles, TBM_SETRANGE, FALSE, MAKELPARAM(MIN_PARTICLES, MAX_PARTICLES));
		SendMessage(hTrackParticles, TBM_SETPOS, TRUE, INITIAL_PSO_PARTICLES);

		CreateWindow(TEXT("button"), TEXT("↓"), WS_CHILD|WS_VISIBLE,
			10, 595, 25, 25, hWnd, (HMENU)ID_BTN_N_STAGES_DOWN, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("↑"), WS_CHILD|WS_VISIBLE,
			45, 595, 25, 25, hWnd, (HMENU)ID_BTN_N_STAGES_UP, g_hInst, NULL);
		CreateWindow(TEXT("static"), TEXT("5"), WS_CHILD|WS_VISIBLE,
			80, 595, 80, 25, hWnd, (HMENU)ID_STATIC_NUM_STAGES, g_hInst, NULL);
		hTrackStages = CreateWindow(TRACKBAR_CLASS, NULL, WS_CHILD|WS_VISIBLE,
			170, 595, 400, 30, hWnd, NULL, g_hInst, NULL);
		SendMessage(hTrackStages, TBM_SETRANGE, FALSE, MAKELPARAM(MIN_STAGES, MAX_STAGES));
		SendMessage(hTrackStages, TBM_SETPOS, TRUE, INITIAL_PSO_STAGES);
		
		// Init Model ListControl
		CreateWindow(TEXT("static"), TEXT("detector list"), WS_CHILD|WS_VISIBLE|SS_CENTER,
			670, 10, 100, 25, hWnd, (HMENU)ID_STATIC_PROC_TIME, g_hInst, NULL);
		hModelList = CreateWindow(TEXT("listbox"), NULL, WS_CHILD|WS_VISIBLE|WS_BORDER|
			WS_VSCROLL|LBS_NOTIFY, 670, 35, 400, 100, hWnd, (HMENU)ID_LST_MODEL_LIST, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("Delete"), WS_CHILD|WS_VISIBLE,
			 760, 140, 80, 25,hWnd, (HMENU)ID_BTN_MODEL_DELETE, g_hInst, NULL);

		// Add Button
		hBtnSetting = CreateWindow(TEXT("button"), TEXT("Add"), WS_CHILD|WS_VISIBLE,
			670, 140, 80, 25, hWnd, (HMENU)ID_BTNSETTING, g_hInst, NULL);

		// Color Setting
		CreateWindow(TEXT("BUTTON"), TEXT("Color Setting"),
			WS_CHILD|WS_VISIBLE|BS_GROUPBOX, 
			680,180,160,200,hWnd, (HMENU)-1, g_hInst, NULL);
		hStaticRed = CreateWindow(TEXT("static"), TEXT("RED"), WS_CHILD|WS_VISIBLE|SS_RIGHT,
			700, 210, 55, 25, hWnd, (HMENU)-1, g_hInst, NULL);
		hStaticGreen = CreateWindow(TEXT("static"), TEXT("GREEN"), WS_CHILD|WS_VISIBLE|SS_RIGHT,
			700, 240, 55, 25, hWnd, (HMENU)-1, g_hInst, NULL);
		hStaticBlue = CreateWindow(TEXT("static"), TEXT("BLUE"), WS_CHILD|WS_VISIBLE|SS_RIGHT,
			700, 270, 55, 25, hWnd, (HMENU)-1, g_hInst, NULL);
		hStaticThick = CreateWindow(TEXT("static"), TEXT("Thick"), WS_CHILD|WS_VISIBLE|SS_RIGHT,
			700, 300, 55, 25, hWnd, (HMENU)-1, g_hInst, NULL);
		CreateWindow(TEXT("edit"), TEXT("255"), WS_CHILD|WS_VISIBLE|WS_BORDER,
			760, 210, 60, 25, hWnd, (HMENU)ID_EDIT_RED, g_hInst, NULL);
		CreateWindow(TEXT("edit"), TEXT("0"), WS_CHILD|WS_VISIBLE|WS_BORDER,
			760, 240, 60, 25, hWnd, (HMENU)ID_EDIT_GREEN, g_hInst, NULL);
		CreateWindow(TEXT("edit"), TEXT("0"), WS_CHILD|WS_VISIBLE|WS_BORDER,
			760, 270, 60, 25, hWnd, (HMENU)ID_EDIT_BLUE, g_hInst, NULL);
		CreateWindow(TEXT("edit"), TEXT("5"), WS_CHILD|WS_VISIBLE|WS_BORDER,
			760, 300, 60, 25, hWnd, (HMENU)ID_EDIT_THICK, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("Apply"), WS_CHILD|WS_VISIBLE,
			 750, 350, 70, 25,hWnd, (HMENU)ID_BTN_RGB_APPLY, g_hInst, NULL);
		
		// Draw Rectangle Magnification
		CreateWindow(TEXT("BUTTON"), TEXT("Draw Magnification"),
			WS_CHILD|WS_VISIBLE|BS_GROUPBOX, 
			850,180,160,200,hWnd, (HMENU)-1, g_hInst, NULL);
		CreateWindow(TEXT("static"), TEXT("LR"), WS_CHILD|WS_VISIBLE|SS_RIGHT,
			870, 210, 55, 25, hWnd, (HMENU)-1, g_hInst, NULL);
		CreateWindow(TEXT("edit"), TEXT("1.0"), WS_CHILD|WS_VISIBLE|WS_BORDER,
			930, 210, 60, 25, hWnd, (HMENU)ID_EDIT_MAG_LR, g_hInst, NULL);
		CreateWindow(TEXT("static"), TEXT("TB"), WS_CHILD|WS_VISIBLE|SS_RIGHT,
			870, 240, 55, 25, hWnd, (HMENU)-1, g_hInst, NULL);
		CreateWindow(TEXT("edit"), TEXT("1.0"), WS_CHILD|WS_VISIBLE|WS_BORDER,
			930, 240, 60, 25, hWnd, (HMENU)ID_EDIT_MAG_TB, g_hInst, NULL);
		CreateWindow(TEXT("button"), TEXT("Apply"), WS_CHILD|WS_VISIBLE,
			 920, 350, 70, 25,hWnd, (HMENU)ID_BTN_MAG_APPLY, g_hInst, NULL);

		// Default Parameters
		memset(_godparm.model_file_name, 0, sizeof(char)*_MAX_PATH);
		_godparm.scale_factor = 1.25;
		_godparm.subwindow_width = 24;
		_godparm.subwindow_height = 24;
		_godparm.initial_scale_factor_index=INITIAL_SCALE_FACTOR_INDEX;

		// For Detection
		_gimg = dinocv_create_image(&dinocv_set_size(WIDTH, HEIGHT), 24);
		_ggray = dinocv_create_image(&dinocv_set_size(WIDTH, HEIGHT), 8);

		// Related PSO and Color of Draw Rectangle
		for(int i = 0 ; i < NUMBER_OF_MODELS ; i++)
		{
			_global_particles[i] = INITIAL_PSO_PARTICLES;
			_global_stage[i] = INITIAL_PSO_STAGES;
		}

		/* VFW Setting */
		hCaptureWindow = capCreateCaptureWindow(TEXT("Huro Competition - Object Detection"), WS_CHILD|WS_VISIBLE,
			10, 10, 640+10, 480+10, hWndMain, NULL);


		return 0;
	case WM_PAINT:
		hdc=BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		dinocv_release_image(_gimg);
		dinocv_release_image(_ggray);

		// Detection Stop
		capDriverDisconnect(hCaptureWindow);

		PostQuitMessage(0);
		return 0;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case ID_BTNSETTING: // Setting Button
			if(DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DLGSETTING), hWnd, SettingDlgProc)==IDOK){
			}
			break;
		case ID_BTNDOWNSTAGE: // Stage Down Button
			if(_godparm.model_file_name[0] != '\0')
			{
				if(cd[_gCurrentSelectModel]->n_sc > 4)
				{
					wsprintf(wstrPath, TEXT("%d"), --cd[_gCurrentSelectModel]->n_sc);
					SetDlgItemText(hWnd, ID_STATICSTAGE, wstrPath);
				}
			}
			break;
		case ID_BTNUPSTAGE: // Stage Up Button
			if(_godparm.model_file_name[0] != '\0')
			{
				if(cd[_gCurrentSelectModel]->n_sc < cd[_gCurrentSelectModel]->n_sc_max)
				{
					wsprintf(wstrPath, TEXT("%d"), ++cd[_gCurrentSelectModel]->n_sc);
					SetDlgItemText(hWnd, ID_STATICSTAGE, wstrPath);
				}
			}
			break;
		case ID_BTN_N_PARTICLES_DOWN:
			if(_gCurrentSelectModel >= 0)
			{
				if(_global_particles[_gCurrentSelectModel] > MIN_PARTICLES)
				{
					wsprintf(wstrPath, TEXT("%d"), --_global_particles[_gCurrentSelectModel]);
					SetDlgItemText(hWnd, ID_STATIC_NUM_PARTICLES, wstrPath);
					SendMessage(hTrackParticles, TBM_SETPOS, TRUE, _global_particles[_gCurrentSelectModel]);
				}
				
			}
			break;
		case ID_BTN_N_PARTICLES_UP:
			if(_gCurrentSelectModel >= 0)
			{
				if(_global_particles[_gCurrentSelectModel] < MAX_PARTICLES)
				{
					wsprintf(wstrPath, TEXT("%d"), ++_global_particles[_gCurrentSelectModel]);
					SetDlgItemText(hWnd, ID_STATIC_NUM_PARTICLES, wstrPath);
					SendMessage(hTrackParticles, TBM_SETPOS, TRUE, _global_particles[_gCurrentSelectModel]);
				}
			}
			break;
		case ID_BTN_N_STAGES_DOWN:
			if(_gCurrentSelectModel >= 0)
			{
				if(_global_stage[_gCurrentSelectModel] > MIN_STAGES)
				{
					wsprintf(wstrPath, TEXT("%d"), --_global_stage[_gCurrentSelectModel]);
					SetDlgItemText(hWnd, ID_STATIC_NUM_STAGES, wstrPath);
					SendMessage(hTrackStages, TBM_SETPOS, TRUE, _global_stage[_gCurrentSelectModel]);
				}
			}
			break;
		case ID_BTN_N_STAGES_UP:
			if(_gCurrentSelectModel >= 0)
			{
				if(_global_stage[_gCurrentSelectModel] < MAX_STAGES)
				{
					wsprintf(wstrPath, TEXT("%d"), ++_global_stage[_gCurrentSelectModel]);
					SetDlgItemText(hWnd, ID_STATIC_NUM_STAGES, wstrPath);
					SendMessage(hTrackStages, TBM_SETPOS, TRUE, _global_stage[_gCurrentSelectModel]);
				}
			}
			break;
		case ID_RBTN_SWO:
			isPSO = false;
			init_avg_time_calculator();
			break;
		case ID_RBTN_PSO:
			isPSO = true;
			init_avg_time_calculator();
			break;
		case ID_BTN_RGB_APPLY:
			if(_gCurrentSelectModel >= 0)
			{
				GetDlgItemText(hWnd, ID_EDIT_RED, wstrPath, MAX_PATH);
				WideCharToMultiByte(CP_ACP, 0, wstrPath, -1, str, sizeof(str), NULL, NULL);
				cd[_gCurrentSelectModel]->color.r = (int)atoi(str);
				GetDlgItemText(hWnd, ID_EDIT_GREEN, wstrPath, MAX_PATH);
				WideCharToMultiByte(CP_ACP, 0, wstrPath, -1, str, sizeof(str), NULL, NULL);
				cd[_gCurrentSelectModel]->color.g = (int)atoi(str);
				GetDlgItemText(hWnd, ID_EDIT_BLUE, wstrPath, MAX_PATH);
				WideCharToMultiByte(CP_ACP, 0, wstrPath, -1, str, sizeof(str), NULL, NULL);
				cd[_gCurrentSelectModel]->color.b = (int)atoi(str);
				GetDlgItemText(hWnd, ID_EDIT_THICK, wstrPath, MAX_PATH);
				WideCharToMultiByte(CP_ACP, 0, wstrPath, -1, str, sizeof(str), NULL, NULL);
				cd[_gCurrentSelectModel]->thick = (int)atoi(str);
			}
			break;
		case ID_BTN_MAG_APPLY:
			if(_gCurrentSelectModel >= 0)
			{
				GetDlgItemText(hWnd, ID_EDIT_MAG_LR, wstrPath, MAX_PATH);
				WideCharToMultiByte(CP_ACP, 0, wstrPath, -1, str, sizeof(str), NULL, NULL);
				cd[_gCurrentSelectModel]->lr=(float)atof(str);
				GetDlgItemText(hWnd, ID_EDIT_MAG_TB, wstrPath, MAX_PATH);
				WideCharToMultiByte(CP_ACP, 0, wstrPath, -1, str, sizeof(str), NULL, NULL);
				cd[_gCurrentSelectModel]->tb=(float)atof(str);
			}
			break;
		case ID_LST_MODEL_LIST:
			switch(HIWORD(wParam))
			{
			case LBN_SELCHANGE: // List Box Sel Change
				_gCurrentSelectModel=SendMessage(hModelList, LB_GETCURSEL, 0, 0);

				// info display about current selected model 
				wsprintf(wstrPath, TEXT("%d"), cd[_gCurrentSelectModel]->n_sc);// cd_total_stage display
				SetDlgItemText(hWndMain, ID_STATICSTAGE, wstrPath);
				
				wsprintf(wstrPath, TEXT("%d"), _global_particles[_gCurrentSelectModel]);// pso parm
				SetDlgItemText(hWnd, ID_STATIC_NUM_PARTICLES, wstrPath);
				SendMessage(hTrackParticles, TBM_SETPOS, TRUE, _global_particles[_gCurrentSelectModel]);
				wsprintf(wstrPath, TEXT("%d"), _global_stage[_gCurrentSelectModel]);
				SetDlgItemText(hWnd, ID_STATIC_NUM_STAGES, wstrPath);
				SendMessage(hTrackStages, TBM_SETPOS, TRUE, _global_stage[_gCurrentSelectModel]);
				InvalidateRect(hWnd, &_grt_particles, TRUE);

				// COLOR & THICK INFO
				wsprintf(wstrPath, TEXT("%d"), cd[_gCurrentSelectModel]->color.r);
				SetDlgItemText(hWnd, ID_EDIT_RED, wstrPath);
				wsprintf(wstrPath, TEXT("%d"), cd[_gCurrentSelectModel]->color.g);
				SetDlgItemText(hWnd, ID_EDIT_GREEN, wstrPath);
				wsprintf(wstrPath, TEXT("%d"), cd[_gCurrentSelectModel]->color.b);
				SetDlgItemText(hWnd, ID_EDIT_BLUE, wstrPath);
				wsprintf(wstrPath, TEXT("%d"), cd[_gCurrentSelectModel]->thick);
				SetDlgItemText(hWnd, ID_EDIT_THICK, wstrPath);

				// MAG INFO
				swprintf(wstrPath, MAX_PATH, TEXT("%2.2lf"), cd[_gCurrentSelectModel]->lr);
				SetDlgItemText(hWnd, ID_EDIT_MAG_LR, wstrPath);
				swprintf(wstrPath, MAX_PATH, TEXT("%2.2lf"), cd[_gCurrentSelectModel]->tb);
				SetDlgItemText(hWnd, ID_EDIT_MAG_TB, wstrPath);


				break;	
			}
			break;
		case ID_BTN_MODEL_DELETE:
			if(SendMessage(hModelList, LB_GETCOUNT, NULL, NULL) == 0)
				break;

			// Detection Stop
			capDriverDisconnect(hCaptureWindow);


			SendMessage(hModelList, LB_DELETESTRING, (WPARAM)_gCurrentSelectModel, 0);
			
			release_cascaded_detector(cd[_gCurrentSelectModel]);
			soc_list_free(candidate_list[_gCurrentSelectModel]); // list 안에 있는 데이터도 제거해야 함
			soc_list_free(tracking_list[_gCurrentSelectModel]); // list 안에 있는 데이터도 제거해야 함

			int localCurrentModel = _gCurrentSelectModel;
			for(int i = 0 ; i < _gNumberOfModel - _gCurrentSelectModel-1 ; i++)
			{
				cd[localCurrentModel] = cd[localCurrentModel+1];
				candidate_list[localCurrentModel] = candidate_list[localCurrentModel+1];
				localCurrentModel++;
			}

			//candidate_list[_gCurrentSelectModel] = (LIST_D *)malloc(sizeof(LIST_D));

			_gCurrentSelectModel = SendMessage(hModelList, LB_GETCOUNT, NULL, NULL)-1;
			SendMessage(hModelList, LB_SETCURSEL, _gCurrentSelectModel, 0);

			if(--_gNumberOfModel > 0)
			{
				// Detection Start
				if(capSetCallbackOnFrame(hCaptureWindow, CallbackOnFrame) == FALSE)
					return false;

				if(capDriverConnect(hCaptureWindow, 0) == FALSE)
					return FALSE;

				capPreviewRate(hCaptureWindow, 30);
				capOverlay(hCaptureWindow, false);
				capPreview(hCaptureWindow, true);
			}
			break;
		}
		return 0;
	case WM_HSCROLL:
		if((HWND)lParam == hTrackParticles)
		{
			_global_particles[_gCurrentSelectModel] = SendMessage(hTrackParticles, TBM_GETPOS, 0, 0);
			wsprintf(wstrPath, TEXT("%d"), _global_particles[_gCurrentSelectModel]);
			SetDlgItemText(hWnd, ID_STATIC_NUM_PARTICLES, wstrPath);
			InvalidateRect(hWnd, &_grt_particles, TRUE);
		}
		else if((HWND)lParam == hTrackStages)
		{
			_global_stage[_gCurrentSelectModel] = SendMessage(hTrackStages, TBM_GETPOS, 0, 0);
			wsprintf(wstrPath, TEXT("%d"), _global_stage[_gCurrentSelectModel]);
			SetDlgItemText(hWnd, ID_STATIC_NUM_STAGES, wstrPath);
			InvalidateRect(hWnd, &_grt_stages, TRUE);
		}
		break;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

BOOL CALLBACK SettingDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HWND hComboDetectionSize;
	TCHAR wstrPath[MAX_PATH]={NULL,};
	char strPath[MAX_PATH]={NULL,};

	TCHAR lpStrFile[MAX_PATH] = {NULL, };
	OPENFILENAME OFN;

	switch(iMessage){
	case WM_INITDIALOG:
		if(_godparm.model_file_name[0]!='\0')
		{
			MultiByteToWideChar(CP_ACP, 0, _godparm.model_file_name, -1, wstrPath, strlen(_godparm.model_file_name)*2);
			SetDlgItemText(hDlg, IDC_EDIT_MODEL_PATH, wstrPath);
		}

		swprintf(wstrPath, MAX_PATH, TEXT("%2.2lf"), _godparm.scale_factor);
		SetDlgItemText(hDlg, IDC_EDIT_SF, wstrPath);

		wsprintf(wstrPath, TEXT("%d"), _godparm.subwindow_width);
		SetDlgItemText(hDlg, IDC_EDIT_WIDTH, wstrPath);
		
		wsprintf(wstrPath, TEXT("%d"), _godparm.subwindow_height);
		SetDlgItemText(hDlg, IDC_EDIT_HEIGHT, wstrPath);

		wsprintf(wstrPath, TEXT("%d"), INITIAL_COLOR_RED); // Red
		SetDlgItemText(hDlg, IDC_EDIT_RED, wstrPath);
		wsprintf(wstrPath, TEXT("%d"), INITIAL_COLOR_BLUE); // Green
		SetDlgItemText(hDlg, IDC_EDIT_BLUE, wstrPath);
		wsprintf(wstrPath, TEXT("%d"), INITIAL_COLOR_GREEN); // Blue
		SetDlgItemText(hDlg, IDC_EDIT_GREEN, wstrPath);
		wsprintf(wstrPath, TEXT("%d"), INITIAL_COLOR_THICK); // Thick
		SetDlgItemText(hDlg, IDC_EDIT_THICK, wstrPath);


		for(int i = 0 ; i < N_LUT_SF ; i++)
		{
			wsprintf(wstrPath, TEXT("%d"), i);
			SendDlgItemMessage(hDlg, IDC_COMBO_MDS, CB_ADDSTRING, 0, (LPARAM)wstrPath);
		}
		hComboDetectionSize = GetDlgItem(hDlg, IDC_COMBO_MDS);
		//SendMessage(hComboDetectionSize, 
		SendMessage(hComboDetectionSize, CB_SETCURSEL, _godparm.initial_scale_factor_index, 0);


		capDriverDisconnect(hCaptureWindow); // 검출기 중지

		//if(_godparm.model_file_name[0] != '\0')
			//release_cascaded_detector(cd); // 검출 데이터 릴리즈
		return TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hDlg, IDC_EDIT_MODEL_PATH, wstrPath, MAX_PATH);
			WideCharToMultiByte(CP_ACP, 0, wstrPath, -1, _godparm.model_file_name, sizeof(_godparm.model_file_name), NULL, NULL);

			GetDlgItemText(hDlg, IDC_EDIT_SF, wstrPath, MAX_PATH);
			WideCharToMultiByte(CP_ACP, 0, wstrPath, -1, strPath, sizeof(strPath), NULL, NULL);
			_godparm.scale_factor=(float)atof(strPath);

			GetDlgItemText(hDlg, IDC_EDIT_WIDTH, wstrPath, MAX_PATH);
			WideCharToMultiByte(CP_ACP, 0, wstrPath, -1, strPath, sizeof(strPath), NULL, NULL);
			_godparm.subwindow_width=(int)atoi(strPath);

			GetDlgItemText(hDlg, IDC_EDIT_HEIGHT, wstrPath, MAX_PATH);
			WideCharToMultiByte(CP_ACP, 0, wstrPath, -1, strPath, sizeof(strPath), NULL, NULL);
			_godparm.subwindow_height=(int)atoi(strPath);

			// Detector Model Load
			cd[_gNumberOfModel]=load_cascaded_detector(_godparm.model_file_name, _godparm.subwindow_width, _godparm.subwindow_height,
				_godparm.initial_scale_factor_index, _godparm.scale_factor, N_LUT_SF, sizeof(DETECTION_RESULT)*10000, _gimg->width, _gimg->height);
			cd[_gNumberOfModel]->lr=1.0;
			cd[_gNumberOfModel]->tb=1.0;
			MultiByteToWideChar(CP_ACP, 0, _godparm.model_file_name, -1, wstrPath, strlen(_godparm.model_file_name)*2);
			SendMessage(hModelList, LB_ADDSTRING, 0, (LPARAM)PathFindFileName(wstrPath));

			
			
			//LB_SETCURSEL
			_gCurrentSelectModel = SendMessage(hModelList, LB_GETCOUNT, 0, 0)-1;
			SendMessage(hModelList, LB_SETCURSEL, _gCurrentSelectModel, NULL);
			
			// cd_total_stage display
			wsprintf(wstrPath, TEXT("%d"), cd[_gCurrentSelectModel]->n_sc_max);
			SetDlgItemText(hWndMain, ID_STATICSTAGE, wstrPath);

			// color & thick
			GetDlgItemText(hDlg, IDC_EDIT_RED, wstrPath, MAX_PATH);
			WideCharToMultiByte(CP_ACP, 0, wstrPath, -1, strPath, sizeof(strPath), NULL, NULL);
			cd[_gNumberOfModel]->color.r=(int)atoi(strPath);
			GetDlgItemText(hDlg, IDC_EDIT_GREEN, wstrPath, MAX_PATH);
			WideCharToMultiByte(CP_ACP, 0, wstrPath, -1, strPath, sizeof(strPath), NULL, NULL);
			cd[_gNumberOfModel]->color.g=(int)atoi(strPath);
			GetDlgItemText(hDlg, IDC_EDIT_BLUE, wstrPath, MAX_PATH);
			WideCharToMultiByte(CP_ACP, 0, wstrPath, -1, strPath, sizeof(strPath), NULL, NULL);
			cd[_gNumberOfModel]->color.b=(int)atoi(strPath);
			GetDlgItemText(hDlg, IDC_EDIT_THICK, wstrPath, MAX_PATH);
			WideCharToMultiByte(CP_ACP, 0, wstrPath, -1, strPath, sizeof(strPath), NULL, NULL);
			cd[_gNumberOfModel]->thick=(int)atoi(strPath);

			// COLOR & THICK INFO
			wsprintf(wstrPath, TEXT("%d"), cd[_gCurrentSelectModel]->color.r);
			SetDlgItemText(hWndMain, ID_EDIT_RED, wstrPath);
			wsprintf(wstrPath, TEXT("%d"), cd[_gCurrentSelectModel]->color.g);
			SetDlgItemText(hWndMain, ID_EDIT_GREEN, wstrPath);
			wsprintf(wstrPath, TEXT("%d"), cd[_gCurrentSelectModel]->color.b);
			SetDlgItemText(hWndMain, ID_EDIT_BLUE, wstrPath);
			wsprintf(wstrPath, TEXT("%d"), cd[_gCurrentSelectModel]->thick);
			SetDlgItemText(hWndMain, ID_EDIT_THICK, wstrPath);

			// DRAW MAG INFO
			swprintf(wstrPath, MAX_PATH, TEXT("1.0"));
			SetDlgItemText(hWndMain, ID_EDIT_MAG_LR, wstrPath);
			swprintf(wstrPath, MAX_PATH, TEXT("1.0"));
			SetDlgItemText(hWndMain, ID_EDIT_MAG_TB, wstrPath);

			_gNumberOfModel++;


			// Detection Start
			if(capSetCallbackOnFrame(hCaptureWindow, CallbackOnFrame) == FALSE)
			{
				MessageBox(hWndMain, TEXT("capSetCallbackOnFrame failed\n"), TEXT("Can't set callback on frame"), MB_OK);
				EndDialog(hDlg, IDOK);
				return FALSE;
			}

			if(capDriverConnect(hCaptureWindow, 0) == FALSE)
			{
				MessageBox(hWndMain, TEXT("capDriverConnect failed\n"), TEXT("Can't connect driver"), MB_OK);
				EndDialog(hDlg, IDOK);
				return FALSE;
			}
			
			capPreviewRate(hCaptureWindow, 30);
			capOverlay(hCaptureWindow, false);
			capPreview(hCaptureWindow, true);

			// time average
			init_avg_time_calculator();

			// candidate/tracking list added
			
			candidate_list[_gCurrentSelectModel] = soc_list_malloc();
			tracking_list[_gCurrentSelectModel] = soc_list_malloc();
			
			EndDialog(hDlg, IDOK);
			return TRUE;
		case IDCANCEL:
			memset(_godparm.model_file_name, 0, sizeof(char)*_MAX_PATH);

			wsprintf(wstrPath, TEXT("detector not loaded."));
			SetDlgItemText(hWndMain, ID_STATICSTAGE, wstrPath);

			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		case IDC_BUTTON_DIALOG:
			memset(&OFN, 0, sizeof(OPENFILENAME));
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = hWndMain;
			OFN.lpstrFilter = TEXT("모델 파일(*.model)\0*.model\0");
			OFN.lpstrFile = lpStrFile;
			OFN.nMaxFile = MAX_PATH;

			if(GetOpenFileName(&OFN))
			{
				SetDlgItemText(hDlg, IDC_EDIT_MODEL_PATH, OFN.lpstrFile);
			}

			return TRUE;
		case IDC_COMBO_MDS:
			if(HIWORD(wParam) == CBN_SELCHANGE)
			{
				hComboDetectionSize = GetDlgItem(hDlg, IDC_COMBO_MDS);
				_godparm.initial_scale_factor_index = SendMessage(hComboDetectionSize, CB_GETCURSEL, 0, 0);
			}
			return TRUE;
		}

		return TRUE;
	}
	return FALSE;
}

void init_avg_time_calculator()
{
	division=0;
	npt=0;
	cpt=0;
	total_time=0;
	memset(time_arr, 0, sizeof(int) * N);
}