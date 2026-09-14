/*
 * T-Rex Chrome for DOS
 * Copyright (c) 2026 SanyaGigabyte229
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for details.
 */



#include <stdio.h>
#include <graph.h>
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <i86.h>
#include <time.h>
#include <string.h>

#include "font8x8.h"
#include "menu.h"

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13

extern unsigned char *VGA;

unsigned char *screen_buffer = NULL;

void clear_buffer(unsigned char color) {
	if (screen_buffer != NULL) {
		memset(screen_buffer, color, 64000);
	}
}

void flip_buffer(void) {
	if (screen_buffer != NULL) {
		memcpy(VGA, screen_buffer, 64000);
	}
}

void draw_pixel(int x, int y, unsigned char color) {
	if (x >= 0 && x < 320 && y >= 0 && y < 200) {
		screen_buffer[y * 320 + x] = color;
	}	
}

void draw_rect(int x, int y, int w, int h, unsigned char color) {
	int i, j;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			draw_pixel(x + j, y + i, color);
		}
	}
}

void draw_wolf_window(int x, int y, int w, int h) {
	draw_rect(x, y, w, h, 8);
	draw_rect(x + 2, y + 2, w - 4, h - 4, 7);
	draw_rect(x + 4, y + 4, w - 8, h - 8, 0);
}

void draw_char(int x, int y, unsigned char ch, unsigned char color) {
	int row, col;
	const unsigned char *char_data;

	if (ch < 32 || ch > 126) return;
	char_data = font8x8_basic[ch - 32];

	for (row = 0; row < 8; row++) {
		unsigned char bits = char_data[row];
		for (col = 0; col < 8; col++) {
			if (bits & (0x80 >> col)) {
				draw_pixel(x + col, y + row, color);
			}
		}
	}
}

void draw_string(int x, int y, const char *str, unsigned char color) {
	while (*str) {
		draw_char(x + 1, y + 1, (unsigned char)*str, 0);
		draw_char(x, y, (unsigned char)*str, color);
		x += 8;
		str++;
	}
}

unsigned int get_ticks(void) {
	return (unsigned int)clock();
}

void render_menu(MenuWindow *menu) {
	int i;
	int cursor_y, cursor_x;

	/* 1. Очищаем виртуальный буфер */
	clear_buffer(0);

	/* 2. Рисуем интерфейс в буфер */
	draw_wolf_window(60, 40, 200, 120);
	draw_string(110, 50, "DINOGAME MENU", 12);

	for (i = 0; i < menu->count; i++) {
		unsigned char color = (i == menu->current_selected) ? 14 : 15;
		draw_string(menu->items[i].x, menu->items[i].y, menu->items[i].text, color);
	}

	cursor_y = menu->items[menu->current_selected].y;
	cursor_x = menu->items[menu->current_selected].x - 14;

	if ((get_ticks() / 8) % 2 == 0) {
		draw_string(cursor_x, cursor_y, ">", 14);
	}
	else {
		draw_string(cursor_x + 2, cursor_y, ">", 12);
	}

	/* 3. Копируем готовый буфер на экран (1 за раз) */
	flip_buffer();
}

void menu_loop(MenuWindow *menu) {
	int running = 1;

	while (running) {
		render_menu(menu);

		if (kbhit()) {
			int ch = getch();
			if (ch == 0 || ch == 224) {
				ch = getch();
				if (ch == KEY_UP) {
					menu->current_selected = (menu->current_selected - 1 + menu->count) % menu->count;
				}
				else if (ch == KEY_DOWN) {
					menu->current_selected = (menu->current_selected + 1) % menu->count;
				}
			}
			else if (ch == KEY_ENTER) {
				if (menu->items[menu->current_selected].action != NULL) {
					menu->items[menu->current_selected].action();
				}
			}
		}

		delay(20);
	}
}