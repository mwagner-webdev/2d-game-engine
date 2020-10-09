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

#ifndef MAP_H
#define MAP_H

#include "controllablesprite.h"
#include "player.h"
#include "layer.h"


class tcl_bind;

class map : public controllable_sprite {
private:
    SDL_Surface* map_background;

    unsigned short tile_width, tile_height;
    uint16_t map_width, map_height;

    player* m_player;

    uint8_t x_keys_pressed, y_keys_pressed;

    bool scroll;

    int16_t old_x, old_y;

    tcl_bind* m_bind;

    int16_t follower_obstructed();
protected:
    bool handle(controller_press_event* event);
    bool handle(controller_release_event* event);

    bool handle(pointer_press_event* event);
    bool handle(pointer_release_event* event);
    bool handle(pointer_move_event* event);
public:
    map(SDL_Surface* screen, SDL_Surface* background, surface_cache* cache, tcl_bind* bind, const std::string &file, uint16_t width, uint16_t height);
    virtual ~map();

    void display();
    void calculate();

    template<class T>
    void event_to_layer_area(T* event, int16_t x, int16_t y, uint16_t w, uint16_t h) const {
	for(
		followers_queue::const_iterator iter = followers.begin();
		iter != followers.end();
		iter++
	) {
		if(layer* victim = dynamic_cast<layer*>(*iter))
			victim->event_to_area(event, x, y, w, h);
	}
    }

    void current_player(player* player);
};

#endif // MAP_H
