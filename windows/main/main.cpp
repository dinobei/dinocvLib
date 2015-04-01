#include <dinocore.h>
#include <dinoproc.h>

int main()
{
	IMAGE_D *img = dinocv_load_image("yuv422_sample.yuv", &dinocv_set_size(800,480), 0, IMGMDL_YUV422);
	printf("loading image complete\n");

	dinocv_binarization(img, BIN_OTSU);

	dinocv_save_bitmap("resumt.bmp", img);
	printf("saving bitmap complete\n");

	dinocv_release_image(img);
	printf("releasing image complete\n");
}
