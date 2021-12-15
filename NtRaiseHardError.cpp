#include <Windows.h>
#include <winternl.h>

typedef NTSTATUS
(NTAPI* pdef_RtlAdjustPrivilege)
(ULONG Privilege,
    BOOLEAN Enable,
    BOOLEAN CurrentThread,
    PBOOLEAN Enabled);
typedef NTSTATUS
(NTAPI* pdef_NtRaiseHardError)
(NTSTATUS ErrorStatus,
    ULONG NumberOfParameters,
    ULONG UnicodeStringParameterMask OPTIONAL,
    PULONG_PTR Parameters,
    ULONG ResponseOption,
    PULONG Response);

void TriggerHardBsod()
{
    BOOLEAN bEnabled;
    ULONG uResp;
    LPVOID lpFuncAddress1 = GetProcAddress(LoadLibraryA("ntdll.dll"), "RtlAdjustPrivilege");
    LPVOID lpFuncAddress2 = GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtRaiseHardError");
    pdef_RtlAdjustPrivilege RtlAdjustPrivilege = (pdef_RtlAdjustPrivilege)lpFuncAddress1;
    pdef_NtRaiseHardError NtRaiseHardError = (pdef_NtRaiseHardError)lpFuncAddress2;
    RtlAdjustPrivilege(19, TRUE, FALSE, &bEnabled);
    NtRaiseHardError(STATUS_FLOAT_MULTIPLE_FAULTS, 0, 0, 0, 6, &uResp);
}

int main()
{
    TriggerHardBsod();
    ExitProcess(1);
}
