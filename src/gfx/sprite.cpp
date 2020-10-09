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

#include "sprite.h"

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>

#include "../constants.h"
#include "../globals.h"

#include "../filenotfoundexception.h"
#include "player.h"
#include "../file.h"

void sprite::init(SDL_Surface* screen, SDL_Surface* background, surface_cache* cache) {
	m_screen = screen;
	m_background = background;

	m_cache = cache;

	m_obstruct = false;

	m_dir = DIR_NONE;
	m_animate = false;
	m_anim_counter = 0;
	m_anim_wait = 10;

	speed_alpha = 0;
	speed_alpha_cycle = 0;
	speed_rotation = 0;
	speed_rotation_cycle = 0;

	current_alpha = SDL_ALPHA_OPAQUE;
	target_alpha = SDL_ALPHA_OPAQUE;
	m_alpha_cycle = false;

	m_angle = 0;
	m_target_angle = 0;
	m_rotation_cycle = false;

	m_font = NULL;
	m_text_color.r = FG_COLOR_R;
	m_text_color.g = FG_COLOR_G;
	m_text_color.b = FG_COLOR_B;

	m_text_offset_x = 0;
	m_text_offset_y = 0;

	text_surface_update = true;

	m_obs_offset_top = 0;
	m_obs_offset_right = 0;
	m_obs_offset_bottom = 0;
	m_obs_offset_left = 0;

	surfaces_iter = surfaces[0].end();
}

sprite::sprite(SDL_Surface* screen, SDL_Surface* background, surface_cache* cache, const std::string &file) : gfx_object() {
	init(screen, background, cache);

	push_file(file);
}

sprite::sprite(SDL_Surface* screen, SDL_Surface* background, surface_cache* cache) : gfx_object() {
	init(screen, background, cache);

	surfaces_iter = surfaces[m_dir].end();
}

sprite::~sprite() { }

void sprite::serialize(std::ostream& stream) {
	stream.write((char*)&m_obstruct, sizeof(m_obstruct));

	for(
		std::map<int, std::vector<std::string> >::iterator iter = files.begin();
		iter != files.end();
		iter++
	) {
		stream.write((char*)&iter->first, sizeof(iter->first));

		for(
			std::vector<std::string>::iterator jter = iter->second.begin();
			jter != iter->second.end();
			jter++
		) {
			stream << *jter << (char)0;
		}

		stream << (char)0;
	}

	stream.write((char*)&m_dir, sizeof(m_dir));
	stream.write((char*)&m_animate, sizeof(m_animate));
	stream.write((char*)&m_anim_counter, sizeof(m_anim_counter));
	stream.write((char*)&m_anim_wait, sizeof(m_anim_wait));

	stream.write((char*)&speed_alpha, sizeof(speed_alpha));
	stream.write((char*)&speed_alpha_cycle, sizeof(speed_alpha_cycle));
	stream.write((char*)&speed_rotation_cycle, sizeof(speed_rotation_cycle));
	stream.write((char*)&speed_rotation_cycle, sizeof(speed_rotation_cycle));

	stream.write((char*)&current_alpha, sizeof(current_alpha));
	stream.write((char*)&target_alpha, sizeof(target_alpha));
	stream.write((char*)&m_alpha_cycle, sizeof(m_alpha_cycle));

	stream.write((char*)&m_angle, sizeof(m_angle));
	stream.write((char*)&m_target_angle, sizeof(m_target_angle));
	stream.write((char*)&m_rotation_cycle, sizeof(m_rotation_cycle));

	stream.write((char*)&m_text_color, sizeof(m_text_color));

	stream.write((char*)&m_text_offset_x, sizeof(m_text_offset_x));
	stream.write((char*)&m_text_offset_y, sizeof(m_text_offset_y));

	stream.write((char*)&text_surface_update, sizeof(text_surface_update));

	stream.write((char*)&m_obs_offset_top, sizeof(m_obs_offset_top));
	stream.write((char*)&m_obs_offset_right, sizeof(m_obs_offset_right));
	stream.write((char*)&m_obs_offset_bottom, sizeof(m_obs_offset_bottom));
	stream.write((char*)&m_obs_offset_left, sizeof(m_obs_offset_left));

	stream.write((char*)&m_x, sizeof(m_x));
	stream.write((char*)&m_y, sizeof(m_y));

	stream.write((char*)&m_offset_x, sizeof(m_offset_x));
	stream.write((char*)&m_offset_y, sizeof(m_offset_y));
}

