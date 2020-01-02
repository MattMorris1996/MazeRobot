#include "stdio.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const char* APP_NAME = "Robot Maze";
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

struct GameWindow {
    SDL_Window *window;
    SDL_Surface *surface;
    SDL_Renderer *renderer;
};

struct GameData {
    int mouse_x;
    int mouse_y;
    int mouse_down;
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

SDL_Texture *brush(struct GameWindow *pmaze_window, struct GameData *pdata, SDL_Surface **pmaze)
{
    SDL_Texture * output;
    SDL_Rect fillRect = {pdata->mouse_x-5, pdata->mouse_y-5, 10, 10};
    if (pdata->mouse_down)
    {
        SDL_FillRect(*pmaze, &fillRect, 0xFF00FF00);
    }
    
    output = SDL_CreateTextureFromSurface(pmaze_window->renderer, *pmaze);
    return output;
}

int main(int argc, char* argv[])        
{
    printf("Hello World\n");

    struct GameWindow maze_window;
    struct GameData data;
    
    //set NULL
    maze_window.window = NULL;
    maze_window.surface = NULL;
    maze_window.renderer = NULL;

    SDL_Surface *maze = NULL;


    SDL_Rect fillRect = {WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
    SDL_Texture *test;
    if(init(&maze_window) == 0)
    {
        printf("init error\n");
    }
    else
    {
        int quit = 0;
        maze = SDL_GetWindowSurface(maze_window.window);
        data.mouse_down = 0;
        SDL_Event e;
        while(!quit)
        {
            while (SDL_PollEvent(&e) != 0)
            {
                if (e.type == SDL_QUIT)
                {
                    quit = 1;
                }
                if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    data.mouse_down = 1;
                }
                if (e.type == SDL_MOUSEBUTTONUP)
                {
                    data.mouse_down = 0;
                }
            }

            SDL_GetMouseState(&data.mouse_x, &data.mouse_y);
            SDL_RenderClear(maze_window.renderer);

            test = brush(&maze_window, &data, &maze);

            SDL_RenderCopy(maze_window.renderer, test, NULL, NULL);

            //Update screen
            SDL_RenderPresent(maze_window.renderer);

            SDL_DestroyTexture(test);
        }
    }
    
    //quit and destroy
    destroy(&maze_window);
    SDL_Quit();
}

