#ifndef H_RAYTRAYCING_UTILS
#define H_RAYTRAYCING_UTILS

#include <stdint.h>
#include <SDL.h>

double	Utils_Hypothenuse(int xa, int ya, int xb, int yb);

uint32_t    Utils_Brightness(uint32_t color, double brightness);
SDL_Color	Utils_GetSDLColorFromHex(uint32_t color);
_Bool	Utils_ColorIsLightest(uint32_t test_color, uint32_t witness_color);
#endif
