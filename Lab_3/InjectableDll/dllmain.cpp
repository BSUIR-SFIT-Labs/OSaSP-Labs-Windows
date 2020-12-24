#include <Windows.h>

#include "../DllStringReplacement/StringReplacer.cpp"

BOOL APIENTRY DllMain( 
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved)
{
    DWORD pid = GetCurrentProcessId();

    const char src_str[] = "Hello, world";
    const char res_str[] = "dlrow ,olleH";

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        ReplaceString(pid, src_str, res_str);
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}