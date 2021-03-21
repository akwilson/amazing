#include <stdlib.h>
#include <time.h>
#include "graphics.h"

int main()
{
    SDL_Event event;
    int running = 1;
    graphics *graphics = init_graphics("Amazing", 1000, 625);
    if (!graphics)
    {
        return 1;
    }

    srand(time(0));

    while (running)
    {
        while (SDL_PollEvent(&event))
        {   
            switch (event.type)
            {
            case SDL_QUIT: 
                running = 0;
                break;
            case SDL_KEYDOWN:
                break;
            }
        }

        clear_frame(graphics);
        commit_to_screen(graphics);
    }

    close_graphics(graphics);
    return 0;
}