#include "../lab1/pnm.h"
#include "line.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define USAGE \
	"<input> <output> <brightness (0..255)> <width (>0)> " \
	"<x0> <y0> <x1> <y1> [<gamma (>0)>]"


struct Args {
	char *inp;
	char *out;
	unsigned char brightness;
	double width;
	struct Line line;
	double gamma;
};


int parse_double(double *dst, const char *str)
{
	char *end = NULL;
	double d = strtod(str, &end);
	if (*end || d == HUGE_VAL || d == -HUGE_VAL)
		return 1;
	*dst = d;
	return 0;
}


int parse_args(struct Args *args, int argc, char **argv)
{
	if (argc < 9 || argc > 10)
		return 1;

	args->inp = argv[1];
	args->out = argv[2];

	char *end = NULL;
	long brightness = strtol(argv[3], &end, 10);
	if (*end || brightness < 0 || brightness > 255)
		return 1;
	args->brightness = brightness;

	if (parse_double(&args->width, argv[4]) != 0 || args->width <= 0)
		return 1;

	int err = 0;
	err += parse_double(&args->line.x0, argv[5]);
	err += parse_double(&args->line.y0, argv[6]);
	err += parse_double(&args->line.x1, argv[7]);
	err += parse_double(&args->line.y1, argv[8]);
	if (err)
		return 1;

	if (argc < 10) {
		args->gamma = 2.2;
		return 0;
	}

	if (parse_double(&args->gamma, argv[9]) != 0 || args->gamma <= 0)
		return 1;

	return 0;
}


void print_usage(char *exe)
{
	fprintf(stderr, "%s %s\n", exe, USAGE);
}


int main(int argc, char **argv)
{
	FILE *f = NULL;
	struct PNMImage img = pnm_image();
	int status = 0;

#define ERRQUIT status = 1; goto cleanup;

	struct Args args;
	if (parse_args(&args, argc, argv) != 0) {
		print_usage(argv[0]);
		ERRQUIT
	}

	f = fopen(args.inp, "rb");
	if (f == NULL) {
		fprintf(
			stderr,
			"Failed to open %s for reading\n",
			args.inp
		);
		ERRQUIT
	}

	if (read_pnm_image(&img, f) != PNM_SUCCESS) {
		fprintf(stderr, "Failed to read PNM image\n");
		ERRQUIT
	}

	fclose(f);
	f = NULL;

	if (img.type != PNM_GRAYMAP) {
		fprintf(stderr, "Only PNM P5 images are supported\n");
		ERRQUIT
	}

	if (img.max_value < args.brightness) {
		fprintf(
			stderr,
			"Max brightness for this is image is %d.\n",
			img.max_value
		);
		ERRQUIT
	}

	f = fopen(args.out, "wb");
	if (f == NULL) {
		fprintf(
			stderr,
			"Failed to open %s for writing\n",
			args.out
		);
		ERRQUIT
	}

	struct DrawLineTask task = {
		&img,
		&args.line,
		args.width,
		args.brightness,
		args.gamma
	};

	draw_line(&task);

	if (write_pnm_image(&img, f) != PNM_SUCCESS) {
		fprintf(stderr, "Failed to write PNM image\n");
		ERRQUIT
	}

#undef ERRQUIT

cleanup:
	if (f != NULL)
		fclose(f);
	free_pnm_image_data(&img);

	return status;
}
