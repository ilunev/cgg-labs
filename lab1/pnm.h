#ifndef PNM_H
#define PNM_H

#include <stdio.h>

enum PNMType {
	PNM_GRAYMAP,
	PNM_PIXMAP
};

enum PNMStatus {
	PNM_SUCCESS = 0,
	PNM_INVALID_FORMAT,
	PNM_UNSUPPORTED_FORMAT,
	PNM_MEMORY_ERROR,
	PNM_READ_ERROR,
	PNM_WRITE_ERROR
};

struct PNMImage {
	enum PNMType type;
	unsigned width;
	unsigned height;
	unsigned char max_value;
	unsigned char *data;
};

struct PNMImage pnm_image();

unsigned pnm_pixel_bytes(enum PNMType);
unsigned pnm_data_size(const struct PNMImage *);
unsigned pnm_px_offset(const struct PNMImage *, unsigned h, unsigned w);

int alloc_pnm_image_data(struct PNMImage *);
void free_pnm_image_data(struct PNMImage *);

enum PNMStatus read_pnm_image(struct PNMImage *, FILE *);
enum PNMStatus write_pnm_image(const struct PNMImage *, FILE *);

#endif // PNM_H
