#include <stdio.h>
#include <SDL.h>
#include <math.h>
#include "types.h"
#include "environment.h"
#include "constants.h"

void	clear_surface(SDL_Surface *surface)
{
	SDL_Rect	rect = { 0, 0, surface->w, surface->h };
	SDL_FillRect(surface, &rect, COLOR_BLACK);
}

int	main()
{
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Failed to initialize SDL.\n");
		exit(1);
	}
	else
		printf("SDL initialized\n");

	SDL_Window	*window = SDL_CreateWindow("Ray-Traycing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W_COLS, W_ROWS, 0);
	
	if (window == NULL)
	{
		printf("Failed to initialize Window.\n");
		exit(1);
	}
	else
		printf("Window initialized\n");

	SDL_SetWindowResizable(window, SDL_FALSE);

	SDL_Surface	*surface = SDL_GetWindowSurface(window);
	
	if (surface == NULL)
	{
		printf("Failed to initialize Surface.\n");
		exit(1);
	}
	else
		printf("Surface initialized\n");

	Environment	environment;
	environment.state = malloc(FLAT_ENVIRONMENT_LENGTH * sizeof(PixelState));
	
	if (environment.state == NULL)
	{
		printf("Failed to initialize Environment.\n");
		exit(1);
	}
	else
		printf("Environment initialized\n");

	Player	player = { 
		.circle = { 
			.x = 50, .y = 200, .r = 20, 
			.color = COLOR_BLUE, 
			.visibility = 1, 
			.light = { 
				.intensity = 200000.,
				.active = 0,
				.visible = 1, 
				.color = COLOR_GRAY_LIGHT,
				.theta = -1,
				.fov = 3.1415 / 6
			} 
		},
		.moves = { 0 } 
	};

	Circle 	circles[4] = {
		{ // SUN BALL
			.x = 450, .y = 200, .r = 40, 
			.color = COLOR_YELLOW, 
			.visibility = 2, 
			.light = { 
				.intensity = 100000.,
				.active = 1, 
				.visible = 1, 
				.color = COLOR_GRAY_LIGHT,
				.theta = 0,
				.fov = 3.1415 * 2
			} 
		},
		{ // RED BALL
			.x = 400, .y = 270, .r = 10, 
			.color = COLOR_RED, 
			.visibility = 2, 
			.light = { 0 }
		},
		{ // GREEN BALL
			.x = 100, .y = 250, .r = 15, 
			.color = COLOR_GREEN, 
			.visibility = 2,
			.light = { 0 }
		},
		{ // PURPLE BALL
			.x = 200, .y = 150, .r = 20, 
			.color = COLOR_PURPLE,
			.visibility = 2,
			.light = { 0 } 
		}
	};

	int	running = 1;
	while (running)
	{
		SDL_Event	event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = 0;
				break;
			}

			if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				if (event.button.button == SDL_BUTTON_LEFT)
					player.circle.light.active = !player.circle.light.active;
			}
			
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_z)
					player.moves.up = 1;
				if (event.key.keysym.sym == SDLK_s)
					player.moves.down = 1;
				if (event.key.keysym.sym == SDLK_q)
					player.moves.left = 1;
				if (event.key.keysym.sym == SDLK_d)
					player.moves.right = 1;
			}

			if (event.type == SDL_KEYUP)
			{
				if (event.key.keysym.sym == SDLK_z)
					player.moves.up = 0;
				if (event.key.keysym.sym == SDLK_s)
					player.moves.down = 0;
				if (event.key.keysym.sym == SDLK_q)
					player.moves.left = 0;
				if (event.key.keysym.sym == SDLK_d)
					player.moves.right = 0;
			}
			
			if (event.type == SDL_MOUSEMOTION)
			{
				int mouse_x = event.motion.x;
				int mouse_y = event.motion.y;

				double dx = mouse_x - player.circle.x;
				double dy = mouse_y - player.circle.y;

				double theta = atan2(-dy, dx); 

				player.circle.light.theta = theta;
			}
		}

		if (player.moves.left == 1 && player.circle.x - PLAYER_SPEED > 0)
			player.circle.x -= PLAYER_SPEED;
		if (player.moves.up == 1 && player.circle.y - PLAYER_SPEED > 0)
			player.circle.y -= PLAYER_SPEED;
		if (player.moves.right == 1 && player.circle.x + PLAYER_SPEED < W_COLS - 1)
			player.circle.x += PLAYER_SPEED;
		if (player.moves.down == 1 && player.circle.y + PLAYER_SPEED < W_ROWS - 1)
			player.circle.y += PLAYER_SPEED;
		
		clear_surface(surface);
		empty_environment(&environment);
		
		for(int i = 0; i < 4; i++)
			add_circle(&environment, &circles[i]);
		add_circle(&environment, &player.circle);

		for(int i = 0; i < 4; i++)
			raytrace(&environment, &circles[i], 0);
		raytrace(&environment, &player.circle, 1);

		flush_environment(surface, &environment);

		SDL_UpdateWindowSurface(window);
		SDL_Delay(1);
	}

	free(environment.state);
	SDL_Quit();
	return 0;
}
