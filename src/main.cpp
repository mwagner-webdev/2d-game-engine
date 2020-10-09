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

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <limits.h>

#ifdef WIN32
#        include <windows.h>
#endif

#include <SDL/SDL.h>
#include <tcl.h>
#include <zlib.h>

#include "constants.h"
#include "globals.h"

#include "tclbind.h"
#include "screen.h"
#include "eventqueue.h"
#include "gfx/mousecursor.h"
#include "gfx/player.h"
#include "eventhandler.h"
#include "events/controllerpressevent.h"
#include "events/controllerreleaseevent.h"
#include "events/pointerevent.h"
#include "audioplayer.h"
#include "configfile.h"
#include "serializable.h"
#include "gfx/draggablesprite.h"
#include "gfx/splash.h"
#include "file.h"


config_file* config;

int16_t faked_key_presses;
int16_t faked_key_releases;

void message(std::string msg, bool error = false) {
	if(error) {
#ifdef WIN32
		std::string title;

		if(config == NULL || config->value("window_title") == "") {
			title = "Error";
		} else {
			title = config->value("window_title");
		}

		MessageBox(NULL, msg.c_str(), title.c_str(), MB_ICONERROR | MB_OK);
#endif

		std::cerr << msg << std::endl;
		exit(EXIT_FAILURE);
	} else {
		std::cout << msg << std::endl;
	}
}

void error_message(std::string msg, std::string amendment) {
	message(msg + amendment, true);
}

void exception_message(std::runtime_error e, std::string amendment) {
	error_message(e.what(), amendment);
}

void load_game(tcl_bind* bind, const std::string& file) {
	std::ifstream stream;
	stream.open(file.c_str(), std::fstream::binary);

	//stream << (char)0x_fA << (char)0x3E << (char)0x50 << (char)0x3E; // File token
	//stream << "V1"; // Format version
	char* header = new char[9];

	stream.read(header, 9);

	// blah

	stream.close();
}

void save_game(tcl_bind* bind, const std::string& file, serializable* master) {
	z_stream zstream;
	memset(&zstream, 0, sizeof(zstream));

	std::stringstream stream;

	master->serialize(stream);

	//std::cout << stream.str();

	size_t size = stream.str().size();
	char* out = new char[size];

	zstream.next_in = (Bytef*)stream.str().data();
	zstream.avail_in = size;
	zstream.next_out = (Bytef*)out;
	zstream.avail_out = size;

	if(deflateInit(&zstream, Z_DEFAULT_COMPRESSION) != Z_OK)
		message("Problem while initializing deflating savegame!", true);

	if(deflate(&zstream, Z_FINISH) != Z_STREAM_END)
		message("Problem while deflating savegame!", true);

	std::ofstream ostream;
	ostream.open(file.c_str(), std::fstream::binary);

	ostream << (char)0xFA << (char)0x3E << (char)0x50 << (char)0x3E; // File token
	ostream << "V0001"; // Format version

	ostream.write(out, zstream.total_out);
	//ostream << stream.str();

	ostream.flush();
	ostream.close();

	deflateEnd(&zstream);
}

void handle_sdl_event(const SDL_Event& sdl_event, event_queue* queue, tcl_bind* bind) {
	controller_press_event* contpress;
	controller_release_event* contrelease;
	pointer_press_event* pointpress;
	pointer_release_event* pointrelease;
	pointer_move_event* pointmove;

	SDL_Event fake_event;
	SDL_Event first_fake;
	SDL_Event second_fake;

	switch(sdl_event.type) {
	case SDL_JOYAXISMOTION:
		// We're faking keypresses here, cleanest solution I could think of XD

		if(sdl_event.jaxis.value < -16383 || sdl_event.jaxis.value > 16383) {
			switch(sdl_event.jaxis.axis) {
			case 0:
				fake_event.key.keysym.sym = sdl_event.jaxis.value < 0 ? SDLK_LEFT : SDLK_RIGHT;
				break;
			case 1:
				fake_event.key.keysym.sym = sdl_event.jaxis.value < 0 ? SDLK_UP : SDLK_DOWN;
				break;
			default:
				fake_event.key.keysym.sym = (SDLKey)0;
			}

			second_fake = first_fake = fake_event;

			if(faked_key_presses > 0) {
				first_fake.type = SDL_KEYUP;
				faked_key_releases++;
				faked_key_presses--;
			}

			if(faked_key_presses <= faked_key_releases) {
				second_fake.type = SDL_KEYDOWN;
				faked_key_presses++;
				faked_key_releases--;
			}
		} else {
			switch(sdl_event.jaxis.axis) {
			case 0:
				fake_event.key.keysym.sym = sdl_event.jaxis.value >= 0 ? SDLK_LEFT : SDLK_RIGHT;
				break;
			case 1:
				fake_event.key.keysym.sym = sdl_event.jaxis.value >= 0 ? SDLK_UP : SDLK_DOWN;
				break;
			default:
				fake_event.key.keysym.sym = (SDLKey)0;
			}

			second_fake = first_fake = fake_event;

			if(faked_key_presses <= faked_key_releases) {
				second_fake.type = SDL_KEYDOWN;
				faked_key_presses++;
				faked_key_releases--;
			}

			if(faked_key_presses > 0) {
				first_fake.type = SDL_KEYUP;
				faked_key_releases++;
				faked_key_presses--;
			}

		}

		handle_sdl_event(first_fake, queue, bind);
		handle_sdl_event(second_fake, queue, bind);

		break;
	case SDL_KEYDOWN:
	case SDL_JOYBUTTONDOWN:
		contpress = new controller_press_event(sdl_event, bind);

		if(bind->call_event_code("contpress", contpress->var()) == false)
			queue->handle(contpress);
		break;
	case SDL_KEYUP:
	case SDL_JOYBUTTONUP:
		contrelease = new controller_release_event(sdl_event, bind);

		if(bind->call_event_code("contrelease", contrelease->var()) == false)
			queue->handle(contrelease);
		break;
	case SDL_MOUSEBUTTONDOWN:
		pointpress = new pointer_press_event(sdl_event, bind);

		if(bind->call_event_code("pointpress", pointpress->var()) == false)
			queue->handle(pointpress);
		break;
	case SDL_MOUSEBUTTONUP:
		pointrelease = new pointer_release_event(sdl_event, bind);

		if(bind->call_event_code("pointrelease", pointrelease->var()) == false)
			queue->handle(pointrelease);
		break;
	case SDL_MOUSEMOTION:
		pointmove = new pointer_move_event(sdl_event, bind);

		if(bind->call_event_code("pointmove", pointmove->var()) == false)
			queue->handle(pointmove);
		break;
	}
}

