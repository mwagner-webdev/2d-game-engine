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

#include "tclbind.h"

#include <iostream>
#include <map>
#include <stdio.h>

#include "globals.h"
#include "constants.h"
#include "file.h"

#include "gfx/sprite.h"
#include "gfx/map.h"


tcl_bind* bind;

typedef std::map<int, gfx_object*> handle_container;

handle_container handles;

int insert_handle(gfx_object* object, gfx_object* potential_layer = NULL) {
	long handle = handles.size() + 1;

	handles.insert(std::make_pair(handle, object));

	if(potential_layer != NULL) {
		if(layer* parent = dynamic_cast<layer*>(potential_layer)) {
			object->layer_id(parent->id());
		}
	}

	return handle;
}

template<class T>
void push_files(T* sprite, Tcl_Interp* interp, int objc, Tcl_Obj* const* objv) {
	Tcl_Obj** file;
	int file_count;

	char* file_string;
	int dir;

	for(int i = 1; i < objc - 1; i++) {
		switch(i) {
		case 1:
			dir = sprite::DIR_N;
			break;
		case 2:
			dir = sprite::DIR_S;
			break;
		case 3:
			dir = sprite::DIR_W;
			break;
		case 4:
			dir = sprite::DIR_E;
			break;
		}

		Tcl_ListObjGetElements(interp, objv[i], &file_count, &file);

		for(int j = 0; j < file_count; j++) {
			file_string = Tcl_GetStringFromObj(*file, NULL);
			//std::cout << file_string << std::endl;
			try {
				sprite->push_file(dir, file_string);
			} catch(file_not_found_exception e) {
				exception_message(e, FILE_NOT_FOUND_MSG);
			}

			file++;
		}
	}
}

template<class T>
int generic_sprite(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj* const* objv) {
	if(objc != 2 && objc != 3 && objc != 5 && objc != 6)
		return TCL_ERROR;

	controllable_sprite* new_sprite;

	if(objc == 2 || objc == 3) {
		new_sprite = bind->m_screen->new_sprite<T>(Tcl_GetStringFromObj(objv[1], NULL));
	} else {
		new_sprite = bind->m_screen->new_sprite<controllable_sprite>();

		push_files(new_sprite, interp, objc, objv);
	}

	if(objc == 3 || objc == 6) {
		long handle;
		Tcl_GetLongFromObj(interp, objv[objc - 1], &handle);

		handle_container::iterator result = handles.find(handle);

		if(result != handles.end()) {
			(*result).second->add_follower(new_sprite);
		} else {
			return TCL_ERROR;
		}

		Tcl_SetObjResult(interp, Tcl_NewIntObj(insert_handle(new_sprite, (*result).second)));
	} else {
		Tcl_SetObjResult(interp, Tcl_NewIntObj(insert_handle(new_sprite)));
	}

	return TCL_OK;
}

template<class T>
T* find_handle(Tcl_Interp* interp, Tcl_Obj* obj) {
	long handle;
	Tcl_GetLongFromObj(interp, obj, &handle);

	handle_container::iterator result = handles.find(handle);

	T* sprite = dynamic_cast<T*>((*result).second);

	if(sprite == NULL || result == handles.end()) {
		throw std::runtime_error("Handle not found!");
	}

	return sprite;
}

template<class P, class F>
bool add_follower(F* follower, Tcl_Interp* interp, Tcl_Obj* obj) {
	long handle;

	Tcl_GetLongFromObj(interp, obj, &handle);

	handle_container::iterator result = handles.find(handle);

	P* parent = dynamic_cast<P*>((*result).second);

	if(parent != NULL && result != handles.end()) {
		parent->add_follower(follower);
	} else {
		return false;
	}

	return true;
}

