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

#include "gfxobject.h"

#include <iostream>

#include "../globals.h"
#include "layer.h"

gfx_object::gfx_object() {
	m_check_bounds = true;
	m_coords_updated = false;

	m_x_min = 0;
	m_x_max = 0;
	m_y_min = 0;
	m_y_max = 0;

	m_x = 0;
	m_y = 0;

	m_target_x = 0;
	m_target_y = 0;

	m_offset_x = 0;
	m_offset_y = 0;

	m_speed_x = 0;
	m_speed_y = 0;

	m_layer_id = layer::NO_ID;
}

gfx_object::~gfx_object() {
	//dtor
}

void gfx_object::add_follower(gfx_object* sprite) {
	//std::cout << "follower" << std::endl;
	sprite->check_bounds(false);

	sprite->offset_x(sprite->x());
	sprite->offset_y(sprite->y());

	sprite->x(0);
	sprite->y(0);

	followers.push_back(sprite);
}

void gfx_object::remove_follower(gfx_object* sprite) {
	for(
		followers_queue::iterator iter = followers.begin();
		iter != followers.end();
		iter++
		) {
		if((*iter) == sprite) {
			followers.erase(iter);
			break;
		}
	}
}

uint16_t gfx_object::obstructed(player* player) const {
	uint16_t ret = 0;

	for(
		followers_queue::const_iterator iter = followers.begin();
		iter != followers.end();
		iter++
	) {
		ret |= (*iter)->obstructed(player);
	}

	return ret;
}

int16_t gfx_object::calculate_movement(int16_t coord, int16_t target, int16_t speed) const {
	if(coord != target && speed != 0) {
		int16_t tolerance = 2 * (target % speed);

		for(int16_t i = -tolerance; i <= tolerance; i++) {
			if(coord + i == target) {
				coord = target;
				break;
			}
		}

		return coord + speed;
	}

	return coord;
}

void gfx_object::calculate() {
	x(calculate_movement(m_x, m_target_x, m_speed_x));
	y(calculate_movement(m_y, m_target_y, m_speed_y));

	set_offsets();
}

void gfx_object::set_offsets() {
	for(
		followers_queue::iterator iter = followers.begin();
		iter != followers.end();
		iter++
	) {
		int16_t iter_x = m_x + m_offset_x;
		int16_t iter_y = m_y + m_offset_y;

		(*iter)->offset_x(iter_x);
		(*iter)->offset_y(iter_y);
		(*iter)->set_offsets();
	}
}

void gfx_object::check_bounds(bool check_bounds) {
	m_check_bounds = check_bounds;
}

int16_t gfx_object::x() const {
	return m_x;
}

int16_t gfx_object::y() const {
	return m_y;
}

void gfx_object::x(int16_t x) {
	m_coords_updated = true;

	if(m_check_bounds && x < m_x_min) {
		m_x = m_x_min;
	} else if(m_check_bounds && x > m_x_max) {
		m_x = m_x_max;
	} else {
		m_x = x;
	}
}

void gfx_object::y(int16_t y) {
	m_coords_updated = true;

	if(m_check_bounds && y < m_y_min) {
		m_y = m_y_min;
	} else if(m_check_bounds && y > m_y_max) {
		m_y = m_y_max;
	} else {
		m_y = y;
	}
}

int16_t gfx_object::offset_x() const {
	return m_offset_x;
}

void gfx_object::offset_x(int16_t offset) {
	//m_coords_updated = true;

	m_offset_x = offset;
}

int16_t gfx_object::offset_y() const {
	return m_offset_y;
}

void gfx_object::offset_y(int16_t offset) {
	//m_coords_updated = true;

	m_offset_y = offset;
}

int32_t gfx_object::display_x() const {
	return m_x + m_offset_x;
}

int32_t gfx_object::display_y() const {
	return m_y + m_offset_y;
}

const uint16_t& gfx_object::layer_id() const {
	return m_layer_id;
}

void gfx_object::layer_id(const uint16_t& layer_id) {
	m_coords_updated = true;

	m_layer_id = layer_id;
}