int main(int argc, char* argv[]) {
	#ifdef WIN32
		FreeConsole(); // We can't just link with -Wl,-subsystem,windows or something like that because that breaks Tcl
	#endif

	faked_key_presses = 0;
	faked_key_releases = 0;

	if(argc == 2) {
		file::path(argv[1]);
	} else if(argc > 2) {
		error_message("Too many arguments!");
	}

	message("Reading configuration files...");

	try {
		config = new config_file("engine.cfg");
	} catch(std::runtime_error e) {
		exception_message(e, "\nIf you don't know what caused this, delete the file to revert back to default settings.");
	}

	message("Success!");


	message("Initializing SDL...");

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
		error_message("Could not initialize SDL: ", SDL_GetError());
		SDL_Quit();

	} else {
		message("Success!");
	}

	SDL_JoystickOpen(config->int_value("joystick"));

	SDL_WM_SetCaption(config->value("window_title").c_str(), "");

	SDL_ShowCursor(SDL_DISABLE);

	if(TTF_Init() < 0) {
		error_message("Error while setting up TTF support: ", TTF_GetError());
	}


	message("Initializing event queue...");

	event_queue* queue = new event_queue();

	message("Success!");


	message("Initializing screen...");

	screen* screen_obj = new screen(queue);

	message("Success!");


	message("Initializing audio...");

	audio_player* player = new audio_player();

	message("Success!");


	message("Initializing Tcl...");

	Tcl_FindExecutable(argv[0]);

	Tcl_Interp* interp = Tcl_CreateInterp();

	if(interp == NULL)
		error_message("Could not initialize Tcl (could not create interpreter).");

	if(config->bool_value("tcl_full_init")) {
		if(Tcl_Init(interp) != TCL_OK)
			error_message("Could not initialize Tcl interpreter. If you don't need packages, set tcl_full_init to false in engine.cfg.");
	}

	tcl_bind bind = tcl_bind(interp, screen_obj, player, queue);

	bind.init_namespace();

	bind.bind_all();

	std::string tcl_file = file("tcl/game.tcl");
	if(Tcl_EvalFile(interp, tcl_file.c_str()) != TCL_OK)
		error_message("Could not evaluate script:\n\n", Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY));

	message("Success!");


	mouse_cursor* cursor = screen_obj->new_sprite<mouse_cursor>("files/img/cursor.png");
	cursor->layer_id(500);

	bool game_running = true;

	bool game_active = true;
	SDL_Event sdl_event;

	while(game_running) {
		if(game_active) {
			while(SDL_PollEvent(&sdl_event)) {
				switch(sdl_event.type) {
				case SDL_ACTIVEEVENT:
					if(!sdl_event.active.gain)
						game_active = false;
					break;
				case SDL_QUIT:
					game_running = false;
					break;
				case SDL_KEYUP:
					if(sdl_event.key.keysym.sym == SDLK_ESCAPE) {
						save_game(&bind, "test.faw", screen_obj);
						game_running = false;
					}
				default:
					handle_sdl_event(sdl_event, queue, &bind);
				}
			}

			bind.call_event_code("frame");

			screen_obj->display();
		} else {
			// Restore game if minimized/lost focus

			game_active = (SDL_GetAppState() & SDL_APPINPUTFOCUS);

			if(!game_active) {
				SDL_PollEvent(&sdl_event);

				if(sdl_event.type == SDL_ACTIVEEVENT && sdl_event.active.state & SDL_APPACTIVE && sdl_event.active.gain) {
					game_active = true;
					screen_obj->reset_frameskip();
				} else if(sdl_event.type == SDL_QUIT) {
					game_running = false;
				} else {
					SDL_Delay(100); // Don't eat CPU
				}
			}
		}
	}

	delete screen_obj;

	SDL_Quit();

	return EXIT_SUCCESS;
}
