#include "convert.h"
#include "../lab1/pnm.h"
#include <math.h>
#include <string.h>


#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)


typedef void (*converter)(double, double, double, double *, double *, double *);


void hsl_rgb(double h, double s, double l, double *R, double *G, double *B)
{
	double c = (1. - fabs(2 * l - 1)) * s;
	double h_stroke = h * 360. / 60.;
	double x = c * (1. - fabs(fmod(h_stroke, 2.) - 1));

	double r1 = 0., g1 = 0., b1 = 0.;

	if (h_stroke <= 1) {
		r1 = c; g1 = x; b1 = 0;
	} else if (h_stroke <= 2) {
		r1 = x; g1 = c; b1 = 0;
	} else if (h_stroke <= 3) {
		r1 = 0; g1 = c; b1 = x;
	} else if (h_stroke <= 4) {
		r1 = 0; g1 = x; b1 = c;
	} else if (h_stroke <= 5) {
		r1 = x; g1 = 0; b1 = c;
	} else if (h_stroke <= 6) {
		r1 = c; g1 = 0; b1 = x;
	}

	double m = l - c / 2.;

	*R = r1 + m;
	*G = g1 + m;
	*B = b1 + m;
}


void hsv_rgb(double h, double s, double v, double *R, double *G, double *B)
{
	double c = v * s;
	double h_stroke = h * 360. / 60.;
	double x = c * (1. - fabs(fmod(h_stroke, 2.) - 1));

	double r1 = 0., g1 = 0., b1 = 0.;

	if (h_stroke <= 1) {
		r1 = c; g1 = x; b1 = 0;
	} else if (h_stroke <= 2) {
		r1 = x; g1 = c; b1 = 0;
	} else if (h_stroke <= 3) {
		r1 = 0; g1 = c; b1 = x;
	} else if (h_stroke <= 4) {
		r1 = 0; g1 = x; b1 = c;
	} else if (h_stroke <= 5) {
		r1 = x; g1 = 0; b1 = c;
	} else if (h_stroke <= 6) {
		r1 = c; g1 = 0; b1 = x;
	}

	double m = v - c;

	*R = r1 + m;
	*G = g1 + m;
	*B = b1 + m;
}


void rgb_hsv(double r, double g, double b, double *H, double *S, double *V)
{
	double x_max = MAX(MAX(r, g), b);
	double x_min = MIN(MIN(r, g), b);
	double c = x_max - x_min;
	
	*V = x_max;

	if (c == 0.)
		*H = 0.;
	else if (*V == r)
		*H = ((g - b) / c) / 6.;
	else if (*V == g)
		*H = (2. + (b - r) / c) / 6.;
	else
		*H = (4. + (r - g) / c) / 6.;

	*S = *V == 0. ? 0. : c / *V;

}


void rgb_hsl(double r, double g, double b, double *H, double *S, double *V)
{
	double x_max = MAX(MAX(r, g), b);
	double x_min = MIN(MIN(r, g), b);
	double c = x_max - x_min;
	
	*V = x_max;

	if (c == 0.)
		*H = 0.;
	else if (*V == r)
		*H = ((g - b) / c) / 6.;
	else if (*V == g)
		*H = (2. + (b - r) / c) / 6.;
	else
		*H = (4. + (r - g) / c) / 6.;

	*S = *V == 0. ? 0. : c / *V;
}


void cmy_rgb(double c, double m, double y, double *R, double *G, double *B)
{
	*R = 1. - c;
	*G = 1. - m;
	*B = 1. - y;
}


void rgb_cmy(double r, double g, double b, double *C, double *M, double *Y)
{
	*C = 1. - r;
	*M = 1. - g;
	*Y = 1. - b;
}


void ycocg_rgb(double y, double co, double cg, double *R, double *G, double *B)
{
	co -= .5;
	cg -= .5;

	*R = y - cg + co;
	*G = y + cg;
	*B = y - cg - co;
}


void rgb_ycocg(double r, double g, double b, double *Y, double *Co, double *Cg)
{
	*Y = r / 4. + g / 2. + b / 4.;
	*Co = r / 2. - b / 2. + .5;
	*Cg = -r / 4. + g / 2. - b / 4. + .5;
}


