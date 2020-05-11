
#include <SDL.h>
#undef main
#include <iostream>
#include <cstdint>
#include <algorithm>

const double pi = 3.14159265359;

const int mapW = 24, mapH = 24;

const double SCR_WIDTH = 640, SCR_HEIGHT = 480;

bool init();

void close();

SDL_Renderer* grenderer = NULL;

SDL_Surface* gsurface = NULL;

SDL_Window* gwindow = NULL;


int worldMap[mapW][mapH] =
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

bool init()
{
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Couldn't init SDL Subysystems\n" << SDL_GetError();
		success = false;
	}
	else
	{
		gwindow = SDL_CreateWindow("raycaster", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCR_WIDTH, SCR_HEIGHT, 0);
		if (gwindow == NULL)
		{
			std::cout << "\nCouldn't create window\n" << SDL_GetError();
			success = false;
		}
		else
		{
			grenderer = SDL_CreateRenderer(gwindow, -1, SDL_RENDERER_ACCELERATED);
			if (grenderer == NULL)
			{
				std::cout << "\nCan't create renderer\n" << SDL_GetError();
				success = false;
			}
			else
				SDL_SetRenderDrawColor(grenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		}
	}
	return success;
}

void close()
{
	SDL_FreeSurface(gsurface);
	gsurface = NULL;

	SDL_DestroyRenderer(grenderer);
	grenderer = NULL;

	SDL_DestroyWindow(gwindow);
	gwindow = NULL;

	SDL_Quit();
}

int main()
{
	double posX = 22, posY = 12;
	double dirX = -1, dirY = 0;
	double planeX = 0, planeY = 0.66; 

	double time = 0;
	double oldTime = 0; 

	if (!init())
	{
		std::cout << "\nCouldn't init\n" << SDL_GetError();
	}
	else
	{
		bool quit = false;

		SDL_Event e;

		while (!quit)
		{
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_QUIT)
				{
					quit = true;
				}
			}
			int w = SCR_WIDTH;
			for (int x = 0; x < w; x++)
			{
				double cameraX = 2 * x / (double)w - 1;
				double rayDirX = dirX + planeX * cameraX;
				double rayDirY = dirY + planeY * cameraX;
				int mapX = int(posX);
				int mapY = int(posY);


				double sideDistX;
				double sideDistY;

				double deltaDistX = std::abs(1 / rayDirX);
				double deltaDistY = std::abs(1 / rayDirY);
				double perpWallDist;

				int stepX;
				int stepY;

				int hit = 0;
				int side;
				if (rayDirX < 0)
				{
					stepX = -1;
					sideDistX = (posX - mapX) * deltaDistX;
				}
				else
				{
					stepX = 1;
					sideDistX = (mapX + 1.0 - posX) * deltaDistX;
				}
				if (rayDirY < 0)
				{
					stepY = -1;
					sideDistY = (posY - mapY) * deltaDistY;
				}
				else
				{
					stepY = 1;
					sideDistY = (mapY + 1.0 - posY) * deltaDistY;
				}

				while (hit == 0)
				{
					if (sideDistX < sideDistY)
					{
						sideDistX += deltaDistX;
						mapX += stepX;
						side = 0;
					}
					else
					{
						sideDistY += deltaDistY;
						mapY += stepY;
						side = 1;
					}

					if (worldMap[mapX][mapY] > 0) hit = 1;
				}

				if (side == 0) perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
				else          perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

				double h = SCR_HEIGHT;
				int lineHeight = (int)(h / perpWallDist);


				int drawStart = -lineHeight / 2 + h / 2;
				if (drawStart < 0)drawStart = 0;
				int drawEnd = lineHeight / 2 + h / 2;
				if (drawEnd >= h)drawEnd = h - 1;

				if (worldMap[mapX][mapY] == 1)
				{
					SDL_SetRenderDrawColor(grenderer, 0, 0, 255, 255);
				}
				else if (worldMap[mapX][mapY] == 2)
				{
					SDL_SetRenderDrawColor(grenderer, 0, 255, 0, 255);
				}
				else if (worldMap[mapX][mapY] == 3)
				{
					SDL_SetRenderDrawColor(grenderer, 255, 0, 0, 255);
				}
				else if (worldMap[mapX][mapY] == 4)
				{
					SDL_SetRenderDrawColor(grenderer, 255, 255, 0, 255);
				}
				else if (worldMap[mapX][mapY] == 5)
				{
					SDL_SetRenderDrawColor(grenderer, 255, 255, 255, 255);
				}

				SDL_RenderDrawLine(grenderer, x, drawStart, x, drawEnd);
			}
			const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
			oldTime = time;
			time = SDL_GetTicks();
			double frameTime = (time - oldTime) / 1000.0;
			std::cout << 1.0 / frameTime << std::endl;
			double moveSpeed = frameTime * 5.0;
			double rotSpeed = frameTime * 3.0;

			if (currentKeyStates[SDL_SCANCODE_UP])
			{
				if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
				if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
			}
			if (currentKeyStates[SDL_SCANCODE_DOWN])
			{
				if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
				if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
			}
			if (currentKeyStates[SDL_SCANCODE_RIGHT])
			{
				double oldDirX = dirX;
				dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
				dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
				double oldPlaneX = planeX;
				planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
				planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
			}
			if (currentKeyStates[SDL_SCANCODE_LEFT])
			{
				double oldDirX = dirX;
				dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
				dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
				double oldPlaneX = planeX;
				planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
				planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
			}
			SDL_RenderPresent(grenderer);
			SDL_SetRenderDrawColor(grenderer, 0, 0, 0, 255);
			SDL_RenderClear(grenderer);
		}
	}
	close();
	return 0;
}

