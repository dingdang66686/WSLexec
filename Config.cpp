#include "Config.h"

#include <fstream>

using namespace std;

string trim(string);

Config::Config(const string& fileName)
{
	char buffer[100];
	ifstream file(fileName);
	if (file.is_open())
	{
		while (file.getline(buffer, 100))
		{
			string line = buffer;
			size_t devide = line.find_first_of('=');
			if (devide == string::npos)
				continue;
			string key = trim(line.substr(0, devide));
			string value = trim(line.substr(devide + 1));
			m_config[key] = value;
		}
		file.close();
	}
}

Config::~Config()
{
}

const string& Config::operator[](const string& key)
{
	return m_config[key];
}

string trim(string s)
{
	if (!s.empty())
	{
		s.erase(0, s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
	}
	return s;
}