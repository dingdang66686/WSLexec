#pragma once

#include <map>
#include <string>

class Config
{
public:
	Config(const std::string& fileName);
	~Config();

	const std::string& operator[](const std::string& key);

private:
	std::map<std::string, std::string> m_config;
};