/*
 * T-Rex Chrome for DOS
 * Copyright (c) 2026 SanyaGigabyte229
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for details.
 */

#include <stdio.h>
#include <string.h>
#include <graph.h>
#include <i86.h>
#include <dos.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>

#include "dino_sprite.h"
#include "cactus_sprite.h"
#include "font3x5.h"

unsigned char *VGA = (unsigned char *)0xA0000;

unsigned char *screen_buffer = NULL;

int dino_x = 10;
int dino_y = 102;
int score = 0;
int game_speed = 4;

void set_vga_mode()
{
	_setvideomode(_MRES256COLOR);
}

void set_text_mode()
{
	_setvideomode(_DEFAULTMODE);
}

//void clear_mode(unsigned char color)
//{
	//memset(VGA, color, 64000);
//}

void clear_buffer(unsigned char color) {
	if (screen_buffer != NULL) {
		memset(screen_buffer, color, 64000);
	}
}

void flip_buffer(void)
{
	if (screen_buffer != NULL) {
		memcpy(VGA, screen_buffer, 64000);
	}
}

void draw_pixel(int x, int y, unsigned char color)
{
	if (x >= 0 && x < 320 && y >= 0 && y < 200) {
		screen_buffer[y * 320 + x] = color;
	}
}

void draw_line(int y, unsigned char color)
{
	int x;
	for (x = 0; x < 320; x++) {
		draw_pixel(x, y, color);
	}
}

void draw_ground(int ys, unsigned char color) {
	int x;

	draw_line(ys, color);

	for (x = 15; x < 320; x += 30) {
		draw_pixel(x, ys + 4, color);
	}

	for (x = 30; x < 320; x += 30) {
		draw_pixel(x, ys + 8, color);
	}
}

void draw_score(int x, int y, int value, unsigned char color)
{
    int i, row, col, digit, digit_x;
    int temp = value;

    for (i = 4; i >= 0; i--) {
        digit = temp % 10;
        temp /= 10;

        digit_x = x + (i * 5);

        for (row = 0; row < 5; row++) {
            unsigned char data = font_3x5[digit][row];

            for (col = 0; col < 3; col++) {
                if ((data >> (2 - col)) & 1) {
                    draw_pixel(digit_x + col, y + row, color);
                }
            }
        }
    }
}

void draw_dino_sprite(int x, int y)
{
	int rx;
	int ry;
	for (ry = 0; ry < SPRITE_DINO_H; ry++) {
		for (rx = 0; rx < SPRITE_DINO_W; rx++) {
			unsigned char color = dino_sprite[ry][rx];
			int screen_x = rx + x;
			int screen_y = ry + y;
			if (color != 15) {
				draw_pixel(x + rx, y + ry, color);
			}
		}
	}
}

void draw_dino_sprite2(int x, int y)
{
	int rx;
	int ry;
	for (ry = 0; ry < SPRITE_DINO2_H; ry++) {
		for (rx = 0; rx < SPRITE_DINO2_W; rx++) {
			unsigned char color = dino_sprite2[ry][rx];
			int screen_x = rx + x;
			int screen_y = ry + y;
			if (color != 15) {
				draw_pixel(x + rx, y + ry, color);
			}
		}
	}
}

void draw_cactus_sprite(int x, int y)
{
	int rx;
	int ry;
	for (ry = 0; ry < SPRITE_CACTUS_H; ry++) {
		for (rx = 0; rx < SPRITE_CACTUS_W; rx++) {
			unsigned char color = cactus_sprite[ry][rx];
			int screen_x = rx + x;
			int screen_y = ry + y;
			if (color != 15) {
				draw_pixel(x + rx, y + ry, color);
			}
		}
	}
}

int check_collision(int dx, int dy, int cx, int cy) {
	int pad = 2;
	if (dx + pad < cx + SPRITE_CACTUS_W - pad &&
	    dx + SPRITE_DINO_W - pad > cx + pad &&
	    dy + pad < cy + SPRITE_CACTUS_H - pad &&
	    dy + SPRITE_DINO_H - pad > cy + pad) 
	{
		return 1;
	}
	return 0;
}

void pc_speaker()
{
	sound(1000);
	delay(30);
	nosound();
}

int main()
{
	int frame_counter = 0;
	int jump_v = 0;
	int is_jumping = 0;
	char key = 0;
	int cmx = 280;
	int cmx2 = 180;
	int min_dist = 120;
	int current_speed;

	if (screen_buffer == NULL) {
		screen_buffer = (unsigned char *)malloc(64000);
		if (screen_buffer == NULL) {
			return 1;
		}
	}

	srand(time(NULL));
	set_vga_mode();

	while (1) {
		if (kbhit()) {
			key = getch();
			if (key == 32 && !is_jumping) {
				is_jumping = 1;
				jump_v = 12;
				pc_speaker();
			}
			if (key == 27) {
				pc_speaker();
				set_text_mode();
				free(screen_buffer);
				screen_buffer = NULL;
				return 0;
			}
		}
		score++;

		current_speed = game_speed + (score / 200) * 1;

		cmx -= current_speed;
		cmx2 -= current_speed;
		if (cmx < -SPRITE_CACTUS_W) {
			cmx = cmx2 + min_dist + (rand() % 61);
			if (cmx < 320) {
				cmx = 320 + (rand() % 40);
			}
		}
		if (cmx2 < -SPRITE_CACTUS_W) {
			cmx2 = cmx + min_dist + (rand() % 61);
			if (cmx2 < 320) {
				cmx2 = 320 + (rand() % 40);
			}
		}

		if (is_jumping) {
			dino_y -= jump_v;
			jump_v -= 2;

			if (dino_y >= 102) {
				dino_y = 102;
				is_jumping = 0;
				jump_v = 0;
			}
		}
		clear_buffer(15);

		draw_ground(123, 0);

		draw_score(280, 10, score, 0);

		frame_counter++;
		if (is_jumping) {
			draw_dino_sprite(dino_x, dino_y);
		} else{
			if ((frame_counter / 4) % 2 == 0) {
				draw_dino_sprite(dino_x, dino_y);
			} else{
				draw_dino_sprite2(dino_x, dino_y);
			}
			
		}
		draw_cactus_sprite(cmx, 100);
		draw_cactus_sprite(cmx2, 100);

		flip_buffer();

		if (check_collision(dino_x, dino_y, cmx, 100) || check_collision(dino_x, dino_y, cmx2, 100)){
			pc_speaker();
			delay(1000);
			dino_y = 102;
			is_jumping = 0;
			jump_v = 0;
			cmx = 280;
			cmx2 = 180;
			frame_counter = 0;
			score = 0;
			current_speed = 4;
			while(kbhit()) getch();
			continue;
		}

		delay(30);
	}
	set_text_mode();
	return 0;
}
