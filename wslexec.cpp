#include <iostream>

#include <io.h>
#include <sys/stat.h>

#include "Config.h"
#include "PathParser.h"
#include "Process.h"

using namespace std;

const string EXE_EXTNAME = ".exe";

const string CONFIG_NAME = "wslexec.ini";

int main(int argc, char** argv)
{	
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);

	//process args
	string cmd = argv[0];
	size_t offset = cmd.find_last_of('\\') + 1;

	Config conf(cmd.substr(0, offset) + CONFIG_NAME);
	
	cmd = cmd.substr(offset, cmd.size() - offset - EXE_EXTNAME.size());
	
	for(int i = 1; i < argc; i++)
	{
		cmd += " \"";
		cmd += IfWinPathThenToWSLPath(argv[i]);
		cmd += "\"";
	}
	
	cmd = "wsl "
		+ (conf["distro"].empty() ? "" : ("-d " + conf["distro"] + " "))
		+ "-e "
		+ cmd;
	
	//create process

	Process p(cmd, !_isatty(_fileno(stdout)));
		
	return p.WaitForExit();
}