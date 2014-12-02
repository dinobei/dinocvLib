#pragma comment(lib, "../Debug/dinocvLib_100d.lib")
#include "../../source/dinocvlib.h"

int main()
{
	IMAGE_D *img = dinocv_load_image("yuv422_sample.yuv", &dinocv_set_size(800,480), 0, IMGMDL_YUV422);
	dinocv_save_bitmap("resumt.bmp", img);
	dinocv_release_image(img);
}
