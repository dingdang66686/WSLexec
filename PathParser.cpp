#include "PathParser.h"

#include <algorithm>

using namespace std;

const string WSL_PREFIX = "/mnt/";

// If s is a Win path, then convert it to a WSL path
string IfWinPathThenToWSLPath(string s)
{
	// Process relative path
	replace(s.begin(), s.end(), '\\', '/');

	if (tolower(s[0]) < 'a' || tolower(s[0]) > 'z')
		return s;
	if (s[1] != ':')
		return s;
	s[0] = tolower(s[0]);
	s.erase(1, 1);
	s.insert(0, WSL_PREFIX);
	return s;
}

// If s is a WSL path, then convert it to a Win path
string IfWSLPathThenToWinPath(string s)
{
	// Process relative path
	replace(s.begin(), s.end(), '/', '\\');

	if (s.find(WSL_PREFIX) != 0)
		return s;

	s.erase(0, WSL_PREFIX.size());
	s[0] = toupper(s[0]);
	s.insert(1, ":");

	return s;
}
