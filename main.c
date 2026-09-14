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
#include <dos.h>
#include <i86.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>

#include "dino_sprite.h"
#include "cactus_sprite.h"
#include "menu.h"

unsigned char *VGA = (unsigned char *)0xA0000;

int dino_x = 10;
int dino_y = 100;

void set_vga_mode() {
	_setvideomode(_MRES256COLOR);
}

void set_text_mode() {
	_setvideomode(_DEFAULTMODE);
}

void clear_mode(unsigned char clscolor) {
	_fmemset(VGA, clscolor, 64000);
}

void pc_speaker() {
	sound(1000);
	delay(30);
	nosound();
}

void draw_dino_sprite(int x, int y) {
	int rx, ry;

	for (ry = 0; ry < SPRITE_DINO_H; ry++) {
		for (rx = 0; rx < SPRITE_DINO_W; rx++) {
			unsigned char color = dino_sprite[ry][rx];
			if (color != 0) {
				int screen_x = x + rx;
				int screen_y = y + ry;
				if (screen_x >= 0 && screen_x < 320 && screen_y >= 0 && screen_y < 200) {
					VGA[screen_y * 320 + screen_x] = color;
				}
			}
		}
	}
}

void draw_dino2_sprite(int x, int y) {
	int rx, ry;

	for (ry = 0; ry < SPRITE_DINO2_H; ry++) {
		for (rx = 0; rx < SPRITE_DINO2_W; rx++) {
			unsigned char color = dino_sprite2[ry][rx];
			if (color != 0) {
				int screen_x = x + rx;
				int screen_y = y + ry;
				if (screen_x >= 0 && screen_x < 320 && screen_y >= 0 && screen_y < 200) {
					VGA[screen_y * 320 + screen_x] = color;
				}
			}
		}
	}
}

void draw_cactus_sprite(int x, int y) {
	int rx, ry;

	for (ry = 0; ry < SPRITE_CACTUS_H; ry++) {
		for (rx = 0; rx < SPRITE_CACTUS_W; rx++) {
			unsigned char color = cactus_sprite[ry][rx];
			if (color != 0) {
				int screen_x = x + rx;
				int screen_y = y + ry;
				if (screen_x >= 0 && screen_x < 320 && screen_y >= 0 && screen_y < 200) {
					VGA[screen_y * 320 + screen_x] = color;
				}
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

int start_game(void) {
	int frame_counter = 0;
	int is_jumping = 0;
	int jump_v = 0;
	char key = 0;
	int cmx = 280;
	int cmx2 = 140;
	int min_dist = 120;
	srand(time(NULL));
	set_vga_mode();
	while(1) {
		if (kbhit()) {
			key = getch();
			if (key == 32, !is_jumping) {
				is_jumping = 1;
				jump_v = 12;
				pc_speaker();
			}
			if (key == 27) {
				pc_speaker();
				menu_run();
			}
		}
		cmx -= 4;
		cmx2 -= 4;
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

            if (dino_y >= 100) {
                dino_y = 100;
                is_jumping = 0;
                jump_v = 0;
            }
		}
		clear_mode(0);
		frame_counter++;
		if (is_jumping) {
			draw_dino_sprite(dino_x, dino_y);
		} else {
			if ((frame_counter / 4) % 2 == 0) {
				draw_dino_sprite(dino_x, dino_y);
			} else {
				draw_dino2_sprite(dino_x, dino_y);
			}
		}
        draw_cactus_sprite(cmx, 100);
        draw_cactus_sprite(cmx2, 100);
		if (check_collision(dino_x, dino_y, cmx, 100)) {
			pc_speaker();
			delay(1000);
			menu_run();
		}
		if (check_collision(dino_x, dino_y, cmx2, 100)) {
			pc_speaker();
			menu_run();
			break;
		}
		delay(30);
	}
	set_text_mode();
	return 0;
}

void show_options(void) {
	clear_buffer(1);
	flip_buffer();
	getch();
}

void exit_game(void) {
	if (screen_buffer != NULL) {
		free(screen_buffer);
	}
	set_text_mode();
	exit(0);
}

int menu_run() {
	MenuWindow MainMenu;

	/* Выделяем 64 КБ под закадровый буфер */
	screen_buffer = (unsigned char *)malloc(64000);
	if (screen_buffer == NULL) {
		printf("Error: Memory allocation failed!\n");
		return 1;
	}

	set_vga_mode();

	MainMenu.count = 3;
	MainMenu.current_selected = 0;

	MainMenu.items[0].text = "NEW GAME";
	MainMenu.items[0].x = 120;
	MainMenu.items[0].y = 80;
	MainMenu.items[0].action = start_game;

	MainMenu.items[1].text = "OPTIONS";
	MainMenu.items[1].x = 120;
	MainMenu.items[1].y = 100;
	MainMenu.items[1].action = show_options;

	MainMenu.items[2].text = "QUIT";
	MainMenu.items[2].x = 120;
	MainMenu.items[2].y = 120;
	MainMenu.items[2].action = exit_game;

	menu_loop(&MainMenu);

	if (screen_buffer != NULL) {
		free(screen_buffer);
	}

	set_text_mode();
	return 0;
}

int main() {
	menu_run();
}