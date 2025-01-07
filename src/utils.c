#include <math.h>
#include "utils.h"

double	hypothenuse(int xa, int ya, int xb, int yb)
{
	return sqrt(pow(xb - xa, 2) + pow(yb - ya, 2));
}

uint32_t	luminosity(uint32_t color, double brightness)
{
	brightness = fabs(brightness / 255.);

	int r = (color & 0xFF0000) >> 16;
	int g = (color & 0x00FF00) >> 8;
	int b = (color & 0x0000FF);
	
	return ((uint32_t)(r * brightness) << 16) + ((uint32_t)(g * brightness) << 8) + ((uint32_t)(b * brightness));
}
