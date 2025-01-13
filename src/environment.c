#include <math.h>
#include <SDL.h>
#include "types.h"
#include "constants.h"
#include "environment.h"
#include "utils.h"

static char	_Environment_Ray2D_Step(Environment *environment, Circle *circle, int x, int y, int next_x, int next_y);
static _Bool	_Environment_Ray2D_NextPixelIsReachable(Environment *environment, int x, int y, int next_x, int next_y);

Environment	Environment_Create()
{
	Environment environment;
	environment.state = malloc(FLAT_ENVIRONMENT_LENGTH * sizeof(Pixel));
	
	if (environment.state == NULL)
	{
		printf("Failed to initialize Environment.\n");
		exit(1);
	}

	return environment;
}

void    Environment_Empty(Environment *environment)
{
	for(int y = 0; y < W_ROWS; y++)
	{
		for(int x = 0; x < W_COLS; x++)
		{
			Pixel pixel = (Pixel) {
				.texture_type = TEXTURE_TYPE_DYNAMIC,
				.intensity = 0,
				.is_fill = 0,
				.color = COLOR_BLACK
			};

			Environment_SetPixel(environment, x, y, pixel);
		}
	}
}

void	Environment_Flush(SDL_Surface *surface, Environment *environment)
{
	static const int	POINTS_BUFFER_COUNT = 1000;

	SDL_Rect	points[POINTS_BUFFER_COUNT];
	unsigned int	points_index = 0;
	uint32_t	last_pixel_color = 0;

	for(int y = 0; y < W_ROWS; y++)
	{
		for(int x = 0; x < W_COLS; x++)
		{
			uint32_t current_pixel_color;

			Pixel *pixel = Environment_GetPixel(environment, x, y);
			
			if (pixel == NULL)
				continue;

			SDL_Rect point = { x, y, 1, 1 };

			if (pixel->texture_type == TEXTURE_TYPE_FILLED)
				current_pixel_color = pixel->color;
			else if (pixel->is_fill && pixel->intensity == 0)
				current_pixel_color = COLOR_GRAY;
			else
				current_pixel_color = Utils_Brightness(pixel->color, pixel->intensity);

			if (current_pixel_color != last_pixel_color || points_index == POINTS_BUFFER_COUNT - 1)
			{
				SDL_FillRects(surface, points, points_index, last_pixel_color);
				points_index = 0;
			}

			points[points_index] = point;
			points_index++;
			last_pixel_color = current_pixel_color;
		}
	}

	if (points_index > 0)
		SDL_FillRects(surface, points, points_index, last_pixel_color);
}

void	Environment_AddCircle(Environment *environment, Circle *circle)
{
	for (int y = circle->y - circle->r - CIRCLE_BOX_PADDING; y < circle->y + circle->r + CIRCLE_BOX_PADDING; y++)
	{
		for (int x = circle->x - circle->r - CIRCLE_BOX_PADDING; x < circle->x + circle->r + CIRCLE_BOX_PADDING; x++)
		{
			double distance = Utils_Hypothenuse(circle->x, circle->y, x, y);

			if (distance <= circle->r)
			{
				Pixel *pixel = Environment_GetPixel(environment, x, y);
				
				if (pixel == NULL)
					continue;

				pixel->is_fill = 1;
				pixel->texture_type = circle->texture_type;
				pixel->color = circle->color;
				pixel->intensity = circle->light.intensity;
			}
		}
	}
}

