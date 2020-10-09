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

#ifndef SCREEN_H
#define SCREEN_H

#include <iostream>
#include <string>
#include <sstream>
#include <set>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "constants.h"
#include "globals.h"
#include "gfx/player.h"
#include "gfx/mousecursor.h"
#include "gfx/map.h"
#include "gfx/draggablesprite.h"
#include "gfx/layer.h"
#include "eventqueue.h"
#include "framelimiter.h"
#include "filenotfoundexception.h"
#include "serializable.h"


class tcl_bind;

typedef std::multiset<sprite*, sprite::less> sprite_container;

class screen : public event_handler, public serializable {
private:
    event_queue* m_queue;
    surface_cache* m_cache;

    sprite_container sprites;

    SDL_Surface* screen_surface;
    SDL_Surface* background;
    SDL_Surface* temp_screen;
    SDL_Surface* zoomed_screen;
    SDL_Surface* tint_surface;

    SDL_Rect display_rect;

    frame_limiter* limiter;
    bool do_frameskip;
    int16_t frameskip;

    TTF_Font* fps_font;
    SDL_Color fps_color;
    SDL_Surface* fps_text;
    std::stringstream fps_stream;

    void push(sprite* sprite);

    template<class T>
    T* create_sprite(const std::string& file) {
    	T* new_sprite = NULL;

        try {
            new_sprite = new T(temp_screen, background, m_cache, file);
            push(new_sprite);
        } catch (file_not_found_exception e) {
            exception_message(e, FILE_NOT_FOUND_MSG);
        }

        return new_sprite;
    }

    template<class T>
    T* create_sprite() {
    	T* new_sprite = NULL;

        try {
            new_sprite = new T(temp_screen, background, m_cache);
            push(new_sprite);
        } catch (file_not_found_exception e) {
            exception_message(e, FILE_NOT_FOUND_MSG);
        }

        return new_sprite;
    }

    template<class T>
    T* create_controllable_sprite(const std::string& file) {
        T* new_sprite = create_sprite<T>(file);
        m_queue->register_handler(new_sprite);
        return new_sprite;
    }

    template<class T>
    T* create_controllable_sprite() {
        T* new_sprite = create_sprite<T>();
        m_queue->register_handler(new_sprite);
        return new_sprite;
    }
public:
    screen(event_queue* queue);
    virtual ~screen();

    void serialize(std::ostream& stream);
    void deserialize(std::istream& stream);

    void display();

    void reset_frameskip();

    void tint(uint8_t r, uint8_t g, uint8_t b, uint8_t a, int16_t rgamma, int16_t ggamma, int16_t bgamma);

    map* new_map(const std::string& file, uint16_t width, uint16_t height, tcl_bind* bind);

    layer* new_layer();

    template<class T>
    T* new_sprite(const std::string& file) {
        return create_sprite<T>(file);
    }

    template<class T>
    T* new_sprite() {
        return create_sprite<T>();
    }
};

template<>
inline mouse_cursor* screen::new_sprite<mouse_cursor>(const std::string& file) {
    return create_controllable_sprite<mouse_cursor>(file);
}

template<>
inline draggable_sprite* screen::new_sprite<draggable_sprite>(const std::string& file) {
    return create_controllable_sprite<draggable_sprite>(file);
}

template<>
inline player* screen::new_sprite<player>() {
    player* new_sprite = create_controllable_sprite<player>();

    for (
            sprite_container::iterator iter = sprites.begin();
            iter != sprites.end();
            iter++
    ) {
        map* parent = dynamic_cast<map*>(*iter);

        if (parent != NULL)
            parent->current_player(new_sprite);
    }

    return new_sprite;
}

#endif // SCREEN_H