void sprite::deserialize(std::istream& stream) {

}

void sprite::push_file(int16_t dir, const std::string &file) {
	SDL_Surface* image_surface = m_cache->fetch(file);

	files[dir].push_back(file);
	surfaces[dir].push_back(image_surface);

	surfaces_iter = surfaces[dir].begin();
	m_dir = dir;

	m_x_min = -(*surfaces_iter)->w;
	m_x_max = m_screen->w;

	m_y_min = -(*surfaces_iter)->h;
	m_y_max = m_screen->h;
}

void sprite::push_file(const std::string &file) {
	push_file(DIR_NONE, file);
}

bool sprite::has_alpha() {
	return(config->bool_value("surface_alpha") && current_alpha != SDL_ALPHA_OPAQUE);
}

void sprite::obstruct(bool obstruct, int16_t offset_top, int16_t offset_right, int16_t offset_bottom, int16_t offset_left) {
	m_obs_offset_top = offset_top;
	m_obs_offset_right = offset_right;
	m_obs_offset_bottom = offset_bottom;
	m_obs_offset_left = offset_left;

	m_obstruct = obstruct;
}

uint16_t sprite::obstructed(player* player) const {
	int16_t ret = DIR_NONE;

	int32_t disp_x = display_x();
	int32_t disp_y = display_y();

	if(
		m_obstruct &&
		disp_x > -(m_screen->w) &&
		disp_x <  (m_screen->w) && // this improves performance
		disp_y > -(m_screen->h) && // by an extreme margin
		disp_y <  (m_screen->h)
	) {
		int32_t x_left = disp_x + m_obs_offset_left;
		int32_t x_middle = (m_screen->w / 2);
		int32_t x_right = disp_x + width() + m_obs_offset_right;

		int32_t y_top = disp_y - (player->height() / 2) + m_obs_offset_top;
		int32_t y_middle = (m_screen->h / 2);
		int32_t y_bottom = disp_y + height() + m_obs_offset_bottom;


		if(
			x_left < x_middle &&
			x_right > x_middle
		) {
			if(
				y_top < y_middle &&
				y_bottom + 1 > y_middle
			) {
				ret |= DIR_N;
			}

			if(
				y_top - 1 < y_middle &&
				y_bottom > y_middle
			) {
				ret |= DIR_S;
			}
		}

		if(
			y_top < y_middle &&
			y_bottom > y_middle
		) {
			if(
				x_left < x_middle &&
				x_right + 1 > x_middle
			) {
				ret |= DIR_W;
			}

			if(
				x_left - 1 < x_middle &&
				x_right > x_middle
			) {
				ret |= DIR_E;
			}
		}
	}

	return ret | gfx_object::obstructed(player);
}

void sprite::calculate() {
	gfx_object::calculate();

	if(m_animate) {
		// Cycle through surfaces if counter reached or has been restarted

		if(m_anim_counter % m_anim_wait == 0) {
			surfaces_iter++;

			if(surfaces_iter == surfaces[m_dir].end())
				surfaces_iter = surfaces[m_dir].begin();

			m_anim_counter = 0;
		}

		m_anim_counter++;
	} else {
		surfaces_iter = surfaces[m_dir].begin();
	}
}

void sprite::display() {
	if(current_alpha != target_alpha) {
		if(
			(
			target_alpha <= current_alpha + speed_alpha ||
			target_alpha >= current_alpha - speed_alpha
			) &&
			current_alpha + speed_alpha >= 0 &&
			current_alpha + speed_alpha < 255
			) {
			alpha(current_alpha + speed_alpha);
		} else {
			alpha(target_alpha);
		}
	}

	if(m_rotation_cycle) {
		m_target_angle = m_angle + speed_rotation_cycle;
	}

	if(m_angle != m_target_angle) {
		m_angle += speed_rotation;
		m_angle %= 360;
	}

	display(m_x, m_y);
}

