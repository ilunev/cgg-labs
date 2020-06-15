#ifndef LINE_H
#define LINE_H

#include "../lab1/pnm.h"

struct Line {
	double x0;
	double y0;
	double x1;
	double y1;
	double width;
	double k;
	double perp_k;
	double half_vert_w;
	char is_transposed;
};

struct Line line_init(double x0, double y0, double x1, double y1, double w);

struct DrawLineTask {
	struct PNMImage *img;
	struct Line *line;
	unsigned char brightness;
	double gamma;
};

void draw_line(struct DrawLineTask *);

#endif // LINE_H