int tcl_tint(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj* const* objv) {
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t a = 128;
	int gr = 255;
	int gg = 255;
	int gb = 255;

	std::vector<int> arg_ints;
	int temp_int;

	// Convert everything to int

	for(int i = 0; i < objc; i++) {
		Tcl_GetIntFromObj(interp, objv[i], &temp_int);
		arg_ints.push_back(temp_int);
	}

	// Error handling

	switch(objc) {
	case 8:
		if(arg_ints[6] < 0 || arg_ints[7] < 0 || arg_ints[7] < 0)
			return TCL_ERROR;
	case 5:
		if(arg_ints[4] > 255 || arg_ints[4] > 255)
			return TCL_ERROR;
	case 3:
		if(arg_ints[1] > 255 || arg_ints[2] > 255 || arg_ints[3] > 255 || arg_ints[1] < 0 || arg_ints[2] < 0 || arg_ints[3] < 0)
			return TCL_ERROR;
		break;
	default:
		return TCL_ERROR;
	}

	// Set values

	switch(objc) {
	case 8:
		gr = arg_ints[5];
		gg = arg_ints[6];
		gb = arg_ints[7];
	case 5:
		a = arg_ints[4];
	case 4:
		r = arg_ints[1];
		g = arg_ints[2];
		b = arg_ints[3];
		break;
	}

	bind->m_screen->tint(r, g, b, a, gr, gg, gb);
	return TCL_OK;
}

int tcl_sprite(ClientData dummy, Tcl_Interp* interp, int objc, Tcl_Obj* const* objv) {
	return generic_sprite<controllable_sprite>(dummy, interp, objc, objv);
}

int tcl_dragsprite(ClientData dummy, Tcl_Interp* interp, int objc, Tcl_Obj* const* objv) {
	return generic_sprite<draggable_sprite>(dummy, interp, objc, objv);
}

int tcl_layer(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj* const* objv) {
	if(objc != 1 && objc != 2)
		return TCL_ERROR;

	layer* layer = bind->m_screen->new_layer();

	if(objc == 2) {
		if(!add_follower<map>(layer, interp, objv[1]))
			return TCL_ERROR;
	}

	long handle = insert_handle(layer);

	layer->id(handle);
	Tcl_SetObjResult(interp, Tcl_NewIntObj(handle));

	return TCL_OK;
}

int tcl_map(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 4)
		return TCL_ERROR;

	int w;
	int h;

	Tcl_GetIntFromObj(interp, objv[2], &w);
	Tcl_GetIntFromObj(interp, objv[3], &h);

	Tcl_SetObjResult(interp, Tcl_NewIntObj(insert_handle(bind->m_screen->new_map(Tcl_GetStringFromObj(objv[1], NULL), w, h, bind))));

	return TCL_OK;
}

int tcl_follow(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 3)
		return TCL_ERROR;

	long handle1, handle2;

	Tcl_GetLongFromObj(interp, objv[1], &handle1);
	Tcl_GetLongFromObj(interp, objv[2], &handle2);

	handle_container::iterator result1 = handles.find(handle1);
	handle_container::iterator result2 = handles.find(handle2);

	sprite* sprite2 = dynamic_cast<sprite*>((*result2).second);

	if(sprite2 != NULL && result1 != handles.end() && result2 != handles.end()) {
		sprite2->add_follower((*result1).second);
	} else {
		return TCL_ERROR;
	}

	return TCL_OK;
}

int tcl_player(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 6)
		return TCL_ERROR;

	player* sprite = bind->m_screen->new_sprite<player>();

	push_files(sprite, interp, objc, objv);

	long layer_handle;
	gfx_object* potential_layer;

	Tcl_GetLongFromObj(interp, objv[5], &layer_handle);

	handle_container::iterator result = handles.find(layer_handle);

	if(result != handles.end()) {
		potential_layer = (*result).second;
	} else {
		return TCL_ERROR;
	}

	Tcl_SetObjResult(interp, Tcl_NewIntObj(insert_handle(sprite, potential_layer)));

	return TCL_OK;
}

int tcl_obstruct(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 2 && objc != 6)
		return TCL_ERROR;

	long handle;
	Tcl_GetLongFromObj(interp, objv[1], &handle);

	handle_container::iterator result = handles.find(handle);

	sprite* victim = dynamic_cast<sprite*>((*result).second);

	if(victim != NULL && result != handles.end()) {
		if(objc == 6) {
			int* dirs = new int[4];

			for(int i = 0; i < 4; i++) {
				Tcl_GetIntFromObj(interp, objv[i + 2], &dirs[i]);
			}

			victim->obstruct(true, dirs[0], dirs[1], dirs[2], dirs[3]);

			delete[] dirs;
		} else {
			victim->obstruct(true);
		}
	} else {
		return TCL_ERROR;
	}

	return TCL_OK;
}