void sprite::display(int16_t x, int16_t y) {
	if(m_alpha_cycle) {
		int16_t cycle_alpha;

		if(current_alpha + speed_alpha_cycle <= m_alpha_min || current_alpha + speed_alpha_cycle >= m_alpha_max) {
			speed_alpha_cycle = -speed_alpha_cycle;
		}

		cycle_alpha = current_alpha + speed_alpha_cycle;

		if(cycle_alpha < m_alpha_min) {
			cycle_alpha = m_alpha_min;
		} else if(cycle_alpha > m_alpha_max) {
			cycle_alpha = m_alpha_max;
		}

		alpha(cycle_alpha);
	}

	display(x, y, current_alpha);
}

void sprite::display(int16_t x, int16_t y, uint8_t alpha) {
	if(!surfaces[m_dir].empty() && alpha != SDL_ALPHA_TRANSPARENT) {
#ifdef DEBUG
		std::stringstream text_stream;
		text_stream << "x: " << m_x << "; y: " << m_x << std::endl;
		text_stream << "offset_x: " << m_offset_x << "; offset_y: " << m_offset_y << std::endl;
		text_stream << "display_x: " << display_x() << "; display_y: " << display_y() << std::endl;
		text_stream << "layer_ID: " << layer_id() << std::endl;
		text(text_stream.str());
#endif

		SDL_Surface* last_surface = (*surfaces_iter);
		SDL_Rect dest_rect = {display_x(), display_y(), (*surfaces_iter)->w, (*surfaces_iter)->h};

		this->x(x);
		this->y(y);
		this->alpha(alpha);

		// Rotozoom stuff

		SDL_Surface* rotozoomed_surface;

		if(m_angle != 0) {
			rotozoomed_surface = rotozoomSurface(last_surface, m_angle, 1, INTERPOLATE_ROTOZOOM);
		} else {
			rotozoomed_surface = last_surface;
		}

		dest_rect.x += ((last_surface->w - rotozoomed_surface->w) / 2);
		dest_rect.y += ((last_surface->h - rotozoomed_surface->h) / 2);

		// Blitting

		if(
			dest_rect.x > -rotozoomed_surface->w &&
			dest_rect.y > -rotozoomed_surface->h &&
			dest_rect.x < m_screen->w &&
			dest_rect.y < m_screen->h
			) {
			if(has_alpha())
				SDL_BlitSurface(m_screen, NULL, m_background, NULL);

			if(SDL_BlitSurface(rotozoomed_surface, NULL, m_screen, &dest_rect) < 0) {
				throw std::runtime_error("Couldn't blit rotozoomed_surface");
			}
		}

		if(m_angle != 0)
			SDL_FreeSurface(rotozoomed_surface);

		// Loop through text lines and add them on top

		if(!text_lines.empty()) {
			SDL_Rect font_rect = dest_rect;
			font_rect.x = display_x() + m_text_offset_x;
			font_rect.y = display_y() + m_text_offset_y;
			SDL_Rect temp_rect = font_rect;

			int16_t line_skip = config->int_value("font_skip");

			if(line_skip == 0)
				line_skip = TTF_FontLineSkip(m_font);

			for(
				std::vector<std::pair<std::string, SDL_Surface*> >::iterator iter = text_lines.begin();
				iter != text_lines.end();
				iter++
			) {
				font_rect.y += line_skip;

				if((*iter).second == NULL || text_surface_update) { // If line surface doesn't exist yet/is stale
					SDL_FreeSurface((*iter).second);
					(*iter).second = TTF_RenderUTF8_Blended(m_font, (*iter).first.c_str(), m_text_color);
				}

				SDL_BlitSurface((*iter).second, NULL, m_screen, &temp_rect);
				temp_rect = font_rect;
			}

			text_surface_update = false;
		}

		// Apply alpha by overlaying the surface with its background (SDL doesn't support anything else)

		if(has_alpha()) {
			SDL_Surface* alpha_bg = SDL_DisplayFormat(m_background);
			SDL_SetAlpha(alpha_bg, SDL_SRCALPHA | SDL_RLEACCEL, SDL_ALPHA_OPAQUE - current_alpha);

			SDL_BlitSurface(m_background, &dest_rect, alpha_bg, &dest_rect);
			SDL_BlitSurface(alpha_bg, &dest_rect, m_screen, &dest_rect);

			SDL_FreeSurface(alpha_bg);
			alpha_bg = NULL;
		}

		prev_rect = dest_rect;
	}
}

