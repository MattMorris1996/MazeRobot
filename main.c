#include "stdio.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "math.h"

const char *APP_NAME = "Robot Maze";
const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 900;
const double PI = 3.14159265359;

enum KeyStates
{
    UP,
    DOWN,
    RIGHT,
    LEFT,
    NONE
};

struct GameWindow
{
    SDL_Window *window;
    SDL_Surface *surface;
    SDL_Renderer *renderer;
};

struct GameData
{
    int mouse_x;
    int mouse_y;
    int mouse_down;
    int key_state;
};

struct Sensors
{
    int forward;
    int left;
    int right;
};

struct Robot
{
    double position_x;
    double position_y;
    double orientation;
    int width;
    int height;
    struct Sensors sensors;
};

int init(struct GameWindow *pmaze_window)
{
    //error variable
    int success = 1;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = 0;
    }
    else
    {
        //create window
        pmaze_window->window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        if (pmaze_window->window == NULL)
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
    SDL_Texture *output;
    SDL_Rect fillRect = {pdata->mouse_x - 5, pdata->mouse_y - 5, 10, 10};
    if (pdata->mouse_down)
    {
        SDL_FillRect(*pmaze, &fillRect, 0xFF00FF00);
    }
    SDL_SetColorKey(*pmaze, SDL_TRUE, SDL_MapRGB((*pmaze)->format, 0x00, 0x00, 0x00));
    output = SDL_CreateTextureFromSurface(pmaze_window->renderer, *pmaze);
    return output;
}

SDL_Texture *robot_init(struct GameWindow *pmaze_window, struct Robot *robot)
{
    SDL_Texture *robot_texture;
    SDL_Surface *loaded_surface = IMG_Load( "robot.bmp" );
    if( loaded_surface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", "robot.bmp", IMG_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        SDL_SetColorKey( loaded_surface, SDL_TRUE, SDL_MapRGB( loaded_surface->format, 0, 0xFF, 0xFF ) );
        robot_texture = SDL_CreateTextureFromSurface( pmaze_window->renderer, loaded_surface );
        if( robot_texture == NULL )
        {
            printf( "Unable to create texture from %s! SDL Error: %s\n", "robot.bmp", SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loaded_surface );
    }

    robot->position_x = WINDOW_WIDTH / 2;
    robot->position_y = WINDOW_HEIGHT / 2;
    robot->orientation = 45;
    robot->height = 80;
    robot->width = 80;
    robot->sensors.forward = 0;
    robot->sensors.left = 0;
    robot->sensors.right = 0;

    return robot_texture;
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp)
    {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0; /* shouldn't happen, but avoids warnings */
    }
}

