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

#ifndef SPRITE_H
#define SPRITE_H

#include <SDL/SDL_video.h>
#include <SDL/SDL_ttf.h>
#include <vector>
#include <map>
#include <string>
#include <assert.h>

#include "../surfacecache.h"
#include "gfxobject.h"
#include "../serializable.h"


class player;

/**
 * Class that represents a graphical sprite with directions and animations.
 * Usually objects of this type are created by the Screen object, so there is usually no need to instantiate this class directly.
 */
class sprite : public gfx_object, public serializable {
private:
    SDL_Rect prev_rect;

    bool m_obstruct;

    std::map<int, std::vector<SDL_Surface*> > surfaces;
    std::map<int, std::vector<std::string> > files;

    int16_t m_dir;
    uint16_t m_anim_counter;
    uint16_t m_anim_wait;

    uint8_t current_alpha, target_alpha, m_alpha_min, m_alpha_max;
    bool m_alpha_cycle;

    int16_t m_angle, m_target_angle;
    bool m_rotation_cycle;

    std::vector<std::pair<std::string, SDL_Surface*> > text_lines;
    TTF_Font* m_font;
    SDL_Color m_text_color;

    bool text_surface_update;

    int16_t m_text_offset_x, m_text_offset_y;

    void init(SDL_Surface* screen, SDL_Surface* background, surface_cache* cache);

    bool has_alpha();

    int16_t m_obs_offset_top, m_obs_offset_right, m_obs_offset_bottom, m_obs_offset_left;

protected:
    SDL_Surface* m_screen;
    SDL_Surface* m_background;

    surface_cache* m_cache;

    bool m_animate;

    int16_t speed_alpha, speed_alpha_cycle;
    int16_t speed_rotation, speed_rotation_cycle;

    std::vector<SDL_Surface*>::iterator surfaces_iter;

public:
    enum {
        DIR_NONE = 0x0000,

        DIR_N    = 0x0001,
        DIR_S    = 0x0002,
        DIR_W    = 0x0004,
        DIR_E    = 0x0008,

        DIR_USER = 0x0400
    };

    void serialize(std::ostream& stream);
    void deserialize(std::istream& stream);

    /**
     * Initializes a sprite with a default file facing DIR_NONE.
     *
     * @param screen
     * 	The screen surface on which the sprite is to be displayed.
     * @param background
     * 	The background surface that the sprite is alpha-blended with.
     * @param file
     * 	The image file to load and display.
     * @param parent
     * 	The parent of the sprite.
     */
    sprite(SDL_Surface* screen, SDL_Surface* background, surface_cache* cache, const std::string &file);

    /**
     * Initializes a sprite without default image file, useful for adding followers/children to.
     *
     * @param screen
     * 	The screen surface on which the sprite is to be displayed.
     * @param background
     * 	The background surface that the sprite is alpha-blended with.
     * @param parent
     * 	The parent of the sprite.
     */
    sprite(SDL_Surface* screen, SDL_Surface* background, surface_cache* cache);

    virtual ~sprite();

    /**
     * Pushes a new file to the DIR_NONE animation list.
     * @param file
     * 	The file to be added to the animation list.
     */
    void push_file(const std::string &file);

    /**
     * Pushes a new file to the animation list for a specific direction.
     * @param dir
     * 	The direction (e.g. DIR_N) for which the file should be added.
     * @param file
     * 	The file to be added to the animation list.
     */
    void push_file(int16_t dir, const std::string &file);

    void obstruct(bool obstruct, int16_t offset_top = 0, int16_t offset_right = 0, int16_t offset_bottom = 0, int16_t offset_left = 0);

    uint16_t obstructed(player* player) const;

    void calculate();

    /**
     * Displays the sprite at its current position.
     */
    virtual void display();

    /**
     * Displays the sprite at a given position.
     *
     * @param x
     * 	The X coordinate in pixels.
     * @param y
     * 	The Y coordinate in pixels.
     */
    void display(int16_t x, int16_t y);

    /**
     * Displays the sprite at a given position with a given alpha value.
     *
     * @param x
     * 	The X coordinate in pixels.
     * @param y
     * 	The Y coordinate in pixels.
     * @param alpha
     * 	The alpha value from 0 (transparent) to 255 (opaque)
     */
    void display(int16_t x, int16_t y, uint8_t alpha);

    /**
     * Stops all movement of the sprite in X direction.
     */
    void stop_movement_x();

    /**
     * Stops all movement of the sprite in Y direction.
     */
    void stop_movement_y();

