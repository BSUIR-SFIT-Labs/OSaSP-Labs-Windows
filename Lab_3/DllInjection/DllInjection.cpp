#include <Windows.h>
#include <iostream>
#include <string>

using namespace std;

extern "C" __declspec(dllimport) void __stdcall ReplaceString(
	DWORD pid,
	const char* srcString,
	const char* resString);

typedef void __stdcall TReplaceString(DWORD, const char*, const char*);

void ReplaceStringDynamic(DWORD, const char*, const char*);
void InjectLibrary(DWORD);

int main()
{
	DWORD pid = GetCurrentProcessId();
	bool isExit = false;

	const char src_str[] = "Hello, world";
	const char res_str[] = "dlrow ,olleH";

	while (!isExit)
	{
		cout << "Available actions:" << endl 
			<< "\t0 - Static import and call" << endl
			<< "\t1 - Dynamic import and call" << endl 
			<< "\t2 - Dll injection" << endl
			<< "Enter action: ";

		char action;

		cin >> action;

		switch (action)
		{
		case '0':
			cout << endl << "String before change: " << src_str << endl;		
			ReplaceString(pid, src_str, res_str);
			cout << "String after changes: " << src_str << endl << endl;
			break;
		case '1':
			cout << endl << "String before change: " << src_str << endl;
			ReplaceStringDynamic(pid, src_str, res_str);
			cout << "String after changes: " << src_str << endl << endl;
			break;
		case '2':
			cout << endl << "String before change: " << src_str << endl;
			InjectLibrary(pid);
			cout << "String after changes: " << src_str << endl << endl;
			break;
		default:
			isExit = true;
			break;
		}
	}

	system("pause");
	return 0;
}

void ReplaceStringDynamic(DWORD pid, const char* src_str, const char* res_str)
{
	HMODULE hDll = LoadLibrary("DllStringReplacement.dll");

	if (hDll != NULL)
	{
		TReplaceString* lpReplaceString = (TReplaceString*)GetProcAddress(hDll, "_ReplaceString@12");

		if (lpReplaceString != NULL)
		{
			lpReplaceString(pid, src_str, res_str);
		}

		FreeLibrary(hDll);
	}
}

void InjectLibrary(DWORD procID)
{
	HANDLE hProc = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION | PROCESS_VM_WRITE |
		PROCESS_CREATE_THREAD | PROCESS_CREATE_PROCESS,
		FALSE, procID);

	if (hProc)
	{
		LPVOID baseAddress = VirtualAllocEx(hProc, NULL, strlen("InjectableDll.dll") + 1,
			MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		if (baseAddress)
		{
			WriteProcessMemory(hProc, baseAddress, "InjectableDll.dll",
				strlen("InjectableDll.dll") + 1, NULL);

			DWORD threadId;

			HANDLE hThread = CreateRemoteThread(hProc, NULL, NULL,
				(LPTHREAD_START_ROUTINE)LoadLibraryA, (LPVOID)baseAddress, NULL, &threadId);

			if (hThread == NULL)
				cout << "Error" << endl;
			else
				WaitForSingleObject(hThread, INFINITE);
		}

		CloseHandle(hProc);
	}
}