void	Environment_Ray2D(Environment *environment, Circle *circle, int visual_debug)
{
	int start_x = circle->x;
	int start_y = circle->y;

	for (double theta = circle->light.theta - circle->light.fov / 2; theta < circle->light.theta + circle->light.fov / 2; theta += 0.001)
	{
		double px = start_x + cos(theta) * circle->r;
		double py = start_y + sin(-theta) * circle->r;

		double cos_direction = (cos(theta) > 0) - (cos(theta) < 0);
		double sin_direction = (sin(-theta) > 0) - (sin(-theta) < 0);

		if (visual_debug == 1)
		{
			Circle pointer = { 
				.x = px, .y = py, .r = 2,
				.vx = 0, .vy = 0,
				.color = COLOR_RED,
				.texture_type = TEXTURE_TYPE_FILLED,
				.light = { 0 }
			};

			Environment_AddCircle(environment, &pointer);
		}
		
		if (!circle->light.active)
			continue;

		double alpha = (py - start_y) / (px - start_x);
		double beta = -(alpha * start_x) + start_y;

		for (int x = start_x; x < W_COLS && x >= 0; x += cos_direction)
		{	
			int y = (int) (alpha * x + beta);
			int next_x = x + cos_direction;
			int next_y = (int) (alpha * (x + cos_direction) + beta);

			if (y >= W_ROWS || y < 0 || abs(next_y - y) > 2)
				break;

			int ret = _Environment_Ray2D_Step(environment, circle, x, y, next_x, next_y);
			if (ret < 0)
				break;
		}

		for (int y = start_y; y < W_ROWS && y >= 0; y += sin_direction)
		{
			int x = (int) ((y - beta) / alpha);
			int next_x = (int) (((y + sin_direction) - beta) / alpha);
			int next_y = y + sin_direction;

			if (x >= W_COLS || x < 0 || abs(next_x - x) > 2)
				break;

			int ret = _Environment_Ray2D_Step(environment, circle, x, y, next_x, next_y);
			if (ret < 0)
				break;
		}
	}

}

Pixel*	Environment_GetPixel(Environment *environment, int x, int y)
{
	if (x < 0 || x >= W_COLS || y >= W_ROWS || y < 0)
		return NULL;
	
	return &environment->state[x + y * W_COLS];
}

void	Environment_SetPixel(Environment *environment, int x, int y, Pixel pixel)
{
	if (x < 0 || x >= W_COLS || y >= W_ROWS || y < 0)
		return;

	environment->state[x + y * W_COLS] = pixel;
}

void	Environment_Free(Environment *environment)
{
	if (environment->state == NULL)
		return;

	free(environment->state);
	environment->state = NULL;
}

static char	_Environment_Ray2D_Step(Environment *environment, Circle *circle, int x, int y, int next_x, int next_y)
{
	int start_x = circle->x;
	int start_y = circle->y;

	double distance = Utils_Hypothenuse(start_x, start_y, x, y);
	double intensity = 255. - (255. / (circle->light.intensity / pow(distance, 2)));

	if (intensity < 0)
		return -1;

	Pixel *pixel = Environment_GetPixel(environment, x, y);		

	if (pixel == NULL)
		return -1;

	if (distance >= circle->r + CIRCLE_BOX_PADDING)
	{
		if (
			(pixel->texture_type == TEXTURE_TYPE_FILLED) ||
			(pixel->is_fill && !Utils_ColorIsLightest(Utils_Brightness(pixel->color, intensity), COLOR_GRAY))
		)
			return -1;
	}

	if (pixel->texture_type == TEXTURE_TYPE_DYNAMIC && 
		intensity > pixel->intensity)
	{
		pixel->intensity = intensity;

		if (!pixel->is_fill)
			pixel->color = circle->light.color;
	}
	_Bool	next_pixel_is_reachable = _Environment_Ray2D_NextPixelIsReachable(environment, x, y, next_x, next_y);

	if (distance >= circle->r + CIRCLE_BOX_PADDING && !next_pixel_is_reachable)
		return -1;

	return 0;
}

static _Bool	_Environment_Ray2D_NextPixelIsReachable(Environment *environment, int x, int y, int next_x, int next_y)
{
	Pixel *pixel = Environment_GetPixel(environment, x, y);
	Pixel *next_pixel = Environment_GetPixel(environment, next_x, next_y);

	if (pixel == NULL || next_pixel == NULL)
		return 0;

	if (pixel->is_fill && next_pixel->is_fill == 0)
		return 0;

	return 1;
}
