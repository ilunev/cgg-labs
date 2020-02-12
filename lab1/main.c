#include <stdio.h>
#include <string.h>
#include "pnm.h"
#include "transform.h"


enum Operation {
	INVERSE = '0',
	MIRROR_HORIZONTAL,
	MIRROR_VERTICAL,
	ROTATE_RIGHT,
	ROTATE_LEFT
};


void report_usage(char *exe)
{
	fprintf(stderr, "Usage: %s <input_path> <output_path> <0-4>\n", exe);
}


void report_fopen_error(char *filename)
{
	fprintf(stderr, "Failed to open file \"%s\"\n", filename);
}


void report_pnm_error(enum PNMStatus s)
{
	char *msg;

	if (s == PNM_INVALID_FORMAT)
		msg = "Input file is not a valid PNM file\n";
	else if (s == PNM_UNSUPPORTED_FORMAT)
		msg = "Input file format is unsupported\n";
	else if (s == PNM_MEMORY_ERROR)
		msg = "Memory error occurred\n";
	else if (s == PNM_READ_ERROR)
		msg = "Failed to read from input file\n";
	else
		msg = "Failed to write to output file\n";

	fprintf(stderr, msg);
}


int main(int argc, char **argv)
{
	if (argc != 4) {
		report_usage(argv[0]);
		return 1;
	}

	char op = *argv[3];
	if (strlen(argv[3]) != 1 || op < '0' || op > '4') {
		report_usage(argv[0]);
		return 1;
	}

	FILE *inp = fopen(argv[1], "rb");
	if (inp == NULL) {
		report_fopen_error(argv[1]);
		return 1;
	}

	FILE *out = fopen(argv[2], "wb");
	if (out == NULL) {
		report_fopen_error(argv[2]);
		fclose(inp);
		return 1;
	}

	int code = 0;
	struct PNMImage img = pnm_image();

	enum PNMStatus s = read_pnm_image(&img, inp);
	if (s != PNM_SUCCESS) {
		report_pnm_error(s);
		code = 1;
		goto cleanup;
	}

	if (op == INVERSE) {
		inverse(&img);

	} else if (op == MIRROR_HORIZONTAL) {
		mirror_horizontal(&img);

	} else if (op == MIRROR_VERTICAL) {
		mirror_vertical(&img);

	} else {
		struct PNMImage new;

		if (op == ROTATE_RIGHT)
			s = rotate_right(&img, &new);
		else
			s = rotate_left(&img, &new);

		if (s != PNM_SUCCESS) {
			report_pnm_error(s);
			code = 1;
			goto cleanup;
		}

		free_pnm_image_data(&img);
		img = new;
	}

	s = write_pnm_image(&img, out);
	if (s != PNM_SUCCESS) {
		report_pnm_error(s);
		code = 1;
		goto cleanup;
	}

cleanup:
	fclose(inp);
	fclose(out);
	free_pnm_image_data(&img);
	return code;
}
