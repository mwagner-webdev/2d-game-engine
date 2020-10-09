/*
	Copyright (c) 2010, Markus Wagner (bgld)
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are
	met:
	- Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	- Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.
	- Neither the name "fawesome" or "FawesomeEngine" nor the names of its contributors
	  may be used to endorse or promote products derived from this software
	  without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
	PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MARKUS WAGNER BE
	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
	THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "pointerevent.h"

#include <iostream>

#include "../globals.h"

pointer_event::pointer_event(const SDL_Event& sdl_event, tcl_bind* bind) : event(sdl_event, bind) {
	switch(sdl_event.type) {
	case SDL_MOUSEBUTTONDOWN:
		buttons[sdl_event.button.button] = (sdl_event.button.state == SDL_PRESSED);
		break;
	case SDL_MOUSEBUTTONUP:
		buttons[sdl_event.button.button] = false;
		break;
	default:
		buttons[sdl_event.button.button] = true;
	}

	m_x = sdl_event.motion.x;
	m_y = sdl_event.motion.y;

	m_x_rel = sdl_event.motion.xrel;
	m_y_rel = sdl_event.motion.yrel;
}

bool pointer_event::button(int16_t button) {
	return buttons[button];
}

int16_t pointer_event::x() {
	return m_x;
}

int16_t pointer_event::y() {
	return m_y;
}

int16_t pointer_event::x_rel() {
	return m_x_rel;
}

int16_t pointer_event::y_rel() {
	return m_y_rel;
}