void sprite::stop_movement_x() {
	m_speed_x = 0;

	m_target_x = m_x;
}

void sprite::stop_movement_y() {
	m_speed_y = 0;

	m_target_y = m_y;
}

void sprite::stop_movement() {
	stop_movement_x();
	stop_movement_y();
}

void sprite::move(int16_t x, int16_t y, uint16_t speed) {
	m_target_x = x;
	m_target_y = y;

	if(x > m_x) {
		m_speed_x = speed;
	} else if(x < m_x) {
		m_speed_x = -speed;
	} else {
		m_speed_x = 0;
	}

	if(y > m_y) {
		m_speed_y = speed;
	} else if(y < m_y) {
		m_speed_y = -speed;
	} else {
		m_speed_y = 0;
	}
}

void sprite::move_relative(int16_t x, int16_t y, uint16_t speed) {
	move(m_x + x, m_y + y, speed);
}

void sprite::stop_alpha() {
	speed_alpha = 0;

	target_alpha = current_alpha;
}

void sprite::alpha_to(uint8_t alpha, uint16_t speed) {
	stop_alpha_cycle();

	target_alpha = alpha;

	if(alpha > current_alpha) {
		speed_alpha = speed;
	} else if(alpha < current_alpha) {
		speed_alpha = -speed;
	} else {
		speed_alpha = 0;
	}
}

void sprite::alpha_to_relative(uint8_t alpha, uint16_t speed) {
	alpha_to(current_alpha + alpha, speed);
}

void sprite::stop_alpha_cycle() {
	m_alpha_cycle = false;
}

void sprite::alpha_cycle(uint8_t alpha_start, uint8_t alpha_end, uint16_t speed) {
	stop_alpha();

	m_alpha_min = std::min(alpha_start, alpha_end);
	m_alpha_max = std::max(alpha_start, alpha_end);

	speed_alpha_cycle = speed;

	m_alpha_cycle = true;
}

void sprite::rotate(int16_t angle, int16_t speed) {
	stop_rotation_cycle();

	m_target_angle = angle;
	speed_rotation = speed;
}

void sprite::rotate_relative(int16_t angle, int16_t speed) {
	rotate(m_angle + angle, speed);
}

void sprite::stop_rotation() {
	speed_rotation = 0;

	m_target_angle = m_angle;
}

void sprite::rotation_cycle(int16_t speed) {
	stop_rotation();

	speed_rotation = speed;
	speed_rotation_cycle = speed;
	m_rotation_cycle = true;
}

void sprite::stop_rotation_cycle() {
	speed_rotation_cycle = 0;
	m_rotation_cycle = false;
}

void sprite::stop_all() {
	stop_movement();
	stop_alpha();
	stop_alpha_cycle();
	stop_rotation();
	stop_rotation_cycle();
}

uint16_t sprite::height() const {
	return(*surfaces_iter)->h;
}

uint16_t sprite::width() const {
	return(*surfaces_iter)->w;
}

int16_t sprite::offset_x() const {
	return gfx_object::offset_x();
}

void sprite::offset_x(int16_t offset) {
	gfx_object::offset_x(offset);

	if(surfaces_iter != surfaces[m_dir].end()) {
		m_x_min = -(*surfaces_iter)->w - offset;
		m_x_max = m_screen->w - offset;
	}
}

int16_t sprite::offset_y() const {
	return gfx_object::offset_y();
}

void sprite::offset_y(int16_t offset) {
	gfx_object::offset_y(offset);

	if(surfaces_iter != surfaces[m_dir].end()) {
		m_y_min = -(*surfaces_iter)->h - offset;
		m_y_max = m_screen->h - offset;
	}
}

void sprite::center() {
	x((config->int_value("display_width") / 2) - (width() / 2));
	y((config->int_value("display_height") / 2) - (height() / 2));
}

int16_t sprite::dir() {
	return m_dir;
}

void sprite::dir(int16_t dir) {
	m_dir = dir;
	m_anim_counter = 0;

	surfaces_iter = surfaces[m_dir].begin();
}

void sprite::animate(bool animate) {
	m_animate = animate;
}

uint16_t sprite::anim_wait() {
	return m_anim_wait;
}

void sprite::anim_wait(uint16_t anim_wait) {
	m_anim_wait = anim_wait;
}

uint8_t sprite::alpha() {
	return current_alpha;
}

