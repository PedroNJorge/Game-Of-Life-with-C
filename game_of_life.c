/*
C Projects #2
-------------
Title: Game of Life
Date: 04/01/2025
Author: Pedro Jorge
*/

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <stdio.h>
#include <SDL3/SDL.h>


#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 900
#define CELL_SIZE 20
#define ROWS SCREEN_HEIGHT/CELL_SIZE
#define COLUMNS SCREEN_WIDTH/CELL_SIZE
#define GRID_LINE_WIDTH 2


typedef struct Cell
{
    int alive;
} Cell;


int compareArrays(int a[ROWS + 2][COLUMNS + 2], int b[ROWS + 2][COLUMNS + 2])
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            if (a[i+1][j+1] != b[i+1][j+1])
            {
                return 0;
            }
        }
    }
    return 1;
}


void init_grid(int grid[ROWS + 2][COLUMNS + 2], int grid_copy[ROWS + 2][COLUMNS + 2])
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            // grid is padded
            grid[i+1][j+1] = grid_copy[i+1][j+1] = 0;
        }
    }
}


void draw_grid(SDL_Renderer* renderer, int grid[ROWS + 2][COLUMNS + 2])
{
    SDL_SetRenderDrawColor(renderer, 0x0f, 0x0f, 0x0f, 0xff);
    SDL_FRect line;

    for (int i = 0; i < ROWS; i++)
    {
        line = (SDL_FRect){0,i*CELL_SIZE,SCREEN_WIDTH,GRID_LINE_WIDTH};
        SDL_RenderFillRect(renderer, &line);
    }

    for (int j = 0; j < COLUMNS; j++)
    {
        line = (SDL_FRect){j*CELL_SIZE,0,GRID_LINE_WIDTH,SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &line);
    }

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
}


int* copy_grid(int grid[ROWS + 2][COLUMNS + 2], int grid_to_copy[ROWS + 2][COLUMNS + 2])
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLUMNS; j++)
        {
            grid[i+1][j+1] = grid_to_copy[i+1][j+1];
        }
    }
    return grid;
}

int* cur_change(int cur[2], int counter, int rel_pos)
{
    cur[counter] = rel_pos;
    return cur;
}
 
int alive_cells(int numAlive, int cur[2], int counter, int i, int j, int grid[ROWS + 2][COLUMNS + 2])
{
    if (counter == 2)
    {
        // use memcmp to check if arrays are equal
        if (cur[0] != 0 && cur[1] != 0)
        {
            numAlive += grid[i + cur[0]][j + cur[1]];
        }
    }
    else
    {
        int pos[3] = {-1, 0, 1};
        
        for (int k = 0; k < 3; k++)
        {
            numAlive = alive_cells(numAlive,cur_change(cur,counter,pos[k]),counter+1,i,j,grid);
        }
    }
    return numAlive;
}


void draw(SDL_Renderer* renderer, int grid[ROWS + 2][COLUMNS + 2], int delay)
{
        //SDL_RenderClear(renderer); 
        draw_grid(renderer, grid);
		
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderPresent(renderer);

        SDL_Delay(delay);
}


void draw_cell(SDL_Renderer* renderer, int x, int y, int alive)
{
    if (alive)
    {
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    }

    SDL_FRect cell = (SDL_FRect){x*CELL_SIZE,y*CELL_SIZE,CELL_SIZE,CELL_SIZE};
    SDL_RenderFillRect(renderer, &cell);
}


int main()
{
	SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Game of Life", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);	
    SDL_Event event;
	const bool* state = SDL_GetKeyboardState(NULL);
    
    // grid is padded vertically and horizontally
    int grid[ROWS + 2][COLUMNS + 2];
    int grid_copy[ROWS + 2][COLUMNS + 2];
    init_grid(grid, grid_copy);

    int mouse_x;
    int mouse_y;

	bool run = true;
    bool simulation = false;
    
    while (run)
    {
        // Get Events
		SDL_PumpEvents();
		SDL_PollEvent(&event);
		
        if (event.type == SDL_EVENT_QUIT)
		{
		    run = false;
			break;
		}

        if (event.type == SDL_EVENT_MOUSE_MOTION)
        {
            /* event.motion.state:
             * 0 -> nothing is being pressed
             * 1 -> Left_Mouse_Click
             * 4 -> Right_Mouse_Click
            */

            switch (event.motion.state)
            {
                case 1:
                    mouse_x = event.motion.x/CELL_SIZE;
                    mouse_y = event.motion.y/CELL_SIZE;
                    grid[mouse_x+1][mouse_y+1] = grid_copy[mouse_x+1][mouse_y+1] = 1;

                    draw_cell(renderer, x, y, 1);
            }
        }

        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            if (event.key.key == SDLK_SPACE)
            {
                simulation = true;
                SDL_Delay(500);
            }
        }

        while (simulation)
        {
		    SDL_PollEvent(&event);
		
            if (event.type == SDL_EVENT_QUIT)
		    {
		        run = simulation = false;
			    break;
		    }
        
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_SPACE)
                {
                    simulation = false;
                    break;
                }
            }

            for (int i = 0; i < ROWS; i++)
            {
                for (int j = 0; j < COLUMNS; j++)
                {
                    int cur[2] = {0,0};
                    int numAlive = alive_cells(0, cur, 0,  i, j, grid);

                    if (grid_copy[i][j])
                    {
                        if (numAlive <= 1 || numAlive >= 4)
                        {
                            grid_copy[i][j] = 0;
                        }
                    }
                    else
                    {
                        if (numAlive == 3)
                        {
                            grid_copy[i][j] = 1;
                        }
                    }
                }
            }
            if (compareArrays(grid, copy_grid(grid, grid_copy)))
            {
                simulation = false;
            }

            draw(renderer, grid, 300);
        }

        draw(renderer, grid, 10);
    }

	SDL_Quit();
	return 0;
}