void draw_robot(struct GameWindow *pmaze_window, struct Robot *robot, SDL_Texture **robot_texture, struct GameData *pdata, SDL_Surface **surface)
{
    //SDL_SetRenderTarget(pmaze_window->renderer, *robot_texture);
    //SDL_SetRenderDrawColor(pmaze_window->renderer, 0x00, 0xFF, 0xFF, 0xFF);

    double unit_y = SDL_cos(robot->orientation * (PI / 180));
    double unit_x = SDL_sin(robot->orientation * (PI / 180));

    double normal_unit_y = SDL_cos((robot->orientation + 90) * (PI / 180));
    double normal_unit_x = SDL_sin((robot->orientation + 90) * (PI / 180));

    SDL_SetRenderDrawColor(pmaze_window->renderer, 0xFF, 0x00, 0x00, 0xFF);

    double x_test = robot->position_x + robot->width / 2;
    double y_test = robot->position_y + robot->height / 2;
    double x_normal = robot->position_x + robot->width / 2;
    double y_normal = robot->position_y + robot->height / 2;

    double n_x_normal = robot->position_x + robot->width / 2;
    double n_y_normal = robot->position_y + robot->height / 2;

    robot->sensors.forward = 0;
    robot->sensors.right = 0;
    robot->sensors.left = 0;

    for (int i = 0; i < 200; i++)
    {
        x_test += unit_x;
        y_test -= unit_y;

        x_normal += normal_unit_x;
        y_normal -= normal_unit_y;

        n_x_normal -= normal_unit_x;
        n_y_normal += normal_unit_y;

        if (y_test > 0 && y_test < WINDOW_HEIGHT && x_test > 0 && x_test < WINDOW_WIDTH)
        {
            if ((getpixel(*surface, SDL_floor(x_test), SDL_floor(y_test)) == 0xFF00FF00))
            {
                robot->sensors.forward = 1;
            }
        }
        else
        {
            robot->sensors.forward = 1;
        }
        if (y_normal > 0 && y_normal < WINDOW_HEIGHT && x_normal > 0 && x_normal < WINDOW_WIDTH)
        {
            if ((getpixel(*surface, SDL_floor(x_normal), SDL_floor(y_normal)) == 0xFF00FF00))
            {
                robot->sensors.right = 1;
            }
        }
        else
        {
            robot->sensors.right = 1;
        }
        if (n_y_normal > 0 && n_y_normal < WINDOW_HEIGHT && n_x_normal > 0 && n_x_normal < WINDOW_WIDTH)
        {
            if ((getpixel(*surface, SDL_floor(n_x_normal), SDL_floor(n_y_normal)) == 0xFF00FF00))
            {
                robot->sensors.left = 1;
            }
        }
        else
        {
            robot->sensors.left = 1;
        }

        if (!robot->sensors.forward)
        {
            SDL_RenderDrawPoint(pmaze_window->renderer, SDL_floor(x_test), SDL_floor(y_test));
        }
        if (!robot->sensors.right)
        {
            SDL_RenderDrawPoint(pmaze_window->renderer, SDL_floor(x_normal), SDL_floor(y_normal));
        }
        if (!robot->sensors.left)
        {
            SDL_RenderDrawPoint(pmaze_window->renderer, SDL_floor(n_x_normal), SDL_floor(n_y_normal));
        }
    }
    SDL_SetRenderDrawColor(pmaze_window->renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    switch (pdata->key_state)
    {
    case UP:
        robot->position_y -= unit_y;
        robot->position_x += unit_x;
        break;
    case DOWN:
        robot->position_y += unit_y;
        robot->position_x -= unit_x;
        break;
    case RIGHT:
        robot->orientation += 1;
        break;
    case LEFT:
        robot->orientation -= 1;
        break;
    case NONE:
        break;
    }

    printf("robot sensors forward: %d right: %d left %d\n", robot->sensors.forward, robot->sensors.right, robot->sensors.left);

    if (robot->sensors.forward == 0)
    {
        robot->position_y -= unit_y;
        robot->position_x += unit_x;
    }
    else if (robot->sensors.right && robot->sensors.left)
    {
        robot->orientation += 4;
    }
    else if (robot->sensors.forward && robot->sensors.left)
    {
        robot->orientation += 4;
    }
    else if (robot->sensors.forward && robot->sensors.right)
    {
        robot->orientation += 4;
        printf("orientation %f\n", robot->orientation);
    }
    else
    {
        robot->orientation += 4;
    }

    SDL_Rect renderQuad = {SDL_floor(robot->position_x), SDL_floor(robot->position_y), robot->width, robot->height};
    //SDL_RenderClear(pmaze_window->renderer);
    //SDL_SetRenderDrawColor(pmaze_window->renderer, 0x00, 0x00, 0xFF, 0xFF);

    SDL_SetRenderTarget(pmaze_window->renderer, NULL);
    SDL_RenderCopyEx(pmaze_window->renderer, *robot_texture, NULL, &renderQuad, robot->orientation, NULL, SDL_FLIP_NONE);


    //SDL_RenderCopyEx(pmaze_window->renderer,*robot_texture, NULL, &renderQuad, robot->orientation, NULL, SDL_FLIP_NONE);

    //SDL_SetRenderTarget(pmaze_window->renderer,NULL);
}

int main(int argc, char *argv[])
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

    if (init(&maze_window) == 0)
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
        while (!quit)
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
                    switch (e.key.keysym.sym)
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

            //Render Paint Layer
            paint = brush(&maze_window, &data, &maze);
            SDL_RenderCopy(maze_window.renderer, paint, NULL, NULL);

            //Render Robot
            draw_robot(&maze_window, &myRobot, &robot, &data, &maze);

            //Update screen
            SDL_RenderPresent(maze_window.renderer);

            //Destroy Texture
            SDL_DestroyTexture(paint);
        }
    }

    //quit and destroy
    destroy(&maze_window);
    SDL_Quit();
}
