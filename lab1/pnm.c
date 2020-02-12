#include "pnm.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define FORMAT_ID 'P'
#define GRAYMAP_ID '5'
#define PIXMAP_ID '6'


struct PNMImage pnm_image()
{
	struct PNMImage img = { PNM_GRAYMAP, 0, 0, 0, NULL };
	return img;
}


unsigned pnm_pixel_bytes(enum PNMType t)
{
	return t == PNM_GRAYMAP ? 1 : 3;
}


unsigned pnm_data_size(const struct PNMImage *img)
{
	return img->height * img->width * pnm_pixel_bytes(img->type);
}


unsigned pnm_px_offset(const struct PNMImage *img, unsigned h, unsigned w)
{
	unsigned depth = pnm_pixel_bytes(img->type);
	return h * img->width * depth + w * depth;
}


int alloc_pnm_image_data(struct PNMImage *img)
{
	img->data = malloc(pnm_data_size(img) * sizeof(unsigned char));
	if (img->data == NULL)
		return 1;
	return 0;
}


void free_pnm_image_data(struct PNMImage *img)
{
	free(img->data);
}


int validate_data(const struct PNMImage *img)
{
	for (unsigned i = 0; i < pnm_data_size(img); i++)
		if (img->data[i] > img->max_value)
			return 1;
	return 0;
}


int read_data(struct PNMImage *img, FILE *f)
{
	unsigned size = pnm_data_size(img);

	if (fread(img->data, 1, size, f) < size)
		return 1;
	return 0;
}


enum PNMStatus read_pnm_image(struct PNMImage *img, FILE *f)
{
	if (fgetc(f) != FORMAT_ID)
		return PNM_INVALID_FORMAT;

	unsigned char id = fgetc(f);
	switch (id) {
	case GRAYMAP_ID:
		img->type = PNM_GRAYMAP;
		break;
	case PIXMAP_ID:
		img->type = PNM_PIXMAP;
		break;
	default:
		return PNM_UNSUPPORTED_FORMAT;
	}

	if (fscanf(
		f, "%u %u %hhu\n",
		&img->width,
		&img->height,
		&img->max_value
	) == EOF)
		return PNM_INVALID_FORMAT;

	if (alloc_pnm_image_data(img) != 0)
		return PNM_MEMORY_ERROR;

	if (read_data(img, f) != 0)
		return PNM_READ_ERROR;

	if (validate_data(img) != 0)
		return PNM_INVALID_FORMAT;

	return PNM_SUCCESS;
}


int write_data(const struct PNMImage *img, FILE *f)
{
	unsigned size = pnm_data_size(img);

	if (fwrite(img->data, 1, size, f) < size)
		return 1;
	return 0;
}


enum PNMStatus write_pnm_image(const struct PNMImage *img, FILE *f)
{
	if (fprintf(
		f, "%c%c\n%u %u\n%u\n",
		FORMAT_ID,
		img->type == PNM_GRAYMAP ? GRAYMAP_ID : PIXMAP_ID,
		img->width,
		img->height,
		img->max_value
	) < 0)
		return PNM_WRITE_ERROR;

	if (write_data(img, f) != 0)
		return PNM_WRITE_ERROR;

	return PNM_SUCCESS;
}
