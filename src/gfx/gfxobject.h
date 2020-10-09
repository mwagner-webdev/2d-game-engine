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

#ifndef GFXOBJECT_H
#define GFXOBJECT_H

#include <vector>
#include <stdint.h>

class gfx_object;
class player;

typedef std::vector<gfx_object*> followers_queue;

class gfx_object {
public:
    gfx_object();
    virtual ~gfx_object();

    void add_follower(gfx_object* object);
    void remove_follower(gfx_object* object);

    virtual uint16_t obstructed(player* player) const;

    virtual void calculate();

    void check_bounds(bool check_bounds);

    int16_t x() const;
    void x(int16_t x);
    int16_t y() const;
    void y(int16_t y);

    virtual int16_t offset_x() const;
    virtual void offset_x(int16_t offset);
    virtual int16_t offset_y() const;
    virtual void offset_y(int16_t offset);

    int32_t display_x() const;
    int32_t display_y() const;

    const uint16_t& layer_id() const;
    void layer_id(const uint16_t& layer);
protected:
    followers_queue followers;

    int16_t m_x, m_y, m_target_x, m_target_y;
    int16_t m_speed_x, m_speed_y;
    int16_t m_x_min, m_x_max, m_y_min, m_y_max;
    int16_t m_offset_x, m_offset_y;

    void set_offsets();

    bool m_coords_updated;
private:
    bool m_check_bounds;

    uint16_t m_layer_id;

    int16_t calculate_movement(int16_t coord, int16_t target, int16_t speed) const;
};

#endif // GFXOBJECT_H