int tcl_animate(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 2)
		return TCL_ERROR;

	long handle;
	Tcl_GetLongFromObj(interp, objv[1], &handle);

	handle_container::iterator result = handles.find(handle);

	sprite* victim = dynamic_cast<sprite*>((*result).second);

	if(victim != NULL && result != handles.end()) {
		victim->animate(true);
	} else {
		return TCL_ERROR;
	}

	return TCL_OK;
}

int tcl_x(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 3)
		return TCL_ERROR;

	long handle;
	Tcl_GetLongFromObj(interp, objv[1], &handle);

	int x;
	Tcl_GetIntFromObj(interp, objv[2], &x);

	handle_container::iterator result = handles.find(handle);

	if(result != handles.end()) {
		(*result).second->x(x);
	} else {
		return TCL_ERROR;
	}

	return TCL_OK;
}

int tcl_y(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 3)
		return TCL_ERROR;

	long handle;
	Tcl_GetLongFromObj(interp, objv[1], &handle);

	int y;
	Tcl_GetIntFromObj(interp, objv[2], &y);

	handle_container::iterator result = handles.find(handle);

	if(result != handles.end()) {
		(*result).second->y(y);
	} else {
		return TCL_ERROR;
	}

	return TCL_OK;
}

int tcl_alpha(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 3)
		return TCL_ERROR;

	long handle;
	Tcl_GetLongFromObj(interp, objv[1], &handle);

	int alpha;
	Tcl_GetIntFromObj(interp, objv[2], &alpha);

	if(alpha < 0 || alpha > 255)
		return TCL_ERROR;

	handle_container::iterator result = handles.find(handle);

	sprite* victim = dynamic_cast<sprite*>((*result).second);

	if(victim != NULL && result != handles.end()) {
		victim->alpha(alpha);
	} else {
		return TCL_ERROR;
	}

	return TCL_OK;
}

int tcl_move(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 5)
		return TCL_ERROR;

	long handle;
	Tcl_GetLongFromObj(interp, objv[1], &handle);

	int x;
	Tcl_GetIntFromObj(interp, objv[2], &x);

	int y;
	Tcl_GetIntFromObj(interp, objv[3], &y);

	int speed;
	Tcl_GetIntFromObj(interp, objv[4], &speed);

	if(speed < 0)
		return TCL_ERROR;

	handle_container::iterator result = handles.find(handle);

	sprite* victim = dynamic_cast<sprite*>((*result).second);

	if(victim != NULL && result != handles.end()) {
		victim->move(x, y, speed);
	} else {
		return TCL_ERROR;
	}

	return TCL_OK;
}

int tcl_fade(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 4)
		return TCL_ERROR;

	long handle;
	Tcl_GetLongFromObj(interp, objv[1], &handle);

	int target;
	Tcl_GetIntFromObj(interp, objv[2], &target);

	int speed;
	Tcl_GetIntFromObj(interp, objv[3], &speed);

	if(speed < 0)
		return TCL_ERROR;

	handle_container::iterator result = handles.find(handle);

	sprite* victim = dynamic_cast<sprite*>((*result).second);

	if(victim != NULL && result != handles.end()) {
		victim->alpha_to(target, speed);
	} else {
		return TCL_ERROR;
	}

	return TCL_OK;
}

int tcl_angle(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 3)
		return TCL_ERROR;

	int angle;
	Tcl_GetIntFromObj(interp, objv[2], &angle);

	find_handle<sprite>(interp, objv[1])->angle(angle);

	return TCL_OK;
}

int tcl_rotate(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 4)
		return TCL_ERROR;

	int angle;
	Tcl_GetIntFromObj(interp, objv[2], &angle);

	int speed;
	Tcl_GetIntFromObj(interp, objv[3], &speed);

	find_handle<sprite>(interp, objv[1])->rotate(angle, speed);

	return TCL_OK;
}

