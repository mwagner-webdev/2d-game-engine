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

#include "player.h"

#include "../globals.h"
#include "../constants.h"

player::player(SDL_Surface* screen, SDL_Surface* background, surface_cache* cache) : controllable_sprite(screen, background, cache) {
	active(true);
	last_dir = DIR_S;
	keys = 0;
}

void player::push_file(uint8_t dir, const std::string &file) {
	sprite::push_file(dir, file);

	center();
}

bool player::handle(controller_press_event* event) {
	switch(event->sym()) {
	case SDLK_UP:
		keys++;
		last_dir = dir();
		dir(DIR_N);
		animate(true);
		break;
	case SDLK_DOWN:
		keys++;
		last_dir = dir();
		dir(DIR_S);
		animate(true);
		break;
	case SDLK_LEFT:
		keys++;
		last_dir = dir();
		dir(DIR_W);
		animate(true);
		break;
	case SDLK_RIGHT:
		keys++;
		last_dir = dir();
		dir(DIR_E);
		animate(true);
		break;
	}

	return PASS;
}

bool player::handle(controller_release_event* event) {
	switch(event->sym()) {
	case SDLK_UP:
	case SDLK_DOWN:
	case SDLK_LEFT:
	case SDLK_RIGHT:
		keys--;
		break;
	}

	if(keys > 0) {
		switch(event->sym()) {
		case SDLK_UP:
		case SDLK_DOWN:
			if(last_dir != DIR_N && last_dir != DIR_S)
				dir(last_dir);
			break;
		case SDLK_LEFT:
		case SDLK_RIGHT:
			if(last_dir != DIR_W && last_dir != DIR_E)
				dir(last_dir);
			break;
		}
	} else {
		animate(false);
	}

	return PASS;
}
