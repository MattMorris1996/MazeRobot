#include "stdio.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

const char* APP_NAME = "Robot Maze";
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const double PI = 3.14159265359;

enum KeyStates {
    UP,
    DOWN,
    RIGHT,
    LEFT,
    NONE
};

struct GameWindow {
    SDL_Window *window;
    SDL_Surface *surface;
    SDL_Renderer *renderer;
};

struct GameData {
    int mouse_x;
    int mouse_y;
    int mouse_down;
    int key_state;
};

struct Robot {
    int position_x;
    int position_y;
    double orientation;
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
    SDL_SetColorKey( *pmaze, SDL_TRUE, SDL_MapRGB( (*pmaze)->format, 0x00, 0x00, 0x00 ) );
    output = SDL_CreateTextureFromSurface(pmaze_window->renderer, *pmaze);
    return output;
}

SDL_Texture* robot_init(struct GameWindow *pmaze_window, struct Robot *robot)
{
    SDL_Texture* out;
    out = SDL_CreateTexture(pmaze_window->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 40, 30);
    if (out == NULL)
    {
        printf("Unable to create blank texture! SDL Error: %s\n", SDL_GetError());
    }

    robot->position_x = WINDOW_WIDTH/2;
    robot->position_y = WINDOW_HEIGHT/2;
    robot->orientation = 45;

    return out; 
}

void draw_robot(struct GameWindow *pmaze_window, struct Robot *robot, SDL_Texture** robot_texture, struct GameData *pdata)
{
    SDL_SetRenderTarget(pmaze_window->renderer, *robot_texture);
    SDL_SetRenderDrawColor(pmaze_window->renderer, 0x00, 0xFF, 0xFF, 0xFF);

    const int robot_w = 20;
    const int robot_h = 40;

    switch (pdata->key_state)
    {
        case UP:
            robot->position_y -= 1;
            break;
        case DOWN:
            robot->position_y += 1;
            break;
        case RIGHT:
            robot->position_x += 1;
            break;
        case LEFT:
            robot->position_x -= 1;
            break;
        case NONE:
            break;
    }

    SDL_Rect renderQuad = { robot->position_x, robot->position_y, robot_w, robot_h};
    SDL_RenderClear(pmaze_window->renderer);
    SDL_SetRenderDrawColor(pmaze_window->renderer, 0x00, 0x00, 0xFF, 0xFF);

    SDL_SetRenderTarget(pmaze_window->renderer,NULL);
    SDL_RenderCopyEx(pmaze_window->renderer,*robot_texture,NULL, &renderQuad, 5, NULL, SDL_FLIP_NONE);

    //SDL_RenderCopyEx(pmaze_window->renderer,*robot_texture, NULL, &renderQuad, robot->orientation, NULL, SDL_FLIP_NONE);

   //SDL_SetRenderTarget(pmaze_window->renderer,NULL);

}



int main(int argc, char* argv[])        
{
    printf("Hello World\n");

    struct GameWindow maze_window;
    struct GameData data;
    
    //init to NULL
    maze_window.window = NULL;
    maze_window.surface = NULL;
    maze_window.renderer = NULL;

    SDL_Surface *maze = NULL;

    SDL_Rect fillRect = {WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2};
    struct Robot myRobot;
    SDL_Texture *paint, *robot;


    if(init(&maze_window) == 0)
    {
        printf("init error\n");
    }
    else
    {
        int quit = 0;

        maze = SDL_GetWindowSurface(maze_window.window);
        robot = robot_init(&maze_window, &myRobot);
        if (robot == NULL)
        {
            printf("failed to load texture\n");
        }
        printf("Texture id: %p\n", robot);
        data.mouse_down = 0;
        data.key_state = NONE;
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
                if (e.type == SDL_KEYDOWN)
                {
                    switch( e.key.keysym.sym )
                    {
                        case SDLK_UP:
                            data.key_state = UP;
                            break;
                        case SDLK_DOWN:
                            data.key_state = DOWN;
                            break;
                        case SDLK_LEFT:
                            data.key_state = LEFT;
                            break;
                        case SDLK_RIGHT:
                            data.key_state = RIGHT;
                            break;
                    }
                }
                if (e.type == SDL_KEYUP)
                {
                    data.key_state = NONE;
                }
            }

            if (data.mouse_down)
            {
                SDL_GetMouseState(&data.mouse_x, &data.mouse_y);
            }

            //Clear screen
            SDL_RenderClear(maze_window.renderer);

            paint = brush(&maze_window, &data, &maze);
            SDL_RenderCopy(maze_window.renderer, paint, NULL, NULL);

            draw_robot(&maze_window, &myRobot, &robot, &data);

            SDL_RenderPresent(maze_window.renderer);
            //Update screen



            SDL_DestroyTexture(paint);
        }
    }
    
    //quit and destroy
    destroy(&maze_window);
    SDL_Quit();
}

