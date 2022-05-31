#pragma once

#include <Windows.h>
#include <string>

class Process
{
public:
	Process(std::string cmd, bool redirected);
	~Process();

	DWORD WaitForExit();

private:
	PROCESS_INFORMATION m_proc;
	STARTUPINFO m_startupInfo;
	HANDLE hReadPipe;
	HANDLE hWritePipe;
	bool m_redirected;
};