int tcl_rotate_cycle(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 3)
		return TCL_ERROR;

	int speed;
	Tcl_GetIntFromObj(interp, objv[2], &speed);

	find_handle<sprite>(interp, objv[1])->rotation_cycle(speed);

	return TCL_OK;
}

int tcl_tassenhalter(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	SDL_InitSubSystem(SDL_INIT_CDROM);

	SDL_CD* cd = SDL_CDOpen(0);
	SDL_CDEject(cd);
	SDL_CDClose(cd);

	return TCL_OK;
}

int tcl_text(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 3 && objc != 5)
		return TCL_ERROR;

	long handle;
	Tcl_GetLongFromObj(interp, objv[1], &handle);

	std::string text = Tcl_GetStringFromObj(objv[2], NULL);

	handle_container::iterator result = handles.find(handle);

	sprite* victim = dynamic_cast<sprite*>((*result).second);

	if(victim != NULL && result != handles.end()) {
		if(objc == 5) {
			int offset_x, offset_y;
			Tcl_GetIntFromObj(interp, objv[3], &offset_x);
			Tcl_GetIntFromObj(interp, objv[4], &offset_y);

			victim->text_offset_x(offset_x);
			victim->text_offset_y(offset_y);
			victim->text(text, victim->width() - (2 * offset_x));
		} else {
			victim->text(text, victim->width());
		}
	} else {
		//interp->result = (char*)"Couldn't find sprite in text procedure.";
		return TCL_ERROR;
	}

	return TCL_OK;
}

int tcl_sound(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 2)
		return TCL_ERROR;

	std::string file = Tcl_GetStringFromObj(objv[1], NULL);

	bind->m_player->play_audio(file);

	return TCL_OK;
}

int tcl_music(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 2)
		return TCL_ERROR;

	std::string file = Tcl_GetStringFromObj(objv[1], NULL);

	bind->m_player->play_audio_loop(file);

	return TCL_OK;
}

int tcl_on(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 3 && objc != 4)
		return TCL_ERROR;

	if(objc == 3) {
		char* event = Tcl_GetStringFromObj(objv[1], NULL);
		char* code = Tcl_GetStringFromObj(objv[2], NULL);

		if(
			strcmp(event, "contpress") == 0 ||
			strcmp(event, "contrelease") == 0 ||
			strcmp(event, "pointpress") == 0 ||
			strcmp(event, "pointrelease") == 0 ||
			strcmp(event, "pointmove") == 0 ||
			strcmp(event, "frame") == 0
			) {
			bind->add_event_code(event, code);
		} else {
			return TCL_ERROR;
		}
	} else if(objc == 4) {
		long sprite_handle;
		Tcl_GetLongFromObj(interp, objv[1], &sprite_handle);
		handle_container::iterator result = handles.find(sprite_handle);
		event_handler* handler = dynamic_cast<event_handler*>((*result).second);

		if(handler == NULL || result == handles.end()) {
			return TCL_ERROR;
		}

		char* event = Tcl_GetStringFromObj(objv[2], NULL);
		char* code = Tcl_GetStringFromObj(objv[3], NULL);

		if(
			strcmp(event, "activate") == 0
		) {
			bind->add_handler_code(handler, event, code);
		} else {
			return TCL_ERROR;
		}
	}

	return TCL_OK;
}

int tcl_unbind(ClientData, Tcl_Interp* interp, int objc, Tcl_Obj * const* objv) {
	if(objc != 2)
		return TCL_ERROR;

	try {
		bind->remove_event(Tcl_GetStringFromObj(objv[1], NULL));
	} catch(std::runtime_error e) {
		return TCL_ERROR;
	}

	return TCL_OK;
}

tcl_bind::tcl_bind(Tcl_Interp* interp, screen* screen, audio_player* player, event_queue* queue) {
	m_interp = interp;
	m_screen = screen;
	m_player = player;
	m_queue = queue;
}

void tcl_bind::init_namespace() {
	if(
		Tcl_Eval(m_interp, "\
			namespace eval ::faw::core {\
			namespace export path tint sprite dragsprite layer map player follow obstruct animate x y alpha move fade angle rotate rotate_cycle tassenhalter text sound music on unbind}\
			")
		!= TCL_OK
	) {
		error_message("Could not initialize Tcl (could not create namespace).");
	}
}

