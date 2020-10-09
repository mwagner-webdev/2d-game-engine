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

#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <vector>

#include "events/event.h"
#include "eventhandler.h"

#include "events/controllerpressevent.h"
#include "events/controllerreleaseevent.h"

class event_queue {
private:
    std::vector<event_handler*> handlers;
public:
    template<class T>
    void handle(T* event) {
        for(
		std::vector<event_handler*>::reverse_iterator iter = handlers.rbegin(); // Iterate in reverse to get last created sprites first
		iter != handlers.rend();
		iter++
	) {
		if((*iter)->handle_abstract(event))
			break;
	}
    }

    void register_handler(event_handler* handler);
};

#endif // EVENTQUEUE_H
