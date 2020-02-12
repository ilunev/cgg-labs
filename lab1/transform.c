#include "transform.h"
#include "pnm.h"
#include <string.h>


void swap(unsigned char *a, unsigned x, unsigned y, unsigned length)
{
	for (unsigned i = 0; i < length; i++) {
		unsigned char t = a[x];
		a[x++] = a[y];
		a[y++] = t;
	}
}


void inverse(struct PNMImage *img)
{
	for (unsigned i = 0; i < pnm_data_size(img); i++)
		img->data[i] = img->max_value - img->data[i];
}


void mirror_horizontal(struct PNMImage *img)
{
	unsigned px_len = pnm_pixel_bytes(img->type);
	unsigned row_last = img->width - 1;
	unsigned h, w1, w2;

	for (h = 0; h < img->height; h++)
		for (w1 = 0, w2 = row_last; w1 < w2; w1++, w2--)
			swap(
				img->data,
				pnm_px_offset(img, h, w1),
				pnm_px_offset(img, h, w2),
				px_len
			);
}


void mirror_vertical(struct PNMImage *img)
{
	unsigned px_len = pnm_pixel_bytes(img->type);
	unsigned col_last = img->height - 1;
	unsigned w, h1, h2;

	for (w = 0; w < img->width; w++)
		for (h1 = 0, h2 = col_last; h1 < h2; h1++, h2--)
			swap(
				img->data,
				pnm_px_offset(img, h1, w),
				pnm_px_offset(img, h2, w),
				px_len
			);
}


int prepare_rotated(const struct PNMImage *old, struct PNMImage *new)
{
	new->type = old->type;
	new->width = old->height;
	new->height = old->width;
	new->max_value = old->max_value;
	return alloc_pnm_image_data(new);
}


enum PNMStatus rotate_right(const struct PNMImage *old, struct PNMImage *new)
{
	if (prepare_rotated(old, new) != 0)
		return PNM_MEMORY_ERROR;

	unsigned px_len = pnm_pixel_bytes(old->type);
	unsigned h, hn, w, wn;

	for (h = 0, wn = new->width - 1; h < old->height; h++, wn--)
		for (w = 0, hn = 0; w < old->width; w++, hn++)
			memcpy(
				new->data + pnm_px_offset(new, hn, wn),
				old->data + pnm_px_offset(old, h, w),
				px_len
			);

	return PNM_SUCCESS;
}


enum PNMStatus rotate_left(const struct PNMImage *old, struct PNMImage *new)
{
	if (prepare_rotated(old, new) != 0)
		return PNM_MEMORY_ERROR;

	unsigned px_len = pnm_pixel_bytes(old->type);
	unsigned h, hn, w, wn;

	for (h = 0, wn = 0; h < old->height; h++, wn++)
		for (w = 0, hn = new->height - 1; w < old->width; w++, hn--)
			memcpy(
				new->data + pnm_px_offset(new, hn, wn),
				old->data + pnm_px_offset(old, h, w),
				px_len
			);

	return PNM_SUCCESS;
}