void ycbcr601_rgb(double y, double cb, double cr, double *R, double *G, double *B)
{
	double kr = .299, kg = .587, kb = .114;

	cb -= .5;
	cr -= .5;

	*R = y + (2. - 2. * kr) * cr;
	*G = y - kb * (2. - 2. * kb) * cb / kg - kr * (2 - 2. * kr) * cr / kg;
	*B = y + (2. - 2. * kb) * cb;
}


void ycbcr709_rgb(double y, double cb, double cr, double *R, double *G, double *B)
{
	double kr = .2126, kg = .7152, kb = .0722;

	cb -= .5;
	cr -= .5;

	*R = y + (2. - 2. * kr) * cr;
	*G = y - kb * (2. - 2. * kb) * cb / kg - kr * (2 - 2. * kr) * cr / kg;
	*B = y + (2. - 2. * kb) * cb;
}


void rgb_ycbcr601(double r, double g, double b, double *Y, double *Cb, double *Cr)
{
	double kr = .299, kg = .587, kb = .114;

	*Y = kr * r + kg * g + kb * b;
	*Cb = (b - *Y) / (2. * (1. - kb)) + .5;
	*Cr = (r - *Y) / (2. * (1. - kb)) + .5;
}


void rgb_ycbcr709(double r, double g, double b, double *Y, double *Cb, double *Cr)
{
	double kr = .2126, kg = .7152, kb = .0722;

	*Y = kr * r + kg * g + kb * b;
	*Cb = (b - *Y) / (2. * (1. - kb)) + .5;
	*Cr = (r - *Y) / (2. * (1. - kb)) + .5;
}


void rgb_rgb(double r, double g, double b, double *R, double *G, double *B)
{
	*R = r;
	*G = g;
	*B = b;
}


void convert(struct PNMImage *img, char *from, char *to)
{
	converter to_intr;

	if (!strcmp(from, "RGB"))
		to_intr = &rgb_rgb;
	else if (!strcmp(from, "HSL"))
		to_intr = &hsl_rgb;
	else if (!strcmp(from, "HSV"))
		to_intr = &hsv_rgb;
	else if (!strcmp(from, "YCbCr.601"))
		to_intr = &ycbcr601_rgb;
	else if (!strcmp(from, "YCbCr.709"))
		to_intr = &ycbcr709_rgb;
	else if (!strcmp(from, "YCoCg"))
		to_intr = &ycocg_rgb;
	else if (!strcmp(from, "CMY"))
		to_intr = &cmy_rgb;
	else
		return;

	converter to_trgt;

	if (!strcmp(to, "RGB"))
		to_trgt = &rgb_rgb;
	else if (!strcmp(to, "HSL"))
		to_trgt = &rgb_hsl;
	else if (!strcmp(to, "HSV"))
		to_trgt = &rgb_hsv;
	else if (!strcmp(to, "YCbCr.601"))
		to_trgt = &rgb_ycbcr601;
	else if (!strcmp(to, "YCbCr.709"))
		to_trgt = &rgb_ycbcr709;
	else if (!strcmp(to, "YCoCg"))
		to_trgt = &rgb_ycocg;
	else if (!strcmp(to, "CMY"))
		to_trgt = &rgb_cmy;
	else
		return;

	for (unsigned h = 0; h < img->height; ++h)
		for (unsigned w = 0; w < img->width; ++w) {

			unsigned idx = pnm_px_offset(img, h, w);

			double sch1 = img->data[idx] / (double) img->max_value;
			double sch2 = img->data[idx + 1] / (double) img->max_value;
			double sch3 = img->data[idx + 2] / (double) img->max_value;

			double dch1, dch2, dch3;

			(*to_intr)(sch1, sch2, sch3, &dch1, &dch2, &dch3);
			(*to_trgt)(dch1, dch2, dch3, &sch1, &sch2, &sch3);

			img->data[idx] = MAX(0, MIN(img->max_value, sch1 * img->max_value));
			img->data[idx+1] = MAX(0, MIN(img->max_value, sch2 * img->max_value));
			img->data[idx+2] = MAX(0, MIN(img->max_value, sch3 * img->max_value));
		}
}
