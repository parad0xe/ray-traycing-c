#ifndef H_RAYTRAYCING_ENVIRONMENT
#define H_RAYTRAYCING_ENVIRONMENT

#include "SDL.h"
#include "types.h"

void    flush_environment(SDL_Surface *surface, Environment *environment);
void    empty_environment(Environment *environment);
void    add_circle(Environment *environment, Circle *circle);
void    raytrace(Environment *environment, Circle *circle, int debug);

#endif
