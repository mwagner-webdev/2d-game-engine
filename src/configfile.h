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

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <stdint.h>
#include <string>
#include <map>

class config_file {
private:
    std::string m_file;

    typedef std::map<std::string, std::string> values_map;
    values_map values;

    typedef std::map<std::string, int> int_map;
    int_map ints;

    typedef std::map<std::string, bool> bool_map;
    bool_map bools;

    void insert_missing(std::string key, std::string value);
    void fill();
public:
    config_file(const std::string& file);
    ~config_file();

    void write();

    std::string value(std::string key);
    int16_t int_value(const std::string &key);
    bool bool_value(const std::string &key);
    void value(const std::string &key, std::string value);
    void value(const std::string &key, int16_t value);
    void value(const std::string &key, bool value);
};

#endif // CONFIGFILE_H
