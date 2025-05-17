#include <iostream>
#include <Windows.h>

using namespace std;

typedef NTSTATUS(WINAPI* NtSuspendProcess)(HANDLE);

void suspendmylove()
{
    cout << "{ vmpmylove } suspended.\n";
    cout << "{ vmpmylove } you may attach now rahhh.\n";

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll)
        return;

    NtSuspendProcess suspendprocess = (NtSuspendProcess)GetProcAddress(ntdll, "NtSuspendProcess");
    if (!suspendprocess)
        return;

    HANDLE process = GetCurrentProcess();
    suspendprocess(process);
}

void vmpmylove()
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    HMODULE base = GetModuleHandleW(NULL);
    auto dos = (PIMAGE_DOS_HEADER)base;
    auto nt = (PIMAGE_NT_HEADERS)((BYTE*)base + dos->e_lfanew);
    auto section = IMAGE_FIRST_SECTION(nt);

    void* rdataaddr = nullptr;
    SIZE_T rdatasize = 0;

    for (int i = 0; i < nt->FileHeader.NumberOfSections; ++i)
    {
        if (memcmp((char*)section[i].Name, ".rdata", 6) == 0)
        {
            rdataaddr = (void*)((BYTE*)base + section[i].VirtualAddress);
            rdatasize = section[i].Misc.VirtualSize;
            break;
        }
    }

    if (!rdataaddr)
    {
        cout << "{ vmpmylove } .rdata not found.\n";
        return;
    }

    cout << "{ vmpmylove } doing some shit.\n";

    bool rdatawritable = false;

    while (!rdatawritable)
    {
        MEMORY_BASIC_INFORMATION mbi;
        if (VirtualQuery(rdataaddr, &mbi, sizeof(mbi)) == sizeof(mbi))
        {
            DWORD protection = mbi.Protect;
            if (protection == PAGE_READWRITE || protection == PAGE_WRITECOPY || protection == PAGE_EXECUTE_WRITECOPY)
            {
                cout << "{ vmpmylove } shit just got unpacked.\n";
                rdatawritable = true;
            }
        }
        Sleep(100);
    }

    BYTE* original = new BYTE[rdatasize];
    memcpy(original, rdataaddr, rdatasize);

    while (true)
    {
        if (memcmp(original, rdataaddr, rdatasize) != 0)
        {
            cout << "{ vmpmylove } IAT should be resolved rahhhh.\n";
            suspendmylove();
            break;
        }
        Sleep(100);
    }

    delete[] original;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)vmpmylove, 0, 0, 0);
    return TRUE;
}
