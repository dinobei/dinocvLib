#include "../common/def_os_selector_export.h"
#include "ppmanal.h"

int annotate_test(FILE *fp)
{
	char temp;
#ifdef _WIN32
	fscanf_s(fp, "%c", &temp); // # ?
#else
	fscanf(fp, "%c", &temp); // # ?
#endif

	if(temp=='#')
	{
		do{
#ifdef _WIN32
			fscanf_s(fp, "%c", &temp);
#else
			fscanf(fp, "%c", &temp);
#endif
		}while(temp != '\n');
		return 1;
	}
	fseek(fp, -1, SEEK_CUR);
	return 0;
}

IMAGE_D *_dinocv_ppm_read(char *filename)
{
	register unsigned int i, j;
	FILE* fp;
	IMAGE_D *ret_img;
	char M, N;
	int max_val;
	uint_d width, height;
	SIZE_D sz;

	if(filename == NULL)
		return NULL;
#ifdef _WIN32
	fopen_s(&fp, filename, "rb"); // binary
#else
	fp = fopen(filename, "rb"); // binary
#endif
	if(fp == NULL)
		return NULL;

	// read magic number
	while(annotate_test(fp));
#ifdef _WIN32
	fscanf_s(fp, "%c%c\n", &M, &N);
#else
	fscanf(fp, "%c%c\n", &M, &N);
#endif

	// Check whether P6 Format or not
	while(annotate_test(fp));
	if(M != 'P' || N != '6')
		return NULL;

	// read width, height
	while(annotate_test(fp));
#ifdef _WIN32
	fscanf_s(fp, "%d %d\n", &width, &height);
#else
	fscanf(fp, "%d %d\n", &width, &height);
#endif

	// read maximum value
	while(annotate_test(fp));
#ifdef _WIN32
	fscanf_s(fp, "%d\n", &max_val);
#else
	fscanf(fp, "%d\n", &max_val);
#endif
	if(max_val != 255)
		return NULL;

	sz = dinocv_set_size(width, height);
	ret_img = dinocv_create_image(&sz, 24);
	//ret_img->channel=3; // default 3 cuz support only rgb model	
	
	for(i = 0 ; i < height ; i++)
		for(j = 0 ; j < width*3 ; j++)
			fread(&ret_img->source[i][j], sizeof(uchar_d), 1, fp);

	return ret_img;
}

IMAGE_D *_dinocv_pgm_read(char *filename)
{
	register unsigned int i, j;
	FILE* fp;
	IMAGE_D *ret_img;
	char M, N;
	int max_val;
	uint_d width, height;
	SIZE_D sz;

	if(filename == NULL)
		return NULL;
#ifdef _WIN32
	fopen_s(&fp, filename, "rb"); // binary
#else
	fp = fopen(filename, "rb"); // binary
#endif
	if(fp == NULL)
		return NULL;

	// read magic number
	while(annotate_test(fp));
#ifdef _WIN32
	fscanf_s(fp, "%c%c\n", &M, &N);
#else
	fscanf(fp, "%c%c\n", &M, &N);
#endif

	// Check whether P6 Format or not
	while(annotate_test(fp));
	if(M != 'P' || N != '5')
		return NULL;

	// read width, height
	while(annotate_test(fp));
#ifdef _WIN32
	fscanf_s(fp, "%d %d\n", &width, &height);
#else
	fscanf(fp, "%d %d\n", &width, &height);
#endif

	// read maximum value
	while(annotate_test(fp));
#ifdef _WIN32
	fscanf_s(fp, "%d\n", &max_val);
#else
	fscanf(fp, "%d\n", &max_val);
#endif
	if(max_val != 255)
		return NULL;

	sz = dinocv_set_size(width, height);
	ret_img = dinocv_create_image(&sz, 8);
	//ret_img->channel=3; // default 3 cuz support only rgb model	
	
	for(i = 0 ; i < height ; i++)
		for(j = 0 ; j < width ; j++)
			fread(&ret_img->source[i][j], sizeof(uchar_d), 1, fp);

	return ret_img;
}
