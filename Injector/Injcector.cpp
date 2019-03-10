#include <Windows.h>
#include <stdio.h>

#define PID_EMPTY 0

DWORD init_process(const char* app)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	si.cb = sizeof(pi);

	char ap[] = "sdasd";

	if (!CreateProcess(NULL, (char*)app, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
	{
		printf("CreateProcess failed (%d).\n", GetLastError());
		return 0;
	}

	//CloseHandle ?? 

	return pi.dwProcessId;
}

int main()
{
	char hookLibname[] = "hook.dll";
	DWORD processId = init_process("SimpleWS.exe");

	if (processId == PID_EMPTY) {
		puts("Pid empty.");
		return 1;
	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

	if (hProcess == NULL) {
		puts("hProcess is null.");

		return 1;
	}
	puts("Opening target process.");

	//Allocate space for libname
	LPVOID libnameAddr = VirtualAllocEx(hProcess, NULL, 0x1000, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (libnameAddr == NULL) {
		puts("VirtualAllocEx error.");

		CloseHandle(hProcess);

		return 1;
	}
	puts("Allocating memory in target procecss.");

	//Copy lib name into remote(target) space
	SIZE_T bytesWritten;
	if (!WriteProcessMemory(hProcess, libnameAddr, hookLibname, strlen(hookLibname) + 1, &bytesWritten)) {
		puts("WriteProcessMemory error.");

		CloseHandle(hProcess);
	}

	//Load library in remote process by creating new remote thread
	LPTHREAD_START_ROUTINE routine = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");

	puts("Press <Enter> to load dll into remote process.");
	getchar();

	DWORD threadId;
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, routine, libnameAddr, 0, &threadId);

	if (hThread == NULL) {
		puts("CreateRemoteThread error.");

		CloseHandle(hProcess);
	}
	puts("Thread created -> Library should be loaded.");

	puts("Hooks should be loaded.");
	puts("Press any key to exit...");
	getchar();

	CloseHandle(hThread);
	CloseHandle(hProcess);
}