    /**
     * Stops all movement of the sprite.
     */
    void stop_movement();

    /**
     * Moves the sprite to an absolute position with a given speed.
     *
     * @param x
     * 	The X coordinate in pixels.
     * @param y
     * 	The Y coordinate in pixels.
     * @param speed
     * 	The speed in pixels per frame.
     */
    void move(int16_t x, int16_t y, uint16_t speed);

    /**
     * Moves the sprite to a position relative to its current one, with a given speed.
     *
     * @param x
     * 	The relative X coordinate in pixels.
     * @param y
     * 	The relative Y coordinate in pixels.
     * @param speed
     * 	The speed in pixels per frame.
     */
    void move_relative(int16_t x, int16_t y, uint16_t speed);

    /**
     * Stops all alpha fading initiated by alpha_to.
     */
    void stop_alpha();

    /**
     * Fades the sprite's alpha to a specific value with a given speed.
     *
     * @param alpha
     * 	The alpha value from 0 (transparent) to 255 (opaque).
     * @param speed
     * 	The speed in alpha values per frame.
     */
    void alpha_to(uint8_t alpha, uint16_t speed);

    /**
     * Fades the sprite's alpha to a relative value with a given speed.
     *
     * @param alpha
     * 	The alpha value from 0 (transparent) to 255 (opaque). This will be capped if the sum of this and current alpha is less than 0 or greater than 255.
     * @param speed
     * 	The speed in alpha values per frame.
     */
    void alpha_to_relative(uint8_t alpha, uint16_t speed);

    /**
     * Stops all alpha cycling initiated by alpha_cycle.
     */
    void stop_alpha_cycle();

    /**
     * Cycles the sprite's alpha between specific values with a given speed.
     *
     * @param alpha_start
     * 	A start alpha value from 0 (transparent) to 255 (opaque).
     * @param alpha_end
     * 	The alpha value at which the fading reverses.
     * @param speed
     * 	The speed in alpha values per frame.
     */
    void alpha_cycle(uint8_t alpha_start, uint8_t alpha_end, uint16_t speed);

    /**
     * Stops rotation of the sprite. The angle is kept.
     */
    void stop_rotation();

    /**
     * Rotates the sprite to the given angle.
     *
     * @param angle
     * 	The angle in degrees to rotate to.
     * @param speed
     *	The speed in degrees per frame.
     */
    void rotate(int16_t angle, int16_t speed);

    /**
     * Rotates the sprite by the given relative angle.
     *
     * @param angle
     * 	The angle in degrees to rotate.
     * @param speed
     *	The speed in degrees per frame.
     */
    void rotate_relative(int16_t angle, int16_t speed);

    /**
     * Stops a rotation cycle of the sprite. The angle is kept.
     */
    void stop_rotation_cycle();

    /**
     * Rotates the sprite continuously with the given speed.
     *
     * @param speed
     *	The speed in degrees per frame.
     */
    void rotation_cycle(int16_t speed);

    /**
     * Stops all movement/alpha blending/rotation.
     */
    void stop_all();

    int16_t dir();
    void dir(int16_t dir);

    void animate(bool animate);

    uint16_t anim_wait();
    void anim_wait(uint16_t anim_wait);

    uint16_t width() const;
    uint16_t height() const;

    void center();

    int16_t offset_x() const;
    void offset_x(int16_t offset);
    int16_t offset_y() const;
    void offset_y(int16_t offset);

    uint8_t alpha();
    void alpha(uint8_t alpha);
    int16_t angle();
    void angle(int16_t angle);

    std::string text();
    void text(const std::string &text);
    void text(std::string text, int16_t max_width);
    void font(const std::string &font_file, uint16_t size);
    void text_color(uint8_t r, uint8_t g, uint8_t b);

    void text_offset_x(int16_t text_offset_x);
    int16_t text_offset_x() const;

    void text_offset_y(int16_t text_offset_y);
    int16_t text_offset_y() const;

    bool coords_updated();

    struct less {
	inline bool operator()(sprite* lhs, sprite* rhs) {
	    if(lhs->surfaces_iter == lhs->surfaces[0].end() || rhs->surfaces_iter == rhs->surfaces[0].end())
		return true;

	    uint16_t llayer = lhs->layer_id();
	    uint16_t rlayer = rhs->layer_id();

            return(
		llayer < rlayer ||
                (llayer == rlayer &&
                lhs->display_y() + lhs->height() < rhs->display_y() + rhs->height())
            );
	}
    };
};

#endif // SPRITE_H
