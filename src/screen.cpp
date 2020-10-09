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

#include "screen.h"

#include <iostream>
#include <algorithm>

#include <SDL/SDL_rotozoom.h>

#include "events/activateevent.h"
#include "constants.h"
#include "file.h"

screen::screen(event_queue* queue) {
	m_queue = queue;

	m_cache = new surface_cache();

	// Screen setup

	int sdl_flags = SDL_SWSURFACE | SDL_DOUBLEBUF; // SDL_SWSURFACE is actually faster here for zooming etc.

	if(config->bool_value("fullscreen"))
		sdl_flags |= SDL_FULLSCREEN;
	if(!config->bool_value("frame"))
		sdl_flags |= SDL_NOFRAME;

	screen_surface = SDL_SetVideoMode(
		config->int_value("screen_width"),
		config->int_value("screen_height"),
		config->int_value("screen_bpp"),
		sdl_flags
	);

	if(screen_surface == NULL)
		error_message("Could not set video mode!");

	background = SDL_CreateRGBSurface(
		SDL_SWSURFACE | SDL_SRCALPHA | SDL_RLEACCEL,
		config->int_value("display_width"),
		config->int_value("display_height"),
		config->int_value("screen_bpp"),

		screen_surface->format->Rmask,
		screen_surface->format->Gmask,
		screen_surface->format->Bmask,
		screen_surface->format->Amask
	);

	temp_screen = SDL_DisplayFormat(background);
	zoomed_screen = NULL;

	// Rect for letterboxing

	display_rect = background->clip_rect;

	display_rect.x = (config->int_value("screen_width") - (config->int_value("display_width") * config->int_value("screen_zoom"))) / 2;
	display_rect.y = (config->int_value("screen_height") - (config->int_value("display_height") * config->int_value("screen_zoom"))) / 2;

	tint_surface = SDL_DisplayFormat(background);
	SDL_SetAlpha(tint_surface, SDL_SRCALPHA, 0);

	// FPS

	frameskip = 0;
	do_frameskip = config->bool_value("frameskip");

	limiter = new frame_limiter(HARD_FPS_LIMIT);

	std::string font_name = file(config->value("font"));
	fps_font = TTF_OpenFont(font_name.c_str(), config->int_value("font_size"));
	if(fps_font == NULL) {
		error_message("Could not load font: ", TTF_GetError());
	}

	fps_color.r = FG_COLOR_R;
	fps_color.g = FG_COLOR_G;
	fps_color.b = FG_COLOR_B;
	fps_text = TTF_RenderText_Blended(fps_font, "0 FPS", fps_color);
}

screen::~screen() {
	for(
		sprite_container::iterator iter = sprites.begin();
		iter != sprites.end();
		iter++
	) {
		delete (*iter);
	}

	SDL_FreeSurface(screen_surface);
	SDL_FreeSurface(background);
	SDL_FreeSurface(temp_screen);
	SDL_FreeSurface(zoomed_screen);
	SDL_FreeSurface(tint_surface);

	delete limiter;

	delete m_cache;

	free(fps_font);
	SDL_FreeSurface(fps_text);
}

void screen::serialize(std::ostream& stream) {
	for(
		sprite_container::iterator iter = sprites.begin();
		iter != sprites.end();
		iter++
	) {
		(*iter)->serialize(stream);
	}
}

void screen::deserialize(std::istream& stream) {
	stream >> frameskip;
}

void screen::display() {
	bool new_fps = limiter->new_fps();
	bool flip = false;

	// Calculate coordinates

	std::vector<sprite_container::iterator> updated_sprites;

	for(
		sprite_container::iterator iter = sprites.begin();
		iter != sprites.end();
		iter++
	) {
		(*iter)->calculate();

		if((*iter)->coords_updated())
			updated_sprites.push_back(iter);
	}

	// Reinsert all elements that changed

	for(
		std::vector<sprite_container::iterator>::iterator iter = updated_sprites.begin();
		iter != updated_sprites.end();
		iter++
	) {
		sprite* tmp = **iter;
		sprites.erase(*iter);
		sprites.insert(tmp);
	}

	// Display all sprites if we are not frameskipping

	if(!do_frameskip || frameskip >= MAX_FRAMESKIP || limiter->fps() >= FPS_TOLERANCE_FACTOR * limiter->fps_limit()) {
		for(
			sprite_container::iterator iter = sprites.begin();
			iter != sprites.end();
			iter++
		) {
			(*iter)->display();
		}

		if(do_frameskip)
			frameskip--;

		flip = true;
	} else if(do_frameskip) {
		frameskip++;
	}

	if(new_fps) {
		fps_stream.str("");
		fps_stream << limiter->fps() << " FPS";

		SDL_FreeSurface(fps_text);
		fps_text = TTF_RenderText_Blended(fps_font, fps_stream.str().c_str(), fps_color);
	}

	SDL_Rect fps_rect = {
		config->int_value("display_width") - fps_text->w - FPS_MARGIN_RIGHT,
		FPS_MARGIN_TOP,
		fps_text->w,
		fps_text->h
	};

	SDL_BlitSurface(fps_text, NULL, temp_screen, &fps_rect);

	SDL_BlitSurface(tint_surface, NULL, temp_screen, NULL);

	SDL_FreeSurface(zoomed_screen);
	zoomed_screen = zoomSurface(temp_screen, config->int_value("screen_zoom"), config->int_value("screen_zoom"), 0);
	SDL_BlitSurface(zoomed_screen, NULL, screen_surface, &display_rect);

	if(flip) {
		SDL_Flip(screen_surface);

		limiter->sleep_till_next();
	}
}

void screen::reset_frameskip() {
	frameskip = 0;
}

void screen::tint(uint8_t r, uint8_t g, uint8_t b, uint8_t a, int16_t rgamma, int16_t ggamma, int16_t bgamma) {
	SDL_FillRect(tint_surface, NULL, SDL_MapRGB(tint_surface->format, r, g, b));
	SDL_SetAlpha(tint_surface, SDL_SRCALPHA, a);

	SDL_SetGamma(rgamma / 256.0, ggamma / 256.0, bgamma / 256.0);
}

void screen::push(sprite* sprite) {
	sprites.insert(sprite);
}

map* screen::new_map(const std::string& file, uint16_t width, uint16_t height, tcl_bind* bind) {
	map* map_sprite = NULL;

	try {
		map_sprite = new map(temp_screen, background, m_cache, bind, file, width, height);
		push(map_sprite);
		m_queue->register_handler(map_sprite);
	} catch(file_not_found_exception e) {
		exception_message(e, ": no such file or directory or missing library for file type.");
	}

	return map_sprite;
}

layer* screen::new_layer() {
	return new layer();
}
