#include <Windows.h>
#include <vector>

extern "C" void __declspec(dllexport) __stdcall ReplaceString(
	DWORD pid,
	const char* srcString,
	const char* resString)
{
	HANDLE hProcess = OpenProcess(
		PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION,
		FALSE,
		pid);

	if (hProcess)
	{
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);

		MEMORY_BASIC_INFORMATION memoryInfo;
		std::vector<char> chunk;
		char* p = 0;

		while (p < systemInfo.lpMaximumApplicationAddress)
		{
			if (VirtualQueryEx(hProcess, p, &memoryInfo, sizeof(memoryInfo)) == sizeof(memoryInfo))
			{
				if (memoryInfo.State == MEM_COMMIT && memoryInfo.AllocationProtect == PAGE_READWRITE)
				{
					p = (char*)memoryInfo.BaseAddress;

					chunk.resize(memoryInfo.RegionSize);
					SIZE_T bytesRead;

					try 
					{
						if (ReadProcessMemory(hProcess, p, &chunk[0], memoryInfo.RegionSize, &bytesRead))
						{
							for (size_t i = 0; i < (bytesRead - strlen(srcString)); ++i)
							{
								if (memcmp(srcString, &chunk[i], strlen(srcString)) == 0)
								{
									char* ref = (char*)p + i;

									for (int j = 0; j < strlen(resString); j++)
										ref[j] = resString[j];

									ref[strlen(resString)] = 0;
								}
							}
						}
					}
					catch (std::bad_alloc& e)
					{

					}
				}

				p += memoryInfo.RegionSize;
			}
		}
	}
}