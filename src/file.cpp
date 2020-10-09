#include "file.h"

#include <iostream>

std::string file::m_path = std::string("./");

file::file(const std::string& name) {
	full_name = m_path + name;
}

void file::path(const std::string& name) {
	m_path = std::string(name);
}

std::string file::path() {
	return m_path;
}

file::operator std::string() {
	return full_name;
}
