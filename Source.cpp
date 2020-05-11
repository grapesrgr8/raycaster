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
	double posX = 22, posY = 12;  //x and y start position
	double dirX = -1, dirY = 0; //initial direction vector
	double planeX = 0, planeY = 0.66; //the 2d raycaster version of camera plane

	double time = 0; //time of current frame
	double oldTime = 0; //time of previous frame

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
				//calculate ray position and direction
				double cameraX = 2 * x / (double)w - 1; //x-coordinate in camera space
				double rayDirX = dirX + planeX * cameraX;
				double rayDirY = dirY + planeY * cameraX;
				//which box of the map we're in
				int mapX = int(posX);
				int mapY = int(posY);

				//length of ray from current position to next x or y-side
				double sideDistX;
				double sideDistY;

				//length of ray from one x or y-side to next x or y-side
				double deltaDistX = std::abs(1 / rayDirX);
				double deltaDistY = std::abs(1 / rayDirY);
				double perpWallDist;

				//what direction to step in x or y-direction (either +1 or -1)
				int stepX;
				int stepY;

				int hit = 0; //was there a wall hit?
				int side; //was a NS or a EW wall hit?
				//calculate step and initial sideDist
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
				//perform DDA
				while (hit == 0)
				{
					//jump to next map square, OR in x-direction, OR in y-direction
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
					//Check if ray has hit a wall
					if (worldMap[mapX][mapY] > 0) hit = 1;
				}
				//Calculate distance projected on camera direction (Euclidean distance will give fisheye effect!)
				if (side == 0) perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
				else          perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

				//Calculate height of line to draw on screen
				double h = SCR_HEIGHT;
				int lineHeight = (int)(h / perpWallDist);

				//calculate lowest and highest pixel to fill in current stripe
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

				SDL_RenderDrawLine(grenderer, x, drawStart, x, drawEnd);
			}
			const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
			oldTime = time;
			time = SDL_GetTicks();
			double frameTime = (time - oldTime) / 1000.0; //frameTime is the time this frame has taken, in seconds
			std::cout << 1.0 / frameTime << std::endl; //FPS counter

			//speed modifiers
			double moveSpeed = frameTime * 5.0; //the constant value is in squares/second
			double rotSpeed = frameTime * 3.0; //the constant value is in radians/second

			//move forward if no wall in front of you
			if (currentKeyStates[SDL_SCANCODE_UP])
			{
				if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
				if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
			}
			//move backwards if no wall behind you
			if (currentKeyStates[SDL_SCANCODE_DOWN])
			{
				if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
				if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
			}
			//rotate to the right
			if (currentKeyStates[SDL_SCANCODE_RIGHT])
			{
				//both camera direction and camera plane must be rotated
				double oldDirX = dirX;
				dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
				dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
				double oldPlaneX = planeX;
				planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
				planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
			}
			//rotate to the left
			if (currentKeyStates[SDL_SCANCODE_LEFT])
			{
				//both camera direction and camera plane must be rotated
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
			//for (int x = 0; x < SCR_WIDTH; x++)
			//{
			//	double blockH = 64, projDist = 277;

			//	double pX = 277, pY = 100;

			//	double cameraX = 2 * double(x) / SCR_WIDTH - 1;

			//	double rayX = pX + cameraX * 160;
			//	double rayY = pY + projDist;

			//	double ya, xa = 36;
			//	double Ay, Ax, By, Bx;

			//	int mapX, mapY;
			//	//horizontal intersection
			//	if (rayX > 0)
			//	{
			//		Ay = std::floor(pY / blockH) * blockH - 1;
			//		ya = -blockH;
			//	}
			//	else
			//	{
			//		Ay = std::floor(pY / blockH) * blockH + blockH;
			//		ya = blockH;
			//	}

			//	Ax = pX + (pY - Ay) / tan(pi / 3);

			//	bool hit = false;

			//	while (!hit)
			//	{
			//		mapY = Ay / blockH - 1;
			//		mapX = Ax / blockH;
			//		if (worldMap[mapX][mapY] >= 1)
			//			hit = true;
			//		Ax = Ax + xa;
			//		Ay = Ay + ya;
			//	}
			//	//end
			//	//vertical intersection
			//	double x_a, y_a;

			//	if (rayY > 0)
			//	{
			//		Bx = std::floor(pX / blockH) * blockH + blockH;
			//		x_a = blockH;
			//	}
			//	else
			//	{
			//		Bx = std::floor(pX / blockH) * blockH - 1;
			//		x_a = -blockH;
			//	}

			//	y_a = blockH * tan(pi / 3);
			//	By = pY + (pX - Bx) / tan(pi / 3);

			//	hit = false;

			//	while (!hit)
			//	{
			//		mapY = By / blockH;
			//		mapX = Bx / blockH;
			//		if (worldMap[mapX][mapY] >= 1)
			//			hit = true;
			//		Bx = Bx + x_a;
			//		By = By + y_a;
			//	}

			//	//distance check
			//	double PD, PE, distToWall;
			//	PD = std::pow((std::pow(pX - Ax, 2) + std::pow(pY - Ay, 2)), 1 / 2);
			//	PE = std::pow((std::pow(pX - Bx, 2) + std::pow(pY - By, 2)), 1 / 2);
			//	if (PD < PE)
			//		distToWall = PD;
			//	else
			//		distToWall = PE;

			//	//projection height
			//	double wallHeight;
			//	wallHeight = double(blockH) / distToWall * double(projDist);

			//	int drawStart = -wallHeight / 2 + SCR_HEIGHT / 2;
			//	if (drawStart < 0)drawStart = 0;
			//	int drawEnd = wallHeight / 2 + SCR_HEIGHT / 2;
			//	if (drawEnd >= SCR_HEIGHT)drawEnd = SCR_HEIGHT - 1;

			//	SDL_SetRenderDrawColor(grenderer, 0, 0, 255, NULL);
			//	SDL_RenderDrawLine(grenderer, x, drawStart, x, drawEnd);