#include "../lab1/pnm.h"
#include <correct.h>
#include <math.h>

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
#define MIN5(a, b, c, d, e) (MIN(a, MIN(b, MIN(c, MIN(d, e)))))
#define MAX5(a, b, c, d, e) (MAX(a, MAX(b, MAX(c, MAX(d, e)))))


double getindex(struct PNMImage *img, int h, int w, int chan)
{
	return img->data[pnm_px_offset(img, h, w) + chan] / (double) img->max_value;
}


unsigned char corrected_px(struct PNMImage *img, int y, int x, int chan, double sh)
{
	double a = getindex(img, y - 1, x - 1, chan);
	double b = getindex(img, y - 1, x, chan);
	double c = getindex(img, y - 1, x + 1, chan);
	double d = getindex(img, y, x - 1, chan);
	double e = getindex(img, y, x, chan);
	double f = getindex(img, y, x + 1, chan);
	double g = getindex(img, y + 1, x - 1, chan);
	double h = getindex(img, y + 1, x, chan);
	double i = getindex(img, y + 1, x + 1, chan);

	double min = MIN5(b, d, e, f, h);
	min += MIN5(min, a, c, g, i);

	double max = MAX5(b, d, e, f, h);
	max += MAX5(max, a, c, g, i);

	double base = sqrt(MIN(min, 2 - max) / max);
	double w = -base / (8 - 3 * sh);

	double corrected = (b*w + d*w + e + f*w + h*w) / (4 * w + 1);

	return MIN(img->max_value, MAX(0, corrected * img->max_value));
}


void correct(struct PNMImage *img, double sharpen)
{
	struct PNMImage tmp = pnm_image();
	tmp.type = img->type;
	tmp.height = img->height;
	tmp.width = img->width;
	tmp.max_value = img->max_value;

	alloc_pnm_image_data(&tmp);

	for (unsigned i = 0; i < pnm_data_size(img); ++i)
		tmp.data[i] = img->data[i];

	for (int c = 0; c < pnm_pixel_bytes(tmp.type); ++c) {
		for (int h = 1; h < tmp.height - 1; ++h)
			for (int w = 1; w < tmp.width - 1; ++w) {
				unsigned idx = pnm_px_offset(img, h, w);
				img->data[idx + c] = corrected_px(&tmp, h, w, c, sharpen);
			}
	}

	free_pnm_image_data(&tmp);
}
