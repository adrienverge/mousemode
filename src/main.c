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

/*
 * TODO list:
 * - Read DISPLAY env var
 * - Check XTestQueryExtension on startup
 * - Configurable keys
 * - Configurable speed parameters
 * - Close cleanly on signal
 * - Timer 60 seconds to disable mouse mode if no button touched
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "config.h"
#include "keyboard.h"
#include "mouse.h"

#define DISPLAY ":0"

static Display *display;

/*
 *  Mask        | Value | Key
 * -------------+-------+------------
 *  ShiftMask   |     1 | Shift
 *  LockMask    |     2 | Caps Lock
 *  ControlMask |     4 | Ctrl
 *  Mod1Mask    |     8 | Alt
 *  Mod2Mask    |    16 | Num Lock
 *  Mod3Mask    |    32 | Scroll Lock
 *  Mod4Mask    |    64 | Windows
 *  Mod5Mask    |   128 | ???
 */

static void grab_key(int keycode, unsigned int modifiers)
{
    XGrabKey(display, keycode, modifiers, DefaultRootWindow(display),
             True, GrabModeAsync, GrabModeAsync);
    if (modifiers != AnyModifier)
        XGrabKey(display, keycode, modifiers | Mod2Mask /* numlock */,
                 DefaultRootWindow(display), True,
                 GrabModeAsync, GrabModeAsync);
}
static void ungrab_key(int keycode, unsigned int modifiers)
{
    // XUngrabKey(display, keycode, AnyModifier, DefaultRootWindow(display));
    XUngrabKey(display, keycode, modifiers, DefaultRootWindow(display));
    if (modifiers != AnyModifier)
        XUngrabKey(display, keycode, modifiers | Mod2Mask /* numlock */,
                   DefaultRootWindow(display));
}

static void enable_trigger_combination()
{
    unsigned int modifiers = ControlMask | Mod4Mask /* super */;
    int keycode = XKeysymToKeycode(display, XK_Alt_L);

    grab_key(keycode, modifiers);
}

static void disable_trigger_combination()
{
    unsigned int modifiers = ControlMask | Mod4Mask /* super */;
    int keycode = XKeysymToKeycode(display, XK_Alt_L);

    ungrab_key(keycode, modifiers);
}

static int mouse_mode_combination_trigerred(XKeyPressedEvent *keyevent)
{
    return keyevent->type == KeyRelease &&
           keyevent->keycode == XKeysymToKeycode(display, XK_Alt_L);
    // modifiers == ControlMask | Mod4Mask /* super */;
}

static int normal_mode_combination_trigerred(XKeyPressedEvent *keyevent)
{
    return keyevent->type == KeyRelease &&
           keyevent->keycode == XKeysymToKeycode(display, XK_Escape);
    // modifiers == ControlMask | Mod4Mask /* super */;
}

static void process_pointer_movement(unsigned long time)
{
    int dx = 0,
        dy = 0;

    compute_pointer_movement(time, &dx, &dy);

    if (dx || dy) {
        mouse_move(display, dx, dy);
        // printf("moving %d %d\n", dx, dy);
    }
}

static void process_pointer_clicks()
{
    int left_press, left_release,
        middle_press, middle_release,
        right_press, right_release;

    compute_pointer_clicks(&left_press, &left_release,
                           &middle_press, &middle_release,
                           &right_press, &right_release);

    if (left_press) {
        mouse_press_button(display, MOUSE_LEFT_BUTTON);
        printf("left click...\n");
    } else if (left_release) {
        mouse_release_button(display, MOUSE_LEFT_BUTTON);
        printf("         release!\n");
    }

    if (middle_press) {
        mouse_press_button(display, MOUSE_MIDDLE_BUTTON);
        printf("middle click...\n");
    } else if (middle_release) {
        mouse_release_button(display, MOUSE_MIDDLE_BUTTON);
        printf("         release!\n");
    }

    if (right_press) {
        mouse_press_button(display, MOUSE_RIGHT_BUTTON);
        printf("right click...\n");
    } else if (right_release) {
        mouse_release_button(display, MOUSE_RIGHT_BUTTON);
        printf("         release!\n");
    }
}

