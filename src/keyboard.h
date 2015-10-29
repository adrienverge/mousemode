/*
 *  Copyright (C) 2015 Adrien Vergé
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <time.h>
#include <X11/Xlib.h>

static inline unsigned long current_milliseconds()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void set_mapping(Display *display);

void process_keyboard_event(int type, KeyCode keycode);

int is_currently_moving_pointer();

void compute_pointer_movement(unsigned long time, int *dx, int *dy);

void compute_pointer_clicks(int *left_press, int *left_release,
                            int *middle_press, int *middle_release,
                            int *right_press, int *right_release);

#endif
