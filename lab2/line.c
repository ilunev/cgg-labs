#include "../lab1/pnm.h"
#include "line.h"
#include <math.h>


double kline_y(double x, double x0, double y0, double k)
{
	return k * (x - x0) + y0;
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


void plot(struct DrawLineTask *t, int x, int y, double intensity, char is_tr)
{
	if (is_tr) {
		int tmp = x;
		x = y;
		y = tmp;
	}

	struct PNMImage *img = t->img;

	if (x < 0 || x >= img->width || y < 0 || y >= img->height)
		return;

	unsigned i = pnm_px_offset(img, y, x);

	double frnt = to_visible(t->brightness, img->max_value, t->gamma);
	double back = to_visible(img->data[i], img->max_value, t->gamma);

	double mix = frnt * intensity + back * (1 - intensity);

	img->data[i] = to_stored(mix, img->max_value, t->gamma);
}


void stroke(struct DrawLineTask *t, int x, double y, char is_transposed)
{

	double up = y - t->width / 2;
	double down = y + t->width / 2;

	int up_px = floor(up + 0.5);
	int down_px = (down + 0.5);

	double up_intensity = up_px - up + 0.5;
	double down_intensity = down - down_px + 0.5;

	int no_higher = up_px;
	int no_lower = down_px;

	double k = t->line->_k;
	if (k != HUGE_VAL) {
		no_higher = kline_y(x, t->line->x0, t->line->y0, k) + 0.5;
		no_lower = kline_y(x, t->line->x1, t->line->y1, k) + 0.5;
		if (no_lower < no_higher) {
			int tmp = no_lower;
			no_lower = no_higher;
			no_higher = tmp;
		}
		if (up_px > no_lower || down_px < no_higher)
			return;
	}

	double coef = t->width >= 1. ? 1. : t->width;

	if (up_px == down_px) {
		plot(t, x, up_px, coef, is_transposed);
		return;
	}

	if (up_px >= no_higher)
		plot(t, x, up_px, up_intensity * coef, is_transposed);
	if (down_px <= no_lower)
		plot(t, x, down_px, down_intensity * coef, is_transposed);

	for (int y_px = up_px + 1; y_px < down_px; y_px++)
		if (y_px >= no_higher && y_px <= no_lower)
			plot(t, x, y_px, coef, is_transposed);
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

	int x0, x1;

	double dy = line->y1 - line->y0;
	double dx = line->x1 - line->x0;
	if (dy != 0) {
		line->_k = -dx / dy;
		x0 = line->x0 - t->width / 2;
		x1 = line->x1 + t->width / 2;
	} else {
		line->_k = HUGE_VAL;
		x0 = line->x0;
		x1 = line->x1;
		if (dx == 0)
			return stroke(t, x0, line->y0, is_transposed);
	}

	for (int x = x0; x <= x1; x++)
		stroke(t, x, math_y(line, x), is_transposed);

	if (is_transposed)
		transpose(line);
}
