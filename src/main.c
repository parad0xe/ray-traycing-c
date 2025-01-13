#include <stdio.h>
#include <SDL.h>
#include <math.h>
#include "utils.h"
#include "types.h"
#include "environment.h"
#include "constants.h"

void	Main_Circle_Move(Circle *circle)
{
	int vx = circle->vx;
	int vy = circle->vy;

	if (
		(vx < 0 && circle->x - circle->r - CIRCLE_BOX_PADDING - vx < 0) ||
		(vx >= 0 && circle->x + circle->r + CIRCLE_BOX_PADDING + vx > W_COLS - 1)
	)
		circle->vx *= -1;

	if (
		(vy < 0 && circle->y - circle->r - CIRCLE_BOX_PADDING - vy < 0) ||
		(vy >= 0 && circle->y + circle->r + CIRCLE_BOX_PADDING + vy > W_ROWS - 1)
	)
		circle->vy *= -1;
	
	circle->x += circle->vx;
	circle->y += circle->vy;
}

void	Main_Player_OnMove(Player *player, SDL_KeyboardEvent event, _Bool active)
{
	if (event.keysym.sym == SDLK_z)
		player->circle.vy = -PLAYER_SPEED * active;
	if (event.keysym.sym == SDLK_s)
		player->circle.vy = PLAYER_SPEED * active;
	if (event.keysym.sym == SDLK_q)
		player->circle.vx = -PLAYER_SPEED *active;
	if (event.keysym.sym == SDLK_d)
		player->circle.vx = PLAYER_SPEED *active;
}

void	Main_Player_ToggleLight(Player *player)
{
	player->circle.light.active = !player->circle.light.active;
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

	Environment	environment = Environment_Create();

	Player	player = { 
		.circle = { 
			.x = 50, .y = 200, .r = 20, 
			.vx = 0, .vy = 0,
			.color = COLOR_BLUE,
			.texture_type = TEXTURE_TYPE_FILLED,
			.light = { 
				.intensity = 200000.,
				.active = 0,
				.color = COLOR_GRAY_LIGHT,
				.theta = -1,
				.fov = 3.1415 / 6
			} 
		}
	};

	Circle 	circles[4] = {
		{ // SUN BALL
			.x = 450, .y = 200, .r = 40, 
			.vx = 4, .vy = 2,
			.color = COLOR_WHITE,
			.texture_type = TEXTURE_TYPE_FILLED, 
			.light = {
				.intensity = 150000.,
				.active = 1, 
				.color = COLOR_GRAY_LIGHT,
				.theta = 0,
				.fov = 3.1415 * 2
			} 
		},
		{ // RED BALL
			.x = 400, .y = 270, .r = 10, 
			.vx = 0, .vy = 0,
			.color = COLOR_RED, 
			.texture_type = TEXTURE_TYPE_DYNAMIC,
			.light = { 0 }
		},
		{ // GREEN BALL
			.x = 100, .y = 250, .r = 15, 
			.vx = 0, .vy = 0,
			.color = COLOR_GREEN,
			.texture_type = TEXTURE_TYPE_DYNAMIC,
			.light = { 0 }
		},
		{ // PURPLE BALL
			.x = 200, .y = 150, .r = 20, 
			.vx = 0, .vy = 0,
			.color = COLOR_PURPLE,
			.texture_type = TEXTURE_TYPE_DYNAMIC,
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
				if (event.button.button == SDL_BUTTON_LEFT)
					Main_Player_ToggleLight(&player);
			
			if (event.type == SDL_KEYDOWN)
				Main_Player_OnMove(&player, event.key, 1);
			if (event.type == SDL_KEYUP)
				Main_Player_OnMove(&player, event.key, 0);
						
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

		Environment_Empty(&environment);
		
		for(int i = 0; i < 4; i++)
			Environment_AddCircle(&environment, &circles[i]);
		Environment_AddCircle(&environment, &player.circle);

		for(int i = 0; i < 4; i++)
			Environment_Ray2D(&environment, &circles[i], 0);
		Environment_Ray2D(&environment, &player.circle, 1);

		for(int i = 0; i < 4; i++)
			Main_Circle_Move(&circles[i]);
		Main_Circle_Move(&player.circle);

		Environment_Flush(surface, &environment);
		
		SDL_UpdateWindowSurface(window);
		SDL_Delay(1);
	}

	Environment_Free(&environment);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
