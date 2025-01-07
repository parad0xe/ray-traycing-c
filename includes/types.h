#ifndef H_RAYTRAYCING_TYPES
#define H_RAYTRAYCING_TYPES

typedef struct	Light {
	double intensity;
	_Bool active;
	_Bool visible;
	uint32_t color;
	double theta; // radians
	double fov; // radians
} Light;

typedef struct	PixelState {
	signed char visibility;
	double intensity;
	_Bool is_light_source;
	_Bool fill;
	uint32_t color;
} PixelState;

typedef struct	Environment {
	PixelState *state;
} Environment;

typedef struct	Circle {
	int x, y, r;
	uint32_t color;
	signed char visibility;
	Light light;
} Circle;

typedef struct	MoveEvent {
	_Bool left;
	_Bool right;
	_Bool up;
	_Bool down;
} MoveEvent;

typedef struct	Player {
	Circle circle;
	MoveEvent moves;
} Player;

#endif
