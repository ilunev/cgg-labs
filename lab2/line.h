#ifndef LINE_H
#define LINE_H

#include "../lab1/pnm.h"

struct Line {
	double x0;
	double y0;
	double x1;
	double y1;
	double _k;
};

struct DrawLineTask {
	struct PNMImage *img;
	struct Line *line;
	double width;
	unsigned char brightness;
	double gamma;
};

void draw_line(struct DrawLineTask *);

#endif // LINE_H
