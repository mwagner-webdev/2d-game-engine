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

#include "map.h"

#include <iostream>
#include <stdexcept>
#include <limits>
#include <assert.h>
#include <math.h>
#include <SDL/SDL.h>

#include "../globals.h"
#include "player.h"
#include "../tclbind.h"

map::map(SDL_Surface* screen, SDL_Surface* background, surface_cache* cache, tcl_bind* bind, const std::string &file, uint16_t width, uint16_t height) : controllable_sprite(screen, background, cache, file) {
	m_bind = bind;

	active(true);

	if(width * (*surfaces_iter)->w < config->int_value("display_width") || height * (*surfaces_iter)->h < config->int_value("display_height"))
		throw std::runtime_error("Map too small.");

	tile_width = (*surfaces_iter)->w;
	tile_height = (*surfaces_iter)->h;

	SDL_Rect dest;

	map_background = SDL_CreateRGBSurface(
		SDL_SWSURFACE | SDL_SRCALPHA | SDL_RLEACCEL,
		screen->w + tile_width,
		screen->h + tile_height,
		config->int_value("screen_bpp"),

		screen->format->Rmask,
		screen->format->Gmask,
		screen->format->Bmask,
		screen->format->Amask
		);

	dest.x = 0;
	dest.y = 0;
	dest.w = (*surfaces_iter)->w;
	dest.h = (*surfaces_iter)->h;

	// Prevent overflow
	if(width * (*surfaces_iter)->w > std::numeric_limits<Sint16>::max())
		throw std::runtime_error("Map too large.");

	// We're filling the map in x direction first and then multiply the segment, this is much faster

	Sint16 segment_length = (*surfaces_iter)->w;

	SDL_BlitSurface((*surfaces_iter), NULL, map_background, NULL);

	while(segment_length < width * (*surfaces_iter)->w) {
		dest.x = segment_length;

		segment_length *= 2;

		SDL_BlitSurface(map_background, NULL, map_background, &dest);
	}

	dest.x = 0;
	segment_length = (*surfaces_iter)->h;

	while(segment_length < height * (*surfaces_iter)->h) {
		dest.y = segment_length;

		segment_length *= 2;

		SDL_BlitSurface(map_background, NULL, map_background, &dest);
	}

	SDL_FreeSurface((*surfaces_iter));
	(*surfaces_iter) = map_background;

	scroll = false;

	map_width = (tile_width * width);
	map_height = (tile_height * height);

	m_x_min = -map_width + screen->w;
	m_y_min = -map_height + screen->h;

	m_x_max = 0;
	m_y_max = 0;

	m_player = NULL;

	old_x = x();
	old_y = y();
}

map::~map() {
	//dtor
}

int16_t map::follower_obstructed() {
	int16_t obstructed = 0;

	for(
		followers_queue::iterator iter = followers.begin();
		iter != followers.end();
		iter++
	) {
		obstructed |= (*iter)->obstructed(m_player);
	}

	return obstructed;
}

void map::calculate() {
	controllable_sprite::calculate();

	if(!scroll) {
		if((follower_obstructed() & DIR_N) || (follower_obstructed() & DIR_S)) {
			y(old_y);
			m_player->animate(false);
		}

		if((follower_obstructed() & DIR_W) || (follower_obstructed() & DIR_E)) {
			x(old_x);
			m_player->animate(false);
		}

		set_offsets();

		if(!(follower_obstructed() & DIR_N) && !(follower_obstructed() & DIR_S) && old_y != y()) {
			old_y = y();
			m_player->animate(true);
		}

		if(!(follower_obstructed() & DIR_W) && !(follower_obstructed() & DIR_E) && old_x != x()) {
			old_x = x();
			m_player->animate(true);
		}
	} else {
		old_y = y();
		old_x = x();
	}
}

void map::display() {
	SDL_Rect dest;

	dest.x = display_x() % tile_width;
	dest.y = display_y() % tile_height;
	dest.w = map_background->w;
	dest.h = map_background->h;

	SDL_BlitSurface(map_background, NULL, m_screen, &dest);

	SDL_BlitSurface(m_screen, NULL, m_background, NULL);
}

bool map::handle(controller_press_event* event) {
	switch(event->sym()) {
	case SDLK_UP:
		if(y() + 1 < m_y_max && !(follower_obstructed() & DIR_N)) {
			m_speed_y = 1;
			m_target_y += map_height;
		}
		break;
	case SDLK_DOWN:
		if(y() - 1 > m_y_min && !(follower_obstructed() & DIR_S)) {
			m_speed_y = -1;
			m_target_y -= map_height;
		}
		break;
	case SDLK_LEFT:
		if(x() + 1 < m_x_max && !(follower_obstructed() & DIR_W)) {
			m_speed_x = 1;
			m_target_x += map_width;
		}
		break;
	case SDLK_RIGHT:
		if(x() - 1 > m_x_min && !(follower_obstructed() & DIR_E)) {
			m_speed_x = -1;
			m_target_x -= map_width;
		}
		break;
	default:
		if(event->sym() == config->int_value("key_activate")) {
			if(m_player == NULL)
				throw std::runtime_error("No player set for map!");

			int16_t area_x = (config->int_value("display_width") / 2) - (m_player->width() / 2);
			int16_t area_y = (config->int_value("display_height") / 2) - (m_player->height() / 2);

			activate_event* new_event = new activate_event(event->sdl_event(), m_bind);

			event_to_layer_area(new_event, area_x, area_y, m_player->width(), m_player->height());
		}
	}

	return PASS; // Give other event_handlers e.g. players a chance too ;)
}

bool map::handle(controller_release_event* event) {
	switch(event->sym()) {
	case SDLK_UP:
	case SDLK_DOWN:
		stop_movement_y();
		break;
	case SDLK_LEFT:
	case SDLK_RIGHT:
		stop_movement_x();
		break;
	}

	return PASS;
}

bool map::handle(pointer_press_event* event) {
	scroll = (event->button(pointer_event::RIGHT_BUTTON));

	return PASS;
}

bool map::handle(pointer_release_event* event) {
	scroll = (event->button(pointer_event::RIGHT_BUTTON));

	return PASS;
}

bool map::handle(pointer_move_event* event) {
	if(scroll) {
		x(x() + event->x_rel());
		y(y() + event->y_rel());
	}

	return PASS;
}

void map::current_player(player* player) {
	m_player = player;
}
