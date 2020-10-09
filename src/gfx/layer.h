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

#ifndef LAYER_H
#define	LAYER_H

#include "gfxobject.h"
#include "controllablesprite.h"


class event;

class layer : public gfx_object {
public:
    static const uint16_t NO_ID = 0;

    layer();
    ~layer();

    const uint16_t& id() const;
    void id(const uint16_t& id);

    template<class T>
    void event_to_area(T* event, int16_t x, int16_t y, uint16_t w, uint16_t h) const {
        for(
		followers_queue::const_iterator iter = followers.begin();
		iter != followers.end();
		iter++
	) {
		controllable_sprite* handler = dynamic_cast<controllable_sprite*>(*iter);

		if(handler == NULL)
			return;

		int16_t handler_x = handler->display_x();
		int16_t handler_y = handler->display_y();

		if(
			handler_x + handler->width() >= x &&
			handler_y + handler->height() >= y &&
			handler_x <= x + w &&
			handler_y <= y + h
		) {
			handler->handle_abstract(event);
		}
	}
    }
private:
    uint16_t m_id;
};

#endif	/* LAYER_H */

