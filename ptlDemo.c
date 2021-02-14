#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PTLIB_IMPL
#include "ptlib.h"

enum directions {
    DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT
};
typedef struct Snake {
    int direction;
    int amount_of_pixels;
    struct ptlPixel* pixels;
} Snake;

typedef struct Food {
    int x, y;
} Food;

int main(){
    ptlRaster screen = ptlInitRaster(25, 20, '.');

    Food food;
    food.x = 5;
    food.y = 10;

    Snake snake;

    int score = 0, high_score = 0;

    float msec = 0, speed = 500;

    int is_running = 1;

    int screen_width = ptlGetWidth(screen);
    int screen_height = ptlGetHeight(screen);

    snake.direction = DIR_UP;
    snake.amount_of_pixels = 4;
    snake.pixels = malloc(sizeof(struct ptlPixel) * snake.amount_of_pixels);
    snake.pixels[0].pixelChar = '@';
    snake.pixels[0].x = 10;
    snake.pixels[0].y = 10;
    for (int i = 1; i < snake.amount_of_pixels; i++) {
        snake.pixels[i].pixelChar = 'O';
        snake.pixels[i].x = snake.pixels[0].x;
        snake.pixels[i].y = snake.pixels[0].y + i;
    }

    for (int i = 0; i < snake.amount_of_pixels; i++)
        ptlDrawPixel(screen, snake.pixels[i].pixelChar, snake.pixels[i].x, snake.pixels[i].y);

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < screen_width; j++) {
            ptlDrawPixel(screen, ' ', j, i);
        }
    }

    ptlDrawLine(screen, '_', 0, 0, screen_width, 0);
    ptlDrawLine(screen, '_', 0, 4, screen_width, 4);
    ptlDrawLine(screen, '_', 1, screen_height - 1, screen_width - 1, screen_height - 1);
    ptlDrawLine(screen, '|', 0, 1, 0, screen_height - 1);
    ptlDrawLine(screen, '|', screen_width - 1, 1, screen_width - 1, screen_height - 1);

    ptlDrawPixel(screen, '*', screen_width - 1, screen_height - 1);
    ptlDrawPixel(screen, '*', 0, screen_height - 1);

    ptlDrawText(screen, (int)(screen_width / 2) - 8, 1, "----SNAKE----");
    //ptlDrawText(screen, 1, 2, "HighScore:");
    ptlDrawText(screen, 1, 3, "Score:");

    snake.direction = DIR_UP;
    snake.amount_of_pixels = 4;
    snake.pixels = malloc(sizeof(struct ptlPixel) * snake.amount_of_pixels);
    snake.pixels[0].pixelChar = '@';
    snake.pixels[0].x = 10;
    snake.pixels[0].y = 10;
    for (int i = 1; i < snake.amount_of_pixels; i++) {
        snake.pixels[i].pixelChar = 'O';
        snake.pixels[i].x = snake.pixels[0].x;
        snake.pixels[i].y = snake.pixels[0].y + i;
    }


    srand(time(NULL));

    clock_t prev_time = clock();


    while (is_running) {
        ptlPollEvents(screen);


        int pressed_key = ptlPressedKey(screen);
        if (pressed_key == KEYCODE_Q) is_running = 0;

        else if (pressed_key == KEYCODE_W) {
            if (snake.direction != DIR_DOWN)
                snake.direction = DIR_UP;
        } else if (pressed_key == KEYCODE_A) {
            if (snake.direction != DIR_RIGHT)
                snake.direction = DIR_LEFT;
        } else if (pressed_key == KEYCODE_S) {
            if (snake.direction != DIR_UP)
                snake.direction = DIR_DOWN;
        } else if (pressed_key == KEYCODE_D) {
            if (snake.direction != DIR_LEFT)
                snake.direction = DIR_RIGHT;
        }

        clock_t difference = clock() - prev_time;
        msec = difference * 1000 / CLOCKS_PER_SEC;

        if (msec > speed) {

            prev_time = clock();
            speed -= 1;
            score += 1;
            char score_text[20];
            _itoa(score, score_text, 10);
            ptlDrawText(screen, 8, 3, score_text);

            // body moves
            ptlRemovePixel(screen, snake.pixels[snake.amount_of_pixels - 1].x, snake.pixels[snake.amount_of_pixels - 1].y);

            if (snake.pixels[0].x == food.x && snake.pixels[0].y == food.y) {
                // snake ate food

                snake.amount_of_pixels++;
                snake.pixels = realloc(snake.pixels, snake.amount_of_pixels * sizeof(struct ptlPixel));
                snake.pixels[snake.amount_of_pixels - 1].pixelChar = 'O';

                int rand_x = 5;
                int rand_y = 10;
                int place_taken = 0;

                do {
                    rand_x = rand() % screen_width - 1;
                    rand_y = rand() % screen_height - 1;
                    if (rand_x <= 0) rand_x += 3;
                    else if (rand_x > screen_width - 1) rand_x -= (int)(screen_width / 2);
                    if (rand_y >= screen_height) rand_y -= (int)(screen_height / 2);
                    if (rand_y <= 5) rand_y += (int)(screen_height / 2);

                    int place_taken = 0;

                    for (int i = 0; i < snake.amount_of_pixels - 1; i++) {
                        if (rand_x == snake.pixels[i].x && rand_y == snake.pixels[i].y) {
                            place_taken = 1;
                            break;
                        }
                    }
                } while (place_taken);

                food.x = rand_x;
                food.y = rand_y;



            }

            for (int i = snake.amount_of_pixels; i > 0; i--) {
                snake.pixels[i].x = snake.pixels[i - 1].x;
                snake.pixels[i].y = snake.pixels[i - 1].y;
            }


            //head moves

            if (snake.pixels[0].x == screen_width - 2 && snake.direction == DIR_RIGHT) {
                snake.pixels[0].x = 1;
            } else if (snake.pixels[0].x == 1 && snake.direction == DIR_LEFT) {
                snake.pixels[0].x = screen_width - 2;
            } else if (snake.pixels[0].y == 5 && snake.direction == DIR_UP) {
                snake.pixels[0].y = screen_height - 2;
            } else if (snake.pixels[0].y == screen_height - 2 && snake.direction == DIR_DOWN) {
                snake.pixels[0].y = 5;
            } else {
                if (snake.direction == DIR_UP) (snake.pixels[0].y)--;
                else if (snake.direction == DIR_DOWN) (snake.pixels[0].y)++;
                else if (snake.direction == DIR_LEFT) (snake.pixels[0].x)--;
                else if (snake.direction == DIR_RIGHT) (snake.pixels[0].x)++;
            }

            ptlDrawPixel(screen, '#', food.x, food.y);

            for (int i = 0; i < snake.amount_of_pixels; i++) {
                ptlDrawPixel(screen, snake.pixels[i].pixelChar, snake.pixels[i].x, snake.pixels[i].y);
            }

            //check for collision
            for (int i = 1; i < snake.amount_of_pixels; i++) {
                if (snake.pixels[i].x == snake.pixels[0].x && snake.pixels[i].y == snake.pixels[0].y) {
                    is_running = 0;
                    ptlDrawPixel(screen, 'X', snake.pixels[0].x, snake.pixels[0].y);
                }
            }

            
            ptlRepaint(screen);

        }

        
/*
        clock_t start = clock();
        clock_t now;
        do {
            now = clock();
        } while (now - start > 1500);
 
 */
    }
 
    
    ptlEndProgram(screen);
    
    return 0;

}

