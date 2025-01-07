#include <math.h>
#include <stdint.h>
#include "types.h"
#include "utils.h"
#include "constants.h"
#include "environment.h"

static _Bool	_raytrace_check_next_point_is_reachable(Environment *environment, int x, int y, int next_x, int next_y);
static int	_raytrace_step(Environment *environment, Circle *circle, int x, int y, int next_x, int next_y);

void	empty_environment(Environment *environment)
{
	for(int y = 0; y < W_ROWS; y++)
	{
		for(int x = 0; x < W_COLS; x++)
		{
			environment->state[x + y * W_COLS] = (PixelState) { 
				.visibility = 0,
				.is_light_source = 0,
				.fill = 0, 
				.color = COLOR_BLACK,
				.intensity = 0
			};
		}
	}
}

void	flush_environment(SDL_Surface *surface, Environment *environment)
{
	for(int y = 0; y < W_ROWS; y++)
	{
		for(int x = 0; x < W_COLS; x++)
		{
			PixelState pixel_state = environment->state[x + y * W_COLS];
			SDL_Rect point = { x, y, 1, 1 };

			switch (pixel_state.visibility)
			{
				case 1:
					SDL_FillRect(surface, &point, pixel_state.color);
					break;
				case 2:
					SDL_FillRect(surface, &point, COLOR_GRAY);
					break;
				default:
					SDL_FillRect(surface, &point, COLOR_BLACK);
			}
		}
	}
}


void	add_circle(Environment *environment, Circle *circle)
{
	for (int y = circle->y - circle->r - CIRCLE_BORDER_PADDING; y < circle->y + circle->r + CIRCLE_BORDER_PADDING; y++)
	{
		for (int x = circle->x - circle->r - CIRCLE_BORDER_PADDING; x < circle->x + circle->r + CIRCLE_BORDER_PADDING; x++)
		{
			double distance = hypothenuse(circle->x, circle->y, x, y);

			if (x < 0 || x >= W_COLS || y >= W_ROWS || y < 0)
				continue;

			if (distance <= circle->r)
			{
				PixelState *pixel_state = &environment->state[x + y * W_COLS];

				pixel_state->fill = 1;
				pixel_state->color = circle->color;
				pixel_state->is_light_source = circle->light.active;
				pixel_state->visibility = circle->visibility;
				pixel_state->intensity = 1;
			}
		}
	}
}


void	raytrace(Environment *environment, Circle *circle, int debug)
{
	int start_x = circle->x;
	int start_y = circle->y;

	if (debug == 1)
	{
		Circle center = { 
			.x = start_x, .y = start_y, .r = 2,
			.color = COLOR_GREEN,
			.visibility = 1,
			.light = { 0 }
		};

		add_circle(environment, &center);
	}


	for (double theta = circle->light.theta - circle->light.fov / 2; theta < circle->light.theta + circle->light.fov / 2; theta += 0.005)
	{
		double px = start_x + cos(theta) * circle->r;
		double py = start_y + sin(-theta) * circle->r;

		double cos_direction = (cos(theta) > 0) - (cos(theta) < 0);
		double sin_direction = (sin(-theta) > 0) - (sin(-theta) < 0);

		if (debug == 1)
		{
			Circle pointer = { 
				.x = px, .y = py, .r = 2,
				.color = COLOR_RED,
				.visibility = 1,
				.light = { 0 }
			};

			add_circle(environment, &pointer);
		}

		if (circle->light.active == 0)
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

			int ret = _raytrace_step(environment, circle, x, y, next_x, next_y);
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

			int ret = _raytrace_step(environment, circle, x, y, next_x, next_y);
			if (ret < 0)
				break;
		}
	}
}

static int	_raytrace_step(Environment *environment, Circle *circle, int x, int y, int next_x, int next_y)
{
	int start_x = circle->x;
	int start_y = circle->y;

	double distance = hypothenuse(start_x, start_y, x, y);
	double intensity = 255. - (255. / (circle->light.intensity / pow(distance, 2)));

	if (intensity < 0)
		return -1;

	PixelState *pixel_state = &environment->state[x + y * W_COLS];		

	if (distance >= circle->r + CIRCLE_BORDER_PADDING && pixel_state->is_light_source == 1)
		return -1;

	pixel_state->visibility = 1;
	
	if (pixel_state->fill == 0)
	{	
		if (circle->light.visible && intensity > pixel_state->intensity)
		{
			pixel_state->intensity = intensity;
			pixel_state->color = brightness(circle->light.color, intensity);
		}
	}
	else
		pixel_state->color = brightness(pixel_state->color, intensity);

	if (distance >= circle->r + CIRCLE_BORDER_PADDING && _raytrace_check_next_point_is_reachable(environment, x, y, next_x, next_y) == 0)
		return -1;

	return 0;
}

static _Bool	_raytrace_check_next_point_is_reachable(Environment *environment, int x, int y, int next_x, int next_y)
{
	if (next_y < 0 || next_y >= W_ROWS || next_x >= W_COLS || next_x < 0)
		return 0;

	PixelState *pixel_state = &environment->state[x + y * W_COLS];
	PixelState *next_pixel_state = &environment->state[next_x + next_y * W_COLS];
	if (pixel_state->fill == 1 && next_pixel_state->fill == 0)
		return 0;

	return 1;
}

