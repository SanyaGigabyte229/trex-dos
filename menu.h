/*
 * T-Rex Chrome for DOS
 * Copyright (c) 2026 SanyaGigabyte229
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for details.
 */


#ifndef FUNCTIONS_H
#define FUNCTIONS_H

extern unsigned char *screen_buffer;

typedef struct {
	char *text;
	int x, y;
	void (*action)(void);
} MenuItem;

typedef struct {
	MenuItem items[5];
	int count;
	int current_selected;
} MenuWindow;

void clear_buffer(unsigned char color);

void flip_buffer(void);

void draw_pixel(int x, int y, unsigned char color);

void draw_rect(int x, int y, int w, int h, unsigned char color);

void draw_wolf_window(int x, int y, int w, int h);

void draw_char(int x, int y, unsigned char ch, unsigned char color);

void draw_string(int x, int y, const char *str, unsigned char color);

unsigned int get_ticks(void);

void render_menu(MenuWindow *menu);

void menu_loop(MenuWindow *menu);

#endif