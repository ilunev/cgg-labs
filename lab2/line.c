#include "../lab1/pnm.h"
#include "line.h"
#include <math.h>


void count_perp_k(struct Line *l)
{
	double dy = l->y1 - l->y0;
	if (dy != 0.) {
		double dx = l->x1 - l->x0;
		l->_k = -dx / dy;
	} else {
		l->_k = HUGE_VAL;
	}
}


double kline_x(double y, double x0, double y0, double k)
{
	return k == HUGE_VAL ? x0 : (y - y0) / k + x0;
}


double math_y(struct Line *l, double x)
{
	return (l->y1 - l->y0) / (l->x1 - l->x0) * (x - l->x0) + l->y0;
}


double to_visible(unsigned char b, unsigned char max_b, double gamma)
{
	return pow(((double) b) / max_b, gamma);
}


unsigned char to_stored(double b, unsigned char max_b, double gamma)
{
	return pow(b, 1 / gamma) * max_b;
}


void plot(struct DrawLineTask *t, int x, int y, double coef, char is_tr)
{
	if (x < 0 || x >= t->img->width || y < 0 || y >= t->img->height)
		return;

	if (is_tr) {
		int tmp = x;
		x = y;
		y = tmp;
	}

	struct PNMImage *img = t->img;

	unsigned i = pnm_px_offset(img, y, x);

	double frnt = to_visible(t->brightness, img->max_value, t->gamma);
	double back = to_visible(img->data[i], img->max_value, t->gamma);

	double mix = frnt * coef + back * (1 - coef);

	img->data[i] = to_stored(mix, img->max_value, t->gamma);
}


double cut_to_normal(double x)
{
	if (x > 1.)
		x = 1.;
	else if (x < 0.)
		x = 0.;
	return x;
}


double brightness_coef(
	int x, int y,
	double upper, double lower, double left, double right
)
{
	double upper_impl = cut_to_normal(upper - (y - .5));
	double lower_impl = cut_to_normal(y + .5 - lower);
	double left_impl = cut_to_normal(left - .5 - (x - .5));
	double right_impl = cut_to_normal(x + .5 - (right + .5));
	double impl = upper_impl + lower_impl + left_impl + right_impl;
	impl = cut_to_normal(impl);
	return 1. - impl;
}


void stroke(struct DrawLineTask *t, int x, char is_tr)
{
	double line_y = math_y(t->line, x);

	double upper = line_y - t->width / 2;
	double lower = line_y + t->width / 2;

	int y = floor(upper + .5);
	int bound = floor(lower + .5);

	for (; y <= bound; y++) {
		double left = kline_x(
			y, t->line->x0, t->line->y0, t->line->_k
		);
		double right = kline_x(
			y, t->line->x1, t->line->y1, t->line->_k
		);

		double coef = brightness_coef(
			x, y, upper, lower, left, right
		);

		plot(t, x, y, coef, is_tr);
	}
}


void transpose(struct Line *l)
{
	double t = l->x0;
	l->x0 = l->y0;
	l->y0 = t;
	t = l->x1;
	l->x1 = l->y1;
	l->y1 = t;
}


void flip(struct Line *l)
{
	double t = l->x0;
	l->x0 = l->x1;
	l->x1 = t;
	t = l->y0;
	l->y0 = l->y1;
	l->y1 = t;
}


void draw_line(struct DrawLineTask *t)
{
	struct Line *line = t->line;
	char is_transposed = 0;

	if (fabs(line->x1 - line->x0) < fabs(line->y1 - line->y0)) {
		is_transposed = 1;
		transpose(line);
	}

	if (line->x0 > line->x1) {
		flip(line);
	}

	count_perp_k(t->line);

	int x = floor(t->line->x0 - t->width / 2 + .5);
	int x1 = floor(t->line->x1 + t->width / 2 + .5);

	for (; x <= x1; x++)
		stroke(t, x, is_transposed);

	if (is_transposed)
		transpose(line);
}
