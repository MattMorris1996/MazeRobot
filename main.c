#include "stdio.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const char* APP_NAME = "Robot Maze";
const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 300;

struct GameWindow {
    SDL_Window *window;
    SDL_Surface *surface;
    SDL_Renderer *renderer;
};

int init(struct GameWindow *pmaze_window)
{
    //error variable
    int success = 1;

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = 0;
    }
    else
    {
        //create window
        pmaze_window->window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if(pmaze_window->window == NULL)
        {
            printf("SDL could not create window %s", SDL_GetError());
            success = 0;
        }
        else 
        {
            //create renderer
            pmaze_window->renderer = SDL_CreateRenderer(pmaze_window->window, -1, SDL_RENDERER_ACCELERATED);
            if (pmaze_window->renderer == NULL)
            {
                printf("Renderer could not be created %s\n", SDL_GetError());
                success = 0;
            }
            else 
            {
                SDL_SetRenderDrawColor(pmaze_window->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            }
        }
    }
    return success;
}

void destroy(struct GameWindow *pmaze_window)
{
    //Destroy Renderer
    SDL_DestroyRenderer(pmaze_window->renderer);
    pmaze_window->renderer = NULL;

    //Destroy Window
    SDL_DestroyWindow(pmaze_window->window);
    pmaze_window->window = NULL;
}

int main(int argc, char* argv[])
{
    printf("Hello World\n");

    struct GameWindow maze_window;
    
    //set NULL
    maze_window.window = NULL;
    maze_window.surface = NULL;
    maze_window.renderer = NULL;

    SDL_Rect fillRect = {WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};

    if(init(&maze_window) == 0)
    {
        printf("init error\n");
    }
    else
    {
        int quit = 0;
        SDL_Event e;
        while(!quit)
        {
            while (SDL_PollEvent(&e) != 0)
            {
                if (e.type == SDL_QUIT)
                {
                    quit = 1;
                }
            }
            SDL_SetRenderDrawColor(maze_window.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(maze_window.renderer);

            //Render red filled square
            SDL_SetRenderDrawColor(maze_window.renderer, 0xFF, 0x00, 0x00, 0xFF);
            SDL_RenderFillRect(maze_window.renderer, &fillRect);

            //Update screen
            SDL_RenderPresent(maze_window.renderer);
        }
    }
    //quit and destroy
    destroy(&maze_window);
    SDL_Quit();
}

