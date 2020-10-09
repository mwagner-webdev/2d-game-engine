#ifndef FILE_H
#define FILE_H

#include <string>


class file
{
public:
	file(const std::string& name);

	static void path(const std::string& name);
	static std::string path();

	operator std::string();
private:
	std::string full_name;
	static std::string m_path;
};

#endif // FILE_H
