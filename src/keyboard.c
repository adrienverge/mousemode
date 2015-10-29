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

#include "keyboard.h"

#include <stdio.h>
#include <math.h>
#include <X11/keysym.h>

struct keyboard_mapping {
    KeyCode up;
    KeyCode down;
    KeyCode left;
    KeyCode right;

    KeyCode lclick;
    KeyCode mclick;
    KeyCode rclick;
};

static struct keyboard_mapping mapping;

struct key_state {
    enum { POS_UP, POS_DOWN } position;

    unsigned long press_time;
    unsigned long release_time;
    unsigned long last_action_time;

    enum { ACTION_NONE, ACTION_PRESS, ACTION_RELEASE } pending;
};

struct keyboard_state {
    struct key_state up;
    struct key_state down;
    struct key_state left;
    struct key_state right;

    struct key_state lclick;
    struct key_state mclick;
    struct key_state rclick;
};

static struct keyboard_state kb_state = {
    .up     = { .position = POS_UP },
    .down   = { .position = POS_UP },
    .left   = { .position = POS_UP },
    .right  = { .position = POS_UP },
    .lclick = { .position = POS_UP },
    .mclick = { .position = POS_UP },
    .rclick = { .position = POS_UP }
};

void set_mapping(Display *display)
{
    mapping.up     = XKeysymToKeycode(display, XK_K);
    mapping.down   = XKeysymToKeycode(display, XK_J);
    mapping.left   = XKeysymToKeycode(display, XK_H);
    mapping.right  = XKeysymToKeycode(display, XK_L);

    mapping.lclick = XKeysymToKeycode(display, XK_F);
    mapping.mclick = XKeysymToKeycode(display, XK_D);
    mapping.rclick = XKeysymToKeycode(display, XK_S);
}

static void process_move_event(struct key_state *key_state, int type)
{
    unsigned long time = current_milliseconds();

    // Update position
    if (type == KeyPress) {
        key_state->position = POS_DOWN;

        /*
         * If key has just been released then pressed (this is X's way to tell
         * the key is maintained down), don't change the press time.
         * E.g. if delta <= 10 milliseconds, this cannot be two real keystrokes:
         * this means user is maintaining the key pressed.
         */
        if (key_state->release_time <= time - 10)
            key_state->press_time = time;
    } else if (type == KeyRelease) {
        key_state->position = POS_UP;
        key_state->release_time = time;
    }
}

static void process_click_event(struct key_state *key_state, int type)
{
    if (type == KeyPress) {
        if (key_state->pending == ACTION_NONE)
            key_state->pending = ACTION_PRESS;
        else if (key_state->pending == ACTION_RELEASE)
            key_state->pending = ACTION_NONE;
        else
            printf("should not happen\n");
    } else if (type == KeyRelease) {
        if (key_state->pending == ACTION_NONE)
            key_state->pending = ACTION_RELEASE;
        else if (key_state->pending == ACTION_PRESS)
            key_state->pending = ACTION_NONE;
        else
            printf("should not happen\n");
    }
}

void process_keyboard_event(int type, KeyCode keycode)
{
    if (keycode == mapping.up)
        process_move_event(&kb_state.up, type);
    else if (keycode == mapping.down)
        process_move_event(&kb_state.down, type);
    else if (keycode == mapping.left)
        process_move_event(&kb_state.left, type);
    else if (keycode == mapping.right)
        process_move_event(&kb_state.right, type);
    else if (keycode == mapping.lclick)
        process_click_event(&kb_state.lclick, type);
    else if (keycode == mapping.mclick)
        process_click_event(&kb_state.mclick, type);
    else if (keycode == mapping.rclick)
        process_click_event(&kb_state.rclick, type);
}

int is_currently_moving_pointer()
{
    return kb_state.up.position == POS_DOWN ||
           kb_state.down.position == POS_DOWN ||
           kb_state.left.position == POS_DOWN ||
           kb_state.right.position == POS_DOWN;
}

static void compute_pointer_movement_for_key(unsigned long time,
                                             struct key_state *key_state,
                                             int base_x, int base_y,
                                             int *dx, int *dy)
{
    static int delay = 200;

    if (key_state->position == POS_DOWN) {
        long delta = time - key_state->press_time;
        if (delta >= delay || key_state->last_action_time <= time - delay) {
            unsigned int speed = 1 + 0.2 * exp(0.005 * (delta - delay));
            speed = speed < 32 ? speed : 32;
            // printf("             speed = %u\n", speed);

            *dx += speed * base_x;
            *dy += speed * base_y;

            key_state->last_action_time = time;
        }
    }
}

void compute_pointer_movement(unsigned long time, int *dx, int *dy)
{
    compute_pointer_movement_for_key(time, &kb_state.up, 0, -1, dx, dy);
    compute_pointer_movement_for_key(time, &kb_state.down, 0, 1, dx, dy);
    compute_pointer_movement_for_key(time, &kb_state.left, -1, 0, dx, dy);
    compute_pointer_movement_for_key(time, &kb_state.right, 1, 0, dx, dy);
}

static void compute_pointer_click_for_button(struct key_state *key_state,
                                             int *press, int *release)
{
    *press = key_state->pending == ACTION_PRESS;
    *release = key_state->pending == ACTION_RELEASE;
    key_state->pending = ACTION_NONE;
}

void compute_pointer_clicks(int *left_press, int *left_release,
                            int *middle_press, int *middle_release,
                            int *right_press, int *right_release)
{
    compute_pointer_click_for_button(&kb_state.lclick,
                                     left_press, left_release);
    compute_pointer_click_for_button(&kb_state.mclick,
                                     middle_press, middle_release);
    compute_pointer_click_for_button(&kb_state.rclick,
                                     right_press, right_release);
}
