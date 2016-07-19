#include <Windows.h>
#include <stdio.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>

// thanks to: http://stackoverflow.com/a/7956651/740182
void killProcessByName(const char *filename)
{    
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, (DWORD) NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof (pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    while (hRes)
    {
        if (strcmp(pEntry.szExeFile, filename) == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                                          (DWORD) pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);
}


int main()
{   
    int killed = 0;
    int maxSec = 20 * 60;
    const char * processName = "upwork.exe";

    LASTINPUTINFO plii;
    plii.cbSize  = sizeof(LASTINPUTINFO);
    int lastInputTicks;

    while(1)
    {        
        if (GetLastInputInfo(&plii)) 
        { 
            lastInputTicks = (int)((GetTickCount() - plii.dwTime) / 1000);

            // if user get up
            if (lastInputTicks == 0)
                killed = 0;

            // if once killed dont bother next time until user get up
            if (maxSec < lastInputTicks && killed != 0)
                killProcessByName(processName);
        }
        else
        {
            puts("unable to get last input info");
            exit(1);
        }
    }

    return 0;
}