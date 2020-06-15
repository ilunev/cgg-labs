#include "../lab1/pnm.h"
#include "correct.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define USAGE "<input> <output> <sharpen>"


struct Args {
	char *inp;
	char *out;
	double sharpen;
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
	if (argc != 4)
		return 1;

	args->inp = argv[1];
	args->out = argv[2];

	if (parse_double(&args->sharpen, argv[3]) != 0)
		return 1;

	if (args->sharpen < 0. || args->sharpen > 1.)
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


int main(int argc, char **argv)
{
	struct PNMImage img = pnm_image();
	FILE *f = NULL;
	int status = 0;

#define ERRQUIT status = 1; goto cleanup;

	struct Args args;
	if (parse_args(&args, argc, argv) != 0) {
		print_usage(argv[0]);
		ERRQUIT
	}

	f = fopen(args.inp, "rb");
	if (!f) {
		print_file_error(args.inp);
		ERRQUIT
	}

	if (read_pnm_image(&img, f) != PNM_SUCCESS) {
		fprintf(stderr, "`%s` is not a PNM file\n", args.inp);
		ERRQUIT
	}

	fclose(f); f = NULL;

	correct(&img, args.sharpen);

	f = fopen(args.out, "wb");
	if (!f) {
		print_file_error(args.out);
		ERRQUIT
	}

	if (write_pnm_image(&img, f) != PNM_SUCCESS) {
		print_file_error(args.out);
		ERRQUIT
	}

#undef ERRQUIT

cleanup:
	free_pnm_image_data(&img);
	if (f)
		fclose(f);

	return status;
}
