#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "pnm.h"

void inverse(struct PNMImage *);
void mirror_horizontal(struct PNMImage *);
void mirror_vertical(struct PNMImage *);

enum PNMStatus rotate_right(const struct PNMImage *old, struct PNMImage *new);
enum PNMStatus rotate_left(const struct PNMImage *old, struct PNMImage *new);

#endif // TRANSFORM_H
