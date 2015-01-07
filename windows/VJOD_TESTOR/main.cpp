#include <stdio.h>
#include <dinocvlib.h>
#include "dinotime.h"
#include "Classifier.h"
#include "list.h"

int main()
{
	DWatch watch;
	double total_time=0.0;

	IMAGE_D *img, *timg;
	CASCADED_DETECTOR_D *cd;
	LIST_D *cl, *tl;
	int i;
	int iteration = 5000;

	int nParticles = 15;
	int nStages = 5;
	
	cd = load_cascaded_detector("cascaded_detector_real4.model", 24, 24,
		3, 1.25, 25,
		100000,
		1, 1);
	if(cd == NULL)
	{
		printf("Invalid detector model\n");
		return -1;
	}

	cl = soc_list_malloc();
	tl = soc_list_malloc();

	for(i = 0 ; i < iteration ; i++)
	{
		img = dinocv_load_image("puneet.bmp", NULL, 0, IMGMDL_RGB);
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
		}

		watch.Start();

		cascaded_classify_with_tpso(img, cd,
				cl, tl,
				nParticles, nStages);

		//cascaded_classify_with_swo(cd, img);

		watch.End();
		printf("\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\r\rProcessing : %4.0lf %%", (double)i/iteration*100.);

		total_time += watch.GetDurationMilliSecond();

		dinocv_release_image(img);
	}

	printf("\t[Complete]\nAverage time : %5.3lf msec\n", total_time/iteration);

	release_cascaded_detector(cd);
	soc_list_free(tl);
	soc_list_free(cl);

	return 0;
}