static int mouse_mode_on;

// #define GRAB_KEYS_NOT_KEYBOARD 1

static void grab_keyboard()
{
#ifdef GRAB_KEYS_NOT_KEYBOARD
    int tab[] = { XK_Escape, XK_H, XK_J, XK_K, XK_L, XK_S, XK_D, XK_F };
    int i;
    for (i = 0; i < sizeof(tab)/sizeof(tab[0]); i++) {
        grab_key(XKeysymToKeycode(display, tab[i]), AnyModifier);
    }
#else
    // TODO: Check return value (== AlreadyGrabbed)
    int ret = XGrabKeyboard(display, DefaultRootWindow(display),
                            /*False, // */ True,
                            /*GrabModeSync, GrabModeSync, // */
                            GrabModeAsync, GrabModeAsync,
                            CurrentTime);
    if (ret) {
        fprintf(stderr, "XGrabKeyboard returned %d\n", ret);
        exit(1);
    }
#endif
}

static void ungrab_keyboard()
{
#ifdef GRAB_KEYS_NOT_KEYBOARD
    int tab[] = { XK_Escape, XK_H, XK_J, XK_K, XK_L, XK_S, XK_D, XK_F };
    int i;
    for (i = 0; i < sizeof(tab)/sizeof(tab[0]); i++) {
        ungrab_key(XKeysymToKeycode(display, tab[i]), AnyModifier);
    }
#else
    XUngrabKeyboard(display, CurrentTime);
#endif
}

static void run_mouse_mode()
{
    mouse_mode_on = 1;

    disable_trigger_combination();

    grab_keyboard();

    // TODO: Do we need this?
    // XTestGrabControl (display, True);

    while (mouse_mode_on) {
        fd_set in_fds;
        int x11_fd = ConnectionNumber(display);

        FD_ZERO(&in_fds);
        FD_SET(x11_fd, &in_fds);

        if (is_currently_moving_pointer()) {
            // Sleep for 20 ms (50 Hz seems to be a good refresh rate).
            usleep(20000);
        } else {
            // Sleep until an event arrives
            select(x11_fd + 1, &in_fds, 0, 0, NULL);
        }

        // Get all X events before doing any pointer action
        while (XPending(display)) {
            XEvent event;

            XNextEvent(display, &event);
            XKeyPressedEvent *keyevent = (XKeyPressedEvent *) &event;
            if (normal_mode_combination_trigerred(keyevent)) {
                mouse_mode_on = 0;
            }

            process_keyboard_event(keyevent->type, keyevent->keycode);
        }

        unsigned long time = current_milliseconds();

        // Process KeyPress and KeyRelease here (and not before), because when
        // a key is maintained down X sends many release-then-press events
        process_pointer_clicks();

        process_pointer_movement(time);
    }

    ungrab_keyboard();

    enable_trigger_combination();
}

static void run_normal_mode()
{
    while (1) {
        XKeyPressedEvent event;
        XNextEvent(display, (XEvent *) &event);

        if (mouse_mode_combination_trigerred(&event)) {
            printf("=== Entering MOUSE mode (press escape to leave) ===\n");

            run_mouse_mode();

            printf("=== Leaving MOUSE mode ===\n");
        }
    }
}

int main(int argc, char **argv)
{
    // read_config();

    /*
     * TODO: XSetErrorHandler(xerror_handler);
     */

    display = XOpenDisplay(DISPLAY);
    if (display == NULL) {
        fprintf(stderr, "Cannot XOpenDisplay\n");
        exit(1);
    }

    set_mapping(display);

    enable_trigger_combination();

    // main loop
    run_normal_mode();

    disable_trigger_combination();

    if (XCloseDisplay(display)) {
        fprintf(stderr, "Cannot XCloseDisplay\n");
        exit(1);
    }

    return EXIT_SUCCESS;
}
