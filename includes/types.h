#ifndef H_RAYTRAYCING_TYPES
#define H_RAYTRAYCING_TYPES

typedef enum	TEXTURE_TYPE
{
	TEXTURE_TYPE_DYNAMIC,
	TEXTURE_TYPE_FILLED
} TEXTURE_TYPE;

typedef struct	Pixel
{
	TEXTURE_TYPE texture_type;
	double intensity;
	uint32_t color;
	_Bool is_fill;
} Pixel;

typedef struct	Environment {
	Pixel *state;
} Environment;

typedef struct	Light
{
	double intensity;
	_Bool active;
	uint32_t color;
	double theta; // radians
	double fov; // radians
} Light;

typedef struct	Circle
{
	int x, y, r;
	int vx, vy;
	uint32_t color;
	TEXTURE_TYPE texture_type;
	Light light;
} Circle;

typedef struct	Player {
	Circle circle;
} Player;

#endif
