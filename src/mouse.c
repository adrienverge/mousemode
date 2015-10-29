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

#include "mouse.h"

#include <X11/extensions/XTest.h>

void mouse_press_button(Display *display, unsigned int button)
{
    XTestFakeButtonEvent(display, button, True, CurrentTime);
}

void mouse_release_button(Display *display, unsigned int button)
{
    XTestFakeButtonEvent(display, button, False, CurrentTime);
}

void mouse_move(Display *display, int dx, int dy)
{
    XEvent event;

    /* Get the current pointer position */
    XQueryPointer(display, RootWindow(display, 0),
                  &event.xbutton.root, &event.xbutton.window,
                  &event.xbutton.x_root, &event.xbutton.y_root,
                  &event.xbutton.x, &event.xbutton.y,
                  &event.xbutton.state);

    // /* Fake the pointer movement to new relative position */
    // XTestFakeMotionEvent(display, 0, event.xbutton.x + dx,
    //         event.xbutton.y + dy, CurrentTime);
    // XSync(display, False);

    XWarpPointer(display, None, event.xbutton.root, 0, 0, 0, 0,
                 event.xbutton.x + dx, event.xbutton.y + dy);
}
