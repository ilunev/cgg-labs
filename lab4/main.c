#include "../lab1/pnm.h"
#include "convert.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>


#define USAGE \
	"-f <from_colorspace> -t <to_colorspace> " \
	"-i <count> <input_files> -o <count> <output_files>"


char is_valid_colorspace_name(char *n)
{
	if (
		!strcmp(n, "RGB")
		|| !strcmp(n, "HSL")
		|| !strcmp(n, "HSV")
		|| !strcmp(n, "YCbCr.601")
		|| !strcmp(n, "YCbCr.709")
		|| !strcmp(n, "YCoCg")
		|| !strcmp(n, "CMY")
	)
		return 1;
	return 0;
}


struct Args {
	char *from_colorspace;
	char *to_colorspace;
	char inp_count;
	char *inp[3];
	char out_count;
	char *out[3];
};


char parse_count(char *str)
{
	if (!strcmp(str, "1"))
		return 1;
	else if (!strcmp(str, "3"))
		return 3;
	else
		return 0;
}


int parse_args(struct Args *args, int argc, char **argv)
{
	args->from_colorspace = args->to_colorspace = args->inp[0] = args->out[0] = NULL;

	for (int i = 1; i < argc; ++i) {

		if (!strcmp(argv[i], "-f")) {
			if (argc - i < 2)
				return 1;

			if (is_valid_colorspace_name(argv[++i]))
				args->from_colorspace = argv[i];
			else
				return 1;

		} else if (!strcmp(argv[i], "-t")) {
			if (argc - i < 2)
				return 1;

			if (is_valid_colorspace_name(argv[++i]))
				args->to_colorspace = argv[i];
			else
				return 1;

		} else if (!strcmp(argv[i], "-i")) {
			if (argc - i < 2)
				return 1;

			if (!(args->inp_count = parse_count(argv[++i])))
				return 1;

			if (i + args->inp_count > argc)
				return 1;

			for (int j = 0; j < args->inp_count; ++j)
				args->inp[j] = argv[++i];

		} else if (!strcmp(argv[i], "-o")) {
			if (argc - i < 2)
				return 1;

			if (!(args->out_count = parse_count(argv[++i])))
				return 1;

			if (i + args->out_count > argc)
				return 1;

			for (int j = 0; j < args->out_count; ++j)
				args->out[j] = argv[++i];
		
		} else {
			return 1;
		}
	}

	if (!args->from_colorspace || !args->to_colorspace || !args->inp[0] || !args->out[0])
		return 1;

	return 0;
}


void print_usage(char *exe)
{
	fprintf(stderr, "%s %s\n", exe, USAGE);
}


void print_file_error(char *filename)
{
	fprintf(stderr, "File error with `%s`\n", filename);
}


int read_count_1(struct PNMImage *img, struct Args *args)
{
	FILE *f = fopen(args->inp[0], "rb");
	if (!f) {
		print_file_error(args->inp[0]);
		return 1;
	}

	if (read_pnm_image(img, f) != PNM_SUCCESS || img->type != PNM_PIXMAP) {
		fprintf(stderr, "`%s` is not a PPM file\n", args->inp[0]);
		free_pnm_image_data(img);
		fclose(f);
		return 1;
	}
	return 0;
}


int read_count_3(struct PNMImage *img, struct Args *args)
{
	img->type = PNM_PIXMAP;
	img->width = img->height = UINT_MAX;
	img->max_value = 255;

	struct PNMImage t = pnm_image();

	for (int i = 0; i < 3; ++i) {
		FILE *f = fopen(args->inp[i], "rb");
		if (!f) {
			print_file_error(args->inp[i]);
			free_pnm_image_data(&t);
			return 1;
		}

		if (read_pnm_image(&t, f) != PNM_SUCCESS || t.type != PNM_GRAYMAP) {
			fprintf(stderr, "`%s` is not a PGM file\n", args->inp[i]);
			free_pnm_image_data(&t);
			fclose(f);
			return 1;
		}

		if (img->width == UINT_MAX) {
			img->width = t.width;
			img->height = t.height;
			alloc_pnm_image_data(img);

		} else if (img->width != t.width || img->height != t.height) {
			fprintf(stderr, "PGM files should have the same size\n");
			free_pnm_image_data(&t);
			fclose(f);
			return 1;
		}

		for (unsigned h = 0; h < t.height; ++h)
			for (unsigned w = 0; w < t.height; ++w) {
				unsigned img_idx = pnm_px_offset(img, h, w) + i;
				unsigned t_idx = pnm_px_offset(&t, h, w);
				img->data[img_idx] = t.data[t_idx];
			}

		fclose(f);
	}

	return 0;
}


int write_count_1(struct PNMImage *img, struct Args *args)
{
	FILE *f = fopen(args->out[0], "wb");
	if (!f) {
		print_file_error(args->out[0]);
		return 1;
	}
	if (write_pnm_image(img, f) != PNM_SUCCESS) {
		print_file_error(args->out[0]);
		fclose(f);
		return 1;
	}

	fclose(f);
	return 0;
}


int write_count_3(struct PNMImage *img, struct Args *args)
{
	struct PNMImage t = pnm_image();
	t.type = PNM_GRAYMAP;
	t.width = img->width;
	t.height = img->height;
	t.max_value = img->max_value;
	alloc_pnm_image_data(&t);

	for (int i = 0; i < 3; ++i) {
		for (unsigned h = 0; h < t.height; ++h)
			for (unsigned w = 0; w < t.height; ++w) {
				unsigned img_idx = pnm_px_offset(img, h, w) + i;
				unsigned t_idx = pnm_px_offset(&t, h, w);
				t.data[t_idx] = img->data[img_idx];
			}

		FILE *f = fopen(args->out[i], "wb");
		if (!f) {
			print_file_error(args->out[i]);
			free_pnm_image_data(&t);
			return 1;
		}
		if (write_pnm_image(&t, f) != PNM_SUCCESS) {
			print_file_error(args->out[i]);
			free_pnm_image_data(&t);
			fclose(f);
			return 1;
		}

		fclose(f);
	}

	return 0;
}


int main(int argc, char **argv)
{
	struct PNMImage img = pnm_image();
	int status = 0;

#define ERRQUIT status = 1; goto cleanup;

	struct Args args;
	if (parse_args(&args, argc, argv) != 0) {
		print_usage(argv[0]);
		ERRQUIT
	}

	if (args.inp_count == 1) {
		if (read_count_1(&img, &args) != 0) {
			ERRQUIT
		}
	} else {
		if (read_count_3(&img, &args) != 0) {
			ERRQUIT
		}
	}

	convert(&img, args.from_colorspace, args.to_colorspace);
	
	if (args.out_count == 1) {
		if (write_count_1(&img, &args) != 0) {
			ERRQUIT
		}
	} else {
		if (write_count_3(&img, &args) != 0) {
			ERRQUIT
		}
	}

#undef ERRQUIT

cleanup:
	free_pnm_image_data(&img);

	return status;
}
