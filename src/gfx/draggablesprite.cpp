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

#include "draggablesprite.h"

#include <iostream>

draggable_sprite::draggable_sprite(SDL_Surface* screen, SDL_Surface* background, surface_cache* cache, const std::string &file) : controllable_sprite(screen, background, cache, file) {
	active(true);
	drag = false;
}

bool draggable_sprite::handle(pointer_press_event* event) {
	if(
		event->button(pointer_event::LEFT_BUTTON) &&

		event->x() > display_x() &&
		event->x() < display_x() + width() &&

		event->y() > display_y() &&
		event->y() < display_y() + height()
	) {
		drag = true;
		alpha_to(128, 5);

		return event_handler::END;
	}

	return event_handler::PASS;
}

bool draggable_sprite::handle(pointer_release_event* event) {
	if(!event->button(pointer_event::LEFT_BUTTON)) {
		drag = false;
		alpha_to(255, 5);
	}

	return event_handler::PASS;
}

bool draggable_sprite::handle(pointer_move_event* event) {
	if(drag) {
		//std::cout << "drag" << std::endl;
		x(x() + event->x_rel());
		y(y() + event->y_rel());
	}

	return event_handler::PASS;
}


