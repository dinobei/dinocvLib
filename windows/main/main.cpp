#include <dinocore.h>
#include <dinoproc.h>

int main()
{
	IMAGE_D *img = dinocv_load_image("yuv422_sample.yuv", &dinocv_set_size(800,480), 0, IMGMDL_YUV422);
	printf("loading image complete\n");

	IMAGE_D *gray = dinocv_conv_24to8(img);
	dinocv_binarization(gray, BIN_OTSU);
	printf("image binarization complete\n");

	IMAGE_D *lut_image = dinocv_lut_proc(gray);
	printf("processing image lookup tables complete\n");

	dinocv_save_bitmap("bin.bmp", gray);
	dinocv_save_bitmap("lut_result.bmp", lut_image);
	dinocv_save_bitmap("result.bmp", img);
	printf("saving bitmap complete\n");

	dinocv_release_image(gray);
	dinocv_release_image(lut_image);
	dinocv_release_image(img);
	printf("releasing image complete\n");
}
