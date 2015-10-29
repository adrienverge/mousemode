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

#ifndef _MOUSE_H
#define _MOUSE_H

#include <X11/Xlib.h>

#define MOUSE_LEFT_BUTTON    1
#define MOUSE_MIDDLE_BUTTON  2
#define MOUSE_RIGHT_BUTTON   3

void mouse_press_button(Display *display, unsigned int button);

void mouse_release_button(Display *display, unsigned int button);

void mouse_move(Display *display, int dx, int dy);

#endif
