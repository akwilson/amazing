#include <stdlib.h>
#include <time.h>
#include "graphics.h"

#define CELL_SIZE 25
#define WIDTH 40
#define HEIGHT 25
#define WALL_WIDTH 5

enum CELL_INFO
{
    NORTH = 0x01,
    EAST = 0x02,
    SOUTH = 0x04,
    WEST = 0x08,
    VISITED = 0x10
};

struct maze_cell
{
    int x_pos;
    int y_pos;
    struct maze_cell *next;
};

static int maze[HEIGHT][WIDTH];
static int visit_count;
static struct maze_cell *maze_stack = 0;

static struct maze_cell *make_maze_cell(int x, int y)
{
    struct maze_cell *cell = malloc(sizeof(struct maze_cell));
    cell->x_pos = x;
    cell->y_pos = y;
    cell->next = 0;
    return cell;
}

static void free_stack()
{
    if (maze_stack)
    {
        for (; maze_stack; maze_stack = maze_stack->next)
        {
            free(maze_stack);
        }
    }
}

/**
 * Push the next cell moved to on to the stack.  Mark it as visited.
 */
static void visit_next_cell(int x_adjust, int y_adjust, int cur_dir, int next_dir)
{
    maze[maze_stack->y_pos][maze_stack->x_pos] |= cur_dir;
    maze[maze_stack->y_pos + y_adjust][maze_stack->x_pos + x_adjust] |= next_dir | VISITED;
    struct maze_cell *next_cell = make_maze_cell(maze_stack->x_pos + x_adjust, maze_stack->y_pos + y_adjust);
    next_cell->next = maze_stack;
    maze_stack = next_cell;
    visit_count++;
}

static void calc_maze()
{
    int choices[4];
    int idx = 0;

    if (visit_count < (WIDTH * HEIGHT))
    {
        // Find all possible moves
        if (maze_stack->y_pos > 0 && ((maze[maze_stack->y_pos - 1][maze_stack->x_pos] & VISITED) == 0))
        {
            choices[idx++] = NORTH;
        }

        if (maze_stack->x_pos < (WIDTH - 1) && ((maze[maze_stack->y_pos][maze_stack->x_pos + 1] & VISITED) == 0))
        {
            choices[idx++] = EAST;
        }

        if (maze_stack->y_pos < (HEIGHT - 1) && ((maze[maze_stack->y_pos + 1][maze_stack->x_pos] & VISITED) == 0))
        {
            choices[idx++] = SOUTH;
        }

        if (maze_stack->x_pos > 0 && ((maze[maze_stack->y_pos][maze_stack->x_pos - 1] & VISITED) == 0))
        {
            choices[idx++] = WEST;
        }

        if (idx)
        {
            // Select random move
            int next = choices[rand() % idx];
            
            // Move to the next cell and mark it visited
            switch (next)
            {
            case NORTH:
                visit_next_cell(0, -1, NORTH, SOUTH);
                break;
            case EAST:
                visit_next_cell(1, 0, EAST, WEST);
                break;
            case SOUTH:
                visit_next_cell(0, 1, SOUTH, NORTH);
                break;
            case WEST:
                visit_next_cell(-1, 0, WEST, EAST);
                break;
            }
        }
        else
        {
            // No valid moves -- backtrack by popping off the stack
            struct maze_cell *top = maze_stack;
            maze_stack = maze_stack->next;
            free(top);
        }
    }
    else
    {
        free_stack();
    }
}

static void draw_frame(graphics *graphics)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            int cell_info = maze[y][x];

            // Draw cell
            render_quad(graphics, x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, 1, cell_info & VISITED ? WHITEISH : BLUE);

            // Draw walls
            if (!(cell_info & NORTH))
                render_quad(graphics, x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, WALL_WIDTH, 1, BLACK);

            if (!(cell_info & WEST))
                render_quad(graphics, x * CELL_SIZE, y * CELL_SIZE, WALL_WIDTH, CELL_SIZE, 1, BLACK);
        }
    }
}

int main()
{
    SDL_Event event;
    int running = 1;
    graphics *graphics = init_graphics("Amazing", CELL_SIZE * WIDTH, CELL_SIZE * HEIGHT);
    if (!graphics)
    {
        return 1;
    }

    srand(time(0));
    maze_stack = make_maze_cell(rand() % WIDTH, rand() % HEIGHT);
    maze[maze_stack->y_pos][maze_stack->x_pos] = VISITED;
    visit_count = 1;

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
        calc_maze();
        draw_frame(graphics);
        commit_to_screen(graphics);
    }

    close_graphics(graphics);
    return 0;
}