void sprite::alpha(uint8_t alpha) {
	current_alpha = alpha;
}

int16_t sprite::angle() {
	return m_angle;
}

void sprite::angle(int16_t angle) {
	m_angle = angle % 360;
}

std::string sprite::text() {
	std::string return_string;

	for(
		std::vector<std::pair<std::string, SDL_Surface*> >::iterator iter = text_lines.begin();
		iter != text_lines.end();
		iter++
		) {
		return_string.append((*iter).first);
	}

	return return_string;
}

void sprite::text(const std::string &text) {
	std::string line = "";

	if(m_font == NULL)
		m_font = TTF_OpenFont(config->value("font").c_str(), config->int_value("font_size"));

	if(m_font == NULL)
		error_message("Couldn't load font: ", TTF_GetError());

	for(
		std::vector<std::pair<std::string, SDL_Surface*> >::iterator iter = text_lines.begin();
		iter != text_lines.end();
		iter++
	) {
		SDL_FreeSurface((*iter).second);
	}

	text_lines.clear();

	for(
		std::string::const_iterator iter = text.begin();
		iter != text.end();
		iter++
	) {
		if(*iter == '\n') {
			text_lines.push_back(std::pair<std::string, SDL_Surface*>(line, NULL));
			line = "";
		} else {
			line.push_back(*iter);
		}
	}

	text_lines.push_back(std::pair<std::string, SDL_Surface*>(line, NULL));

	text_surface_update = true;
}

void sprite::text(std::string text, int16_t max_width) {
	std::string new_text = "";
	std::string line = "";
	std::string old_line = "";
	std::string word = "";
	uint16_t manual_breaks = 0;
	uint16_t break_pos = 0;
	int width = 0;

	if(m_font == NULL) {
		std::string font_file = file(config->value("font"));
		m_font = TTF_OpenFont(font_file.c_str(), config->int_value("font_size"));
	}

	if(m_font == NULL)
		error_message("Couldn't load font: ", TTF_GetError());

	int16_t init_break_pos = text.find("\n");

	if(init_break_pos > 0) {
		text.replace(init_break_pos, 1, " \n"); // Otherwise some lines that should get wrapped won't be
	}

	for(
		std::string::iterator iter = text.begin();
		iter != text.end();
		iter++
	) {
		if(*iter == ' ' || *iter == '\n') {
			if(*iter == '\n') {
				manual_breaks++;
			}

			old_line = line;

			if(manual_breaks == 0) {
				line.append(word + " ");
			} else {
				line.append(word + '\n');

				manual_breaks = 0;
			}

			break_pos = line.find_last_of('\n');

			if(break_pos > line.length()) {
				break_pos = 0;
			}

			TTF_SizeUTF8(m_font, line.substr(break_pos).c_str(), &width, NULL);

			if(width > max_width) {
				new_text.append(old_line);
				new_text.push_back('\n');

				line = word + " ";

				manual_breaks = 0;
			}

			word = "";
		} else {
			word.push_back(*iter);
		}
	}

	old_line = line;
	line.append(word);

	TTF_SizeUTF8(m_font, line.substr(break_pos).c_str(), &width, NULL);

	if(width > max_width) {
		line = word;

		new_text.append(old_line);
		new_text.push_back('\n');
	}

	new_text.append(line);

	this->text(new_text);
}

void sprite::font(const std::string &font_file, uint16_t size) {
	TTF_CloseFont(m_font);
	m_font = TTF_OpenFont(font_file.c_str(), size);

	text_surface_update = true;
}

void sprite::text_color(uint8_t r, uint8_t g, uint8_t b) {
	m_text_color.r = r;
	m_text_color.g = g;
	m_text_color.b = b;

	text_surface_update = true;
}

void sprite::text_offset_x(int16_t text_offset_x) {
	m_text_offset_x = text_offset_x;

	text_surface_update = true;
}

int16_t sprite::text_offset_x() const {
	return m_text_offset_x;
}

void sprite::text_offset_y(int16_t text_offset_y) {
	m_text_offset_y = text_offset_y;

	text_surface_update = true;
}

int16_t sprite::text_offset_y() const {
	return m_text_offset_y;
}

bool sprite::coords_updated() {
	bool ret = m_coords_updated;
	m_coords_updated = false;
	return ret;
}
