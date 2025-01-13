#ifndef H_RAYTRAYCING_ENVIRONMENT
#define H_RAYTRAYCING_ENVIRONMENT

#include <SDL.h>
#include "types.h"

Environment	Environment_Create();
void	Environment_Empty(Environment *environment);
void	Environment_Flush(SDL_Surface *surface, Environment *environment);

void	Environment_AddCircle(Environment *environment, Circle *circle);
void	Environment_Ray2D(Environment *environment, Circle *circle, int visual_debug);

Pixel*	Environment_GetPixel(Environment *environment, int x, int y);
void	Environment_SetPixel(Environment *environment, int x, int y, Pixel pixel);

void	Environment_Free(Environment *environment);
#endif
