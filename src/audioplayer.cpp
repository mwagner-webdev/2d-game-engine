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

#include "audioplayer.h"

#include <iostream>

#include <SDL/SDL_thread.h>

#include "globals.h"
#include "file.h"

int thread_callback(void* data) {
	//fawesound_playdata play_data;

	//play_data.loops = 1;
	//play_data.volume = 100;

	//fawesound_play((char*)data, &play_data);

	return 0;
}

audio_player::audio_player() {
	//fawesound_init();
}

void audio_player::play_audio(const std::string& name) {
	if(!config->bool_value("sound"))
		return;

	std::string file_name = file(name);
	const char* const_data = file_name.c_str();
	char* data = new char[strlen(const_data) + 1];
	strcpy(data, const_data);

	SDL_CreateThread(thread_callback, data);
}

void audio_player::play_audio_loop(const std::string& name) {
	play_audio(name);
}
