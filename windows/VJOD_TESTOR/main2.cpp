// This code is for speed test of each SoC Board

#include <stdio.h>
#include <dinocore.h>
#include "dinotime.h"
#include "Classifier.h"
#include "list.h"

int test_time;
int main(int argc, char **argv)
{
	DWatch watch;
	double total_time=0.0;
	int total_cnt=0;

	IMAGE_D *img, *timg;
	CASCADED_DETECTOR_D *cd;
	LIST_D *cl, *tl;
	int i;
	int nParticles = 4;
	int nStages = 11;
	
	if(argc != 3)
	{
		printf("Usage : ./%s [image_name] [test time(msec)]\n", argv[0]);
		return -1;
	}
	test_time = atoi(argv[2]);

	cd = load_cascaded_detector("cascaded_detector_real4.model", 24, 24,
		3, 1.25, 25,
		100000,
		1, 1);
	if(cd == NULL)
	{
		printf("Invalid detector model\n");
		return -1;
	}





	printf("nParticles : %d, nStages : %d\n\n", nParticles, nStages);			

	cl = soc_list_malloc();
	tl = soc_list_malloc();

	printf("TPSO Scanning Performance Test\n");
	while(1)
	{
		img = dinocv_load_image(argv[1], NULL, 0, IMGMDL_RGB);
		if(img->bpp != 8 && img->bpp != 24)
		{
			printf("Invalid input file\n");
			continue;
		}
		if(img->bpp == 24)
		{
			timg = dinocv_conv_24to8(img);
			dinocv_release_image(img);
			img = timg;
			timg = NULL;
			dinocv_save_bitmap(argv[1], img);
		}

		watch.Start();

		cascaded_classify_with_tpso(img, cd,
			cl, tl,
			nParticles, nStages);

		watch.End();
		printf("\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\rProcessing : %4.0lf %% (tl:%d)", (double)total_time/test_time*100., tl->cnt);

		total_time += watch.GetDurationMilliSecond();
		total_cnt++;
		/*
		for(i = 0 ; i < tl->cnt ; i++)
		{
			TRACKING_OBJECT *to = (TRACKING_OBJECT *)soc_list_get_idx_data(tl, i);
			int x = TO_GET_AVG_X(to);
			int y = TO_GET_AVG_Y(to);
			int w = TO_GET_AVG_W(to);
			int h = TO_GET_AVG_H(to);
			dinocv_draw_rect(img,
				&dinocv_set_rect(
				x-(w>>1),
				y-(h>>1),
				x+(w>>1),
				y+(h>>1)
				),
				&dinocv_set_color(255, 0, 0), 5);
		}
		dinocv_save_bitmap("tpso_result.bmp", img);
		*/
		dinocv_release_image(img);

		if(total_time >= test_time)
			break;
	}
	printf("\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\rProcessing : %4.0lf %% (tl:%d)", (double)100., tl->cnt);

	/*
	img = dinocv_load_image(argv[1], NULL, 0, IMGMDL_RGB);
	for(i = 0 ; i < tl->cnt ; i++)
	{
		TRACKING_OBJECT *to = (TRACKING_OBJECT *)soc_list_get_idx_data(tl, i);
		int x = TO_GET_AVG_X(to);
		int y = TO_GET_AVG_Y(to);
		int w = TO_GET_AVG_W(to);
		int h = TO_GET_AVG_H(to);
		dinocv_draw_rect(img,
			&dinocv_set_rect(
			x-(w>>1),
			y-(h>>1),
			x+(w>>1),
			y+(h>>1)
			),
			&dinocv_set_color(255, 0, 0), 5);
	}
	dinocv_save_bitmap("tpso_result.bmp", img);
	dinocv_release_image(img);
	*/
	printf("\t[Complete]\nAverage time : %5.3lf msec, %5.3lf fps\n", total_time/total_cnt, 1000./(total_time/total_cnt));


	total_time=0.0;
	total_cnt=0;
	printf("\nSWO Scanning Performance Test\n");
	while(1)
	{
		img = dinocv_load_image(argv[1], NULL, 0, IMGMDL_RGB);
		if(img->bpp != 8 && img->bpp != 24)
		{
			printf("Invalid input file\n");
			return -1;
		}
		if(img->bpp == 24)
		{
			timg = dinocv_conv_24to8(img);
			dinocv_release_image(img);
			img = timg;
			timg = NULL;
			dinocv_save_bitmap(argv[1], img);
		}

		watch.Start();

		cascaded_classify_with_swo(cd, img);

		watch.End();
		printf("\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\rProcessing : %4.0lf %% (num:%d)", (double)total_time/test_time*100., cd->merged_detection_result->n_objects);

		total_time += watch.GetDurationMilliSecond();
		total_cnt++;

		dinocv_release_image(img);

		if(total_time >= test_time)
			break;
	}
	printf("\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\rProcessing : %4.0lf %% (num:%d)", (double)100., cd->merged_detection_result->n_objects);

	img = dinocv_load_image(argv[1], NULL, 0, IMGMDL_RGB);
	for(i = 0 ; i < cd->merged_detection_result->n_objects ; i++)
	{
		dinocv_draw_rect(img, &cd->merged_detection_result->p_rt[i], &dinocv_set_color(255, 0, 0), 5);
	}
	dinocv_save_bitmap("swo_result.bmp", img);
	dinocv_release_image(img);
	printf("\t[Complete]\nAverage time : %5.3lf msec, %5.3lf fps\n", total_time/total_cnt, 1000./(total_time/total_cnt));




	release_cascaded_detector(cd);
	soc_list_free(tl);
	soc_list_free(cl);
	
	return 0;
}
