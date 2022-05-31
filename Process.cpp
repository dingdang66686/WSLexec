#include "Process.h"
#include "PathParser.h"
#include <stdexcept>

using namespace std;

const int BUFFER_SIZE = 0x80000;

Process::Process(string cmd, bool redirected) : m_redirected(redirected)
{
	ZeroMemory(&m_startupInfo, sizeof(m_startupInfo));
	ZeroMemory(&m_proc, sizeof(m_proc));

	if (m_redirected)
	{
		if (!CreatePipe(&hReadPipe, &hWritePipe, NULL, 0))
			throw runtime_error("CreatePipe failed");
		
		if (!SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0))
			throw runtime_error("SetupPipe failed");

		m_startupInfo.hStdOutput = hWritePipe;
	}

	m_startupInfo.cb = sizeof(m_startupInfo);

	if (!CreateProcess(NULL, const_cast<LPSTR>(cmd.c_str()),
		NULL, NULL, FALSE, 0, NULL, NULL,
		&m_startupInfo, &m_proc))
		throw runtime_error("CreateProcess failed");
}

Process::~Process()
{
}

DWORD Process::WaitForExit()
{
	if (m_redirected)
	{
		if (!CloseHandle(hWritePipe))
			throw runtime_error("CloseOwnWritingHandle failed");

		char* buffer = new char[BUFFER_SIZE];

		DWORD count;
		if (ReadFile(hReadPipe, buffer, BUFFER_SIZE, &count, NULL) && count > 0)
		{
			strcpy_s(buffer, BUFFER_SIZE, IfWSLPathThenToWinPath(buffer).c_str());
			WriteFile(stdout, buffer, count, NULL, NULL);
			while (ReadFile(hReadPipe, buffer, BUFFER_SIZE, &count, NULL) && count != 0)
				WriteFile(stdout, buffer, count, NULL, NULL);
		}
		delete[] buffer;
		buffer = nullptr;
	}
	WaitForSingleObject(m_proc.hProcess, INFINITE);

	DWORD exitCode = 0;
	GetExitCodeProcess(m_proc.hProcess, &exitCode);

	CloseHandle(m_proc.hProcess);
	CloseHandle(m_proc.hThread);

	return exitCode;
}
