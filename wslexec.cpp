#include <iostream>
#include <algorithm>

#include <Windows.h>
#include <io.h>

using namespace std;

const int BUFFER_SIZE = 0xFFFF;

const string WSL_PREFIX = "/mnt/";
const string EXE_EXTNAME = ".exe";

string IfWinPathThenToWSLPath(string s);
string IfWSLPathThenToWinPath(string s);

int main(int argc, char** argv)
{
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);

	string cmd = argv[0];
	size_t offset = cmd.find_last_of('\\') + 1;
	cmd = cmd.substr(offset, cmd.size() - offset - EXE_EXTNAME.size());
	
	for(int i = 1; i < argc; i++)
	{
		cmd += " \"";
		cmd += IfWinPathThenToWSLPath(argv[i]);
		cmd += "\"";
	}
	
	cmd = "wsl -e " + cmd;
	
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	
	HANDLE hReadPipe = NULL;
	HANDLE hWritePipe = NULL;

	if (!_isatty(_fileno(stdout)))
	{
		if(!CreatePipe(&hReadPipe, &hWritePipe, NULL, 0))
			throw runtime_error("CreatePipe failed");

		if(!SetHandleInformation(hReadPipe, 1, 0))
			throw runtime_error("SetupPipe failed");

		si.hStdOutput = hWritePipe;
	}
	
	si.cb = sizeof(si);

	if (!CreateProcess(NULL, const_cast<LPSTR>(cmd.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		throw runtime_error("CreateProcess failed");
	
	if (!_isatty(_fileno(stdout))) 
	{
		if(hReadPipe == NULL || hWritePipe == NULL)
			throw runtime_error("hReadPipe or hWritePipe is NULL");
		
		if (!CloseHandle(hWritePipe))
			throw runtime_error("CloseOwnWritingHandle failed");
		
		char* buffer = new char[BUFFER_SIZE];
		
		DWORD count;
		if (ReadFile(hReadPipe, buffer, BUFFER_SIZE, &count, NULL) || count == 0)
			strcpy_s(buffer, BUFFER_SIZE, IfWSLPathThenToWinPath(buffer).c_str());
		WriteFile(stdout, buffer, count, &count, NULL);
		while(ReadFile(hReadPipe, buffer, BUFFER_SIZE, &count, NULL) && count != 0)
			WriteFile(stdout, buffer, count, &count, NULL);

		delete[] buffer;

		buffer = nullptr;
	}
	
	WaitForSingleObject(pi.hProcess, INFINITE);
	
	DWORD exitCode = 0;
	GetExitCodeProcess(pi.hProcess, &exitCode);
	
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	
	return exitCode;
}

// If s is a Win path, then convert it to a WSL path
 string IfWinPathThenToWSLPath(string s)
{
	// Process relative path
	replace(s.begin(), s.end(), '\\', '/');
	
	if(tolower(s[0]) < 'a' || tolower(s[0]) > 'z')
		return s;
	if(s[1] != ':')
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

	if(s.find(WSL_PREFIX) != 0)
		return s;
	
	s.erase(0, WSL_PREFIX.size());
	s[0] = toupper(s[0]);
	s.insert(1, ":");
	
	return s;
}
