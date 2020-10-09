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

#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "events/controllerpressevent.h"
#include "events/controllerreleaseevent.h"
#include "events/pointerpressevent.h"
#include "events/pointerreleaseevent.h"
#include "events/pointermoveevent.h"
#include "events/activateevent.h"


class tcl_bind;

class event_handler {
private:
    bool m_active;

    tcl_bind* m_bind;
protected:
    virtual bool handle(pointer_press_event* event);
    virtual bool handle(pointer_release_event* event);
    virtual bool handle(pointer_move_event* event);

    virtual bool handle(controller_press_event* event);
    virtual bool handle(controller_release_event* event);

    virtual bool handle(activate_event* event);

	virtual ~event_handler() {}
public:
    static const bool END = true;
    static const bool PASS = false;

    template<class E>
    bool handle_abstract(E event) {
        if(m_active) {
		event->call_code(this);

                return handle(static_cast<E>(event));
	}

	return PASS;
    }

    void active(bool value);

    void bind(tcl_bind* tcl_bind);
};

#endif // EVENTHANDLER_H