void tcl_bind::bind_all() {
	bind = this;

	Tcl_SetVar(m_interp, "::faw::path", file::path().c_str(), 0);

	Tcl_CreateObjCommand(m_interp, "::faw::core::tint", tcl_tint, NULL, NULL);

	Tcl_CreateObjCommand(m_interp, "::faw::core::sprite", tcl_sprite, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::dragsprite", tcl_dragsprite, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::map", tcl_map, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::layer", tcl_layer, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::player", tcl_player, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::follow", tcl_follow, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::obstruct", tcl_obstruct, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::animate", tcl_animate, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::x", tcl_x, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::y", tcl_y, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::move", tcl_move, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::alpha", tcl_alpha, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::fade", tcl_fade, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::angle", tcl_angle, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::rotate", tcl_rotate, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::rotate_cycle", tcl_rotate_cycle, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::tassenhalter", tcl_tassenhalter, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::text", tcl_text, NULL, NULL);

	Tcl_CreateObjCommand(m_interp, "::faw::core::sound", tcl_sound, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::music", tcl_music, NULL, NULL);

	Tcl_CreateObjCommand(m_interp, "::faw::core::on", tcl_on, NULL, NULL);
	Tcl_CreateObjCommand(m_interp, "::faw::core::unbind", tcl_unbind, NULL, NULL);
}

void tcl_bind::add_event_code(const std::string& type, const std::string& code) {
	// Prevent strange bugs/unusabilities when only one type of a press/release pair was used

	if(type == "contpress") {
		event_codes["contrelease"] = event_codes["contrelease"]; // This creates an empty event code if none was there
	} else if(type == "contrelease") {
		event_codes["contpress"] = event_codes["contpress"];
	} else if(type == "pointpress") {
		event_codes["pointrelease"] = event_codes["pointrelease"];
	} else if(type == "pointrelease") {
		event_codes["pointpress"] = event_codes["pointpress"];
	}

	// The setting itself
	event_codes[type] = code;
}

void tcl_bind::add_handler_code(event_handler* handler, const std::string& type, const std::string& code) {
	handler_codes[handler].insert(std::make_pair(type, code));

	handler->active(true);
	handler->bind(this);
}

void tcl_bind::call_handler_code(const event_handler* handler, const std::string &type) const {
	handler_map::const_iterator result = handler_codes.find(handler);

	if(result == handler_codes.end())
		return;

	type_map types = (*result).second;
	type_map::const_iterator type_result = types.find(type);

	if(type_result == types.end())
		return;

	if(Tcl_CommandComplete((*type_result).second.c_str()) == 0)
		error_message("Syntax error in handler code for ", type);

	if(Tcl_Eval(m_interp, (*type_result).second.c_str()) != TCL_OK)
		error_message("Runtime error in handler code:\n\n", Tcl_GetVar(m_interp, "errorInfo", TCL_GLOBAL_ONLY));
}

bool tcl_bind::call_event_code(const std::string &type, const std::string& var) {
	type_map::iterator result = event_codes.find(type);

	if(result == event_codes.end())
		return false;

	Tcl_SetVar(m_interp, type.c_str(), var.c_str(), 0);

	if(Tcl_CommandComplete((*result).second.c_str()) == 0)
		error_message("Syntax error in event code for ", type);

	if(Tcl_Eval(m_interp, (*result).second.c_str()) != TCL_OK)
		error_message("Runtime error in event code:\n\n", Tcl_GetVar(m_interp, "errorInfo", TCL_GLOBAL_ONLY));

	return true;
}

void tcl_bind::add_wait(const std::string &type, const std::string& var) {
	waits[type] = var;
}

void tcl_bind::remove_event(const std::string& type) {
	type_map::iterator result = event_codes.find(type);

	if(result != waits.end()) {
		if(type == "contpress") {
			event_codes.erase("contrelease");
		} else if(type == "contrelease") {
			event_codes.erase("contpress");
		} else if(type == "pointpress") {
			event_codes.erase("pointrelease");
		} else if(type == "pointrelease") {
			event_codes.erase("pointpress");
		}

		event_codes.erase(result);
	} else {
		throw std::runtime_error("No temporary event code found.");
	}
}
