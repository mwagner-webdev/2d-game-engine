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

#include "configfile.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <stdlib.h>
#include <sstream>

#include "file.h"


config_file::config_file(const std::string& name) {
	std::ifstream handle;

	char temp_char;
	std::string temp_key;
	std::string temp_value;

	int16_t line_number = 1;
	int16_t equals_count = 0;
	bool skip_spaces = true;

	bool linebreak = true;
	bool comment = false;

	m_file = file(name);

	handle.open(m_file.c_str(), std::fstream::binary);

	// Try to create a default file (no need for parsing as a result)

	if(handle.fail()) {
		handle.close();

		fill();
		write();

		return;
	}

	// Parse our file

	while(handle.good()) {
		if(!handle.get(temp_char))
			temp_char = '\n'; // ensure all entries get read

		switch(temp_char) {
		case '\n':
			if(equals_count == 0 && !temp_key.empty()) {
				std::stringstream error;
				error << "The configuration entry \"" << temp_key << "\" on line " << line_number << " in " << m_file << " has no value (missing \"=\")";

				throw std::runtime_error(error.str().c_str());
			}

			values.insert(std::make_pair(temp_key, temp_value));

			temp_key.clear();
			temp_value.clear();

			linebreak = true;
			line_number++;
			comment = false;
			equals_count = 0;
			skip_spaces = true;
			break;
		case '\r':
			break;
		case ' ':
			if(!skip_spaces) {
				temp_value.push_back(temp_char);
			}
			break;
		case '#':
			comment = true;
			break;
		case '=':
			if(skip_spaces) { // Allow = signs as well as spaces
				equals_count++;
				break;
			}
		default:
			if(!comment) {
				if(equals_count == 0) {
					temp_key.push_back(temp_char);
				} else if(equals_count == 1) {
					temp_value.push_back(temp_char);
					skip_spaces = false;
				} else {
					std::stringstream error;
					error << "Too many = signs on line " << line_number << " in file " << m_file;

					throw std::runtime_error(error.str().c_str());
				}
			}

			linebreak = false;
		}
	}

	fill();
}

config_file::~config_file() {
	write();
}

void config_file::write() {
	std::ofstream handle;
	handle.open(m_file.c_str());

	if(handle.fail())
		throw std::runtime_error("Permission denied for file " + m_file + ".");

	for(
		values_map::iterator iter = values.begin();
		iter != values.end();
		iter++
	) {
		handle << (*iter).first + " = " + (*iter).second + "\n";
	}

	handle.close();
}

void config_file::insert_missing(std::string key, std::string value) {
	if(values.find(key) == values.end())
		values.insert(std::make_pair(key, value));
}

void config_file::fill() {
	insert_missing("screen_width", "640");
	insert_missing("screen_height", "360");
	insert_missing("display_width", "640");
	insert_missing("display_height", "360");
	insert_missing("screen_bpp", "32");
	insert_missing("fullscreen", "false");
	insert_missing("screen_zoom", "1");

	insert_missing("frameskip", "true");

	insert_missing("surface_alpha", "true");

	insert_missing("window_title", "Engine");

	insert_missing("font", "files/fonts/FreeSerif.ttf");
	insert_missing("font_size", "12");
	insert_missing("font_skip", "14");

	insert_missing("sound", "true");

	insert_missing("key_activate", "32");
}

std::string config_file::value(std::string key) {
	return values.find(key) != values.end() ? values[key] : "";
}

int16_t config_file::int_value(const std::string &key) {
	int16_t ret;

	int_map::iterator result = ints.find(key);

	if(result != ints.end()) {
		ret = (*result).second;
	} else {
		ret = atoi(value(key).c_str());

		ints.insert(std::make_pair(key, ret));
	}

	return ret;
}

bool config_file::bool_value(const std::string &key) {
	bool ret;

	bool_map::iterator result = bools.find(key);

	if(result != bools.end()) {
		ret = (*result).second;
	} else {
		ret = (value(key) == "true");

		bools.insert(std::make_pair(key, ret));
	}

	return ret;
}

void config_file::value(const std::string &key, std::string value) {
	if(values.find(key) != values.end()) {
		values[key] = value;
	} else {
		values.insert(std::make_pair(key, value));
	}
}

void config_file::value(const std::string &key, int16_t value) {
	std::stringstream stream;
	stream << value;

	this->value(key, stream.str());
}

void config_file::value(const std::string &key, bool value) {
	this->value(key, value);
}
