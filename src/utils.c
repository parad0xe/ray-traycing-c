#include <math.h>
#include <SDL.h>
#include "utils.h"

double	Utils_Hypothenuse(int xa, int ya, int xb, int yb)
{
	return sqrt(pow(xb - xa, 2) + pow(yb - ya, 2));
}

SDL_Color	Utils_GetSDLColorFromHex(uint32_t color)
{
	SDL_Color sdl_color = {
		.r = (color & 0xFF0000) >> 16,
		.g = (color & 0x00FF00) >> 8,
		.b = (color & 0x0000FF),
		.a = 255
	};

	return sdl_color;
}

uint32_t	Utils_Brightness(uint32_t color, double intensity)
{
	intensity = fabs(intensity / 255.);

	SDL_Color sdl_color = Utils_GetSDLColorFromHex(color);
	
	return (
		((uint32_t)(sdl_color.r * intensity) << 16) + 
		((uint32_t)(sdl_color.g * intensity) << 8) + 
		((uint32_t)(sdl_color.b * intensity))
	);
}

_Bool   Utils_ColorIsLightest(uint32_t test_color, uint32_t witness_color)
{
	SDL_Color sdl_test_color = Utils_GetSDLColorFromHex(test_color);
	SDL_Color sdl_witness_color = Utils_GetSDLColorFromHex(witness_color);

	return sdl_test_color.r > sdl_witness_color.r || 
		sdl_test_color.g > sdl_witness_color.g || 
		sdl_test_color.b > sdl_witness_color.b;
}
