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

#include "framelimiter.h"

#include <stdexcept>
#include <iostream>
#include <math.h>
#include <SDL/SDL_timer.h>

#include "constants.h"

frame_limiter::frame_limiter(int16_t fps_limit) {
	if(fps_limit <= 0)
		throw std::range_error("fps_limit must be > 0!");

	m_fps_limit = fps_limit;
	m_tick_limit = (1.0 / fps_limit) * 1000 * FPS_INIT_FACTOR;

	m_fps = 0;
	next_second_tick = SDL_GetTicks() + 1000;
	next_frame_tick = SDL_GetTicks() + m_tick_limit;

	current_fps = HARD_FPS_LIMIT;
	m_new_fps = true;
}

void frame_limiter::sleep_till_next() {
	current_fps++;

	if(m_fps > m_fps_limit) {
		if(m_fps - m_fps_limit > 5) {
			m_tick_limit += .0025;
		} else {
			m_tick_limit += .00005;
		}
	} else if(m_fps < m_fps_limit) {
		if(m_fps_limit - m_fps > 5) {
			m_tick_limit -= .001;
		} else {
			m_tick_limit -= .000025;
		}
	}

	if(SDL_GetTicks() < next_frame_tick) {
		SDL_Delay(m_tick_limit / 2);
	}

	if(SDL_GetTicks() > next_second_tick) {
		m_new_fps = true;
		m_fps = current_fps;
		current_fps = 0;

		next_second_tick = SDL_GetTicks() + 1000;
	}

	next_frame_tick = SDL_GetTicks() + m_tick_limit;
}

uint16_t frame_limiter::fps() {
	m_new_fps = false;
	return m_fps;
}

uint16_t frame_limiter::fps_limit() {
	return m_fps_limit;
}

bool frame_limiter::new_fps() {
	return m_new_fps;
}
