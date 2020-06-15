#include "../lab1/pnm.h"
#include "line.h"
#include <math.h>


#define SWAP(a, b) { typeof(a) t = a; a = b; b = t; } while (0)


struct Line line_init(double x0, double y0, double x1, double y1, double w)
{
	struct Line l;

	if (fabs(y1 - y0) > fabs(x1 - x0)) {
		l.is_transposed = 1;
		SWAP(x0, y0);
		SWAP(x1, y1);
	} else {
		l.is_transposed = 0;
	}

	if (x0 > x1) {
		SWAP(x0, x1);
		SWAP(y0, y1);
	}

	double dx = x1 - x0;
	double dy = y1 - y0;

	l.x0 = x0 - .5;
	l.y0 = y0 - .5;
	l.x1 = x1 - .5;
	l.y1 = y1 - .5;
	l.width = w;

	l.k = dy / dx;

	if (dy == 0.) {
		l.k = 0.;
		l.perp_k = HUGE_VAL;
		l.half_vert_w = w / 2;
	} else if (dx == 0.) {
		l.k = HUGE_VAL;
		l.perp_k = 0.;
		l.half_vert_w = 0.;
	} else {
		l.k = dy / dx;
		l.perp_k = -dx / dy;
		l.half_vert_w = sqrt(pow(w * l.k, 2) + pow(w, 2)) / 2;
	}

	return l;
}


double kline_x(double y, double x0, double y0, double k)
{
	return k == HUGE_VAL ? x0 : (y - y0) / k + x0;
}


double math_y(struct Line *l, double x)
{
	return (l->y1 - l->y0) / (l->x1 - l->x0) * (x - l->x0) + l->y0;
}


double to_visible_srgb(unsigned char b, unsigned char max_b)
{
	double u = ((double) b) / max_b;
	return u <= .04045 ? u / 12.92 : pow((u + .055) / 1.055, 2.4);
}


double to_visible(unsigned char b, unsigned char max_b, double gamma)
{
	return gamma ? pow(((double) b) / max_b, gamma) : to_visible_srgb(b, max_b);
}


unsigned char to_stored_srgb(double b, unsigned char max_b)
{
	return (b <= .0031308 ? 12.92 * b : 1.055 * pow(b, 1 / 2.4) - .055) * max_b;
}


unsigned char to_stored(double b, unsigned char max_b, double gamma)
{
	return gamma ? pow(b, 1 / gamma) * max_b : to_stored_srgb(b, max_b);
}


void plot(struct DrawLineTask *t, int x_px, int y_px, double coef)
{
	if (t->line->is_transposed) {
		SWAP(x_px, y_px);
	}

	if (x_px < 0 || x_px >= t->img->width || y_px < 0 || y_px >= t->img->height)
		return;

	struct PNMImage *img = t->img;

	unsigned i = pnm_px_offset(img, y_px, x_px);

	double frnt = to_visible(t->brightness, img->max_value, t->gamma);
	double back = to_visible(img->data[i], img->max_value, t->gamma);

	double mix = frnt * coef + back * (1 - coef);

	img->data[i] = to_stored(mix, img->max_value, t->gamma);
}


double brightness_coef(struct Line *l, int x_px, int y_px)
{
	int match_count = 0;

	for (double x = x_px - .5 + .05; x < x_px + .5; x += .1) {
		double line_y = math_y(l, x);
		double upper = line_y - l->half_vert_w;
		double lower = line_y + l->half_vert_w;

		for (double y = y_px - .5 + .05; y < y_px + .5; y += .1) {
			if (y < upper || y > lower)
				continue;

			double left = kline_x(y, l->x0, l->y0, l->perp_k);
			double right = kline_x(y, l->x1, l->y1, l->perp_k);

			if (x < left || x > right)
				continue;

			++match_count;
		}
	}

	return match_count / 100.;
}


void stroke(struct DrawLineTask *t, int x_px)
{
	double line_y = math_y(t->line, x_px);
	int upper_px = floor(line_y - t->line->half_vert_w + .5) - 1;
	int lower_px = floor(line_y + t->line->half_vert_w + .5) + 1;

	for (int y_px = upper_px; y_px <= lower_px; ++y_px)
		plot(t, x_px, y_px, brightness_coef(t->line, x_px, y_px));
}


void draw_line(struct DrawLineTask *t)
{
	int x_px = floor(t->line->x0 - t->line->width / 2 + .5);
	int last_px = floor(t->line->x1 + t->line->width / 2 + .5);

	for (; x_px <= last_px; ++x_px)
		stroke(t, x_px);
}
