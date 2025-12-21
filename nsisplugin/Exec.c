/*
Copyright (C) 2002 Robert Rainwater <rrainwater@yahoo.com>
Copyright (C) 2002-2023 Nullsoft and Contributors

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

*/
#include <windows.h>
#include <commctrl.h>
#include <winnt.h>
#include <nsis/pluginapi.h> // nsis plugin
#include "main.h"

#if defined(_MSC_VER) && !defined(GetVersion)
#if _MSC_VER >= 1500
FORCEINLINE DWORD NoDepr_GetVersion(void) { __pragma(warning(push))__pragma(warning(disable:4996)) DWORD r = GetVersion(); __pragma(warning(pop)) return r; }
#define GetVersion NoDepr_GetVersion
#endif //~ _MSC_VER >= 1500
#endif //~ _MSC_VER

#define TAB_REPLACE _T("        ")
#define TAB_REPLACE_SIZE (sizeof(TAB_REPLACE) - sizeof(_T("")))
#define TAB_REPLACE_CCH (TAB_REPLACE_SIZE / sizeof(_T("")))
enum { MODE_IGNOREOUTPUT = 0, MODE_LINES = 1, MODE_STACK = 2 };

#define LOOPTIMEOUT  100
static HWND g_hwndList;

static void ExecScript(BOOL log);
static TCHAR *my_strstr(TCHAR *a, TCHAR *b);
static unsigned int my_atoi(TCHAR *s);
static int WINAPI AsExeWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);

PLUGIN_METHOD(Exec) {
  PLUGIN_INIT();
  ExecScript(MODE_IGNOREOUTPUT);
}

PLUGIN_METHOD(ExecToLog) {
  PLUGIN_INIT();
  ExecScript(extra->exec_flags->status_update & 1 ? MODE_LINES : MODE_IGNOREOUTPUT);
}

PLUGIN_METHOD(ExecToStack) {
  PLUGIN_INIT();
  ExecScript(MODE_STACK);
}

static BOOL IsLeadSurrogateUTF16(unsigned short c) { return c >= 0xd800 && c <= 0xdbff; }
static BOOL IsTrailSurrogateUTF16(unsigned short c) { return c >= 0xdc00 && c <= 0xdfff; }

static PWSTR MyCharNext(PCWSTR p)
{
  // Note: This is wrong for surrogate pair combining characters but CharNextW does
  // not support surrogate pairs correctly so we have to manually handle the pairs.
  if (!p[0]) return (PWSTR) p;
  if (IsLeadSurrogateUTF16(p[0]) && IsTrailSurrogateUTF16(p[1])) return (PWSTR) p + 2; // Current is a surrogate pair, we incorrectly assume that it is not followed by combining characters.
  if (IsLeadSurrogateUTF16(p[1]) && IsTrailSurrogateUTF16(p[2])) return (PWSTR) p + 1; // Next is a surrogate pair, we incorrectly assume that it is not a combining character for the current character.
  return (CharNextW)(p);
}
#define CharNextW MyCharNext

static void TruncateStringUTF16LE(LPWSTR Buffer, SIZE_T Length, LPCWSTR Overflow, SIZE_T lenOver) {
  if (Length) {
    LPWSTR p = &Buffer[Length - 1];
    UINT stripBaseCharIfCuttingCombining = TRUE;

    // CharNextW is buggy on XP&2003 but we don't care enough to call GetStringTypeW (http://archives.miloush.net/michkap/archive/2005/01/30/363420.html)
    if (stripBaseCharIfCuttingCombining && lenOver) {
      WCHAR buf[] = { *p, Overflow[0], lenOver > 1 ? Overflow[1] : L' ', L'\0' };
      for (;;) {
        BOOL comb = CharNextW(buf) > buf + 1;
        if (!comb || p < Buffer) break;
        *((WORD*)((BYTE*)&buf[1])) = *((WORD*)((BYTE*)&buf[0]));
        buf[0] = *p;
        *p-- = L'\0';
      }
    }

    if (IsLeadSurrogateUTF16(*p)) {
      *p = L'\0'; // Avoid incomplete pair
    }
  }
}

static void TruncateStringMB(UINT Codepage, LPSTR Buffer, SIZE_T Length, unsigned short OverflowCh) {
  if (Length) {
    CHAR *p = &Buffer[Length - 1], buf[] = { *p, ' ', ' ', '\0' };

    if (CharNextExA(Codepage, buf, 0) > buf + 1) { // Remove incomplete DBCS character?
      *p = '\0';
    }
  }
}

static BOOL IsWOW64(void) {
#ifdef _WIN64
  return FALSE;
#else
  typedef BOOL (WINAPI*ISWOW64PROCESS)(HANDLE, BOOL*);
  ISWOW64PROCESS pfIsWow64Process;
  typedef BOOL (WINAPI*ISWOW64PROCESS2)(HANDLE, USHORT*, USHORT*);
  ISWOW64PROCESS2 pfIsWow64Process2;
  HANDLE hProcess = GetCurrentProcess();
  HMODULE hK32 = GetModuleHandleA("KERNEL32");
  UINT_PTR retval;
  USHORT appmach, image_file_machine_unknown = 0;
  CHAR funcnam[16]
#if defined(_MSC_VER) && (_MSC_VER-0 <= 1400)
    = "IsWow64Process2"; // MOVSD * 4
#else
    ; lstrcpyA(funcnam, "IsWow64Process2");
#endif
  pfIsWow64Process2 = (ISWOW64PROCESS2) GetProcAddress(hK32, funcnam);
  if (pfIsWow64Process2 && pfIsWow64Process2(hProcess, &appmach, NULL)) {
    retval = image_file_machine_unknown != appmach;
  }
  else {
    BOOL wow64;
    pfIsWow64Process = (ISWOW64PROCESS) GetProcAddress(hK32, (funcnam[14] = '\0', funcnam));
    retval = (UINT_PTR) pfIsWow64Process;
    if (pfIsWow64Process && (retval = pfIsWow64Process(hProcess, &wow64))) {
      retval = wow64;
    }
  }
  return (BOOL) (UINT) retval;
#endif
}

// Tim Kosse's LogMessage
#ifdef UNICODE
static void LogMessage(const TCHAR *pStr, BOOL bOEM) {
#else
static void LogMessage(TCHAR *pStr, BOOL bOEM) {
#endif
  LVITEM item;
  int nItemCount;
  if (!g_hwndList) return;
  //if (!*pStr) return;
#ifndef UNICODE
  if (bOEM == TRUE) OemToCharBuff(pStr, pStr, lstrlen(pStr));
#endif
  nItemCount=(int) SendMessage(g_hwndList, LVM_GETITEMCOUNT, 0, 0);
  item.mask=LVIF_TEXT;
  item.pszText=(TCHAR *)pStr;
  item.cchTextMax=0;
  item.iItem=nItemCount, item.iSubItem=0;
  ListView_InsertItem(g_hwndList, &item);
  ListView_EnsureVisible(g_hwndList, item.iItem, 0);
}


void ExecScript(int mode) {
  TCHAR szRet[128];
  TCHAR meDLLPath[MAX_PATH];
  TCHAR *g_exec, *executor;
  TCHAR *pExec;
  int ignoreData = mode == MODE_IGNOREOUTPUT;
  int logMode = mode == MODE_LINES, stackMode = mode == MODE_STACK;
  unsigned int to, tabExpandLength = logMode ? TAB_REPLACE_CCH : 0, codepage;
  BOOL bOEM, forceNarrowInput = FALSE;

  *szRet = _T('\0');

  if (!IsWOW64()) {
    TCHAR* p;
    int nComSpecSize;

    nComSpecSize = GetModuleFileName(g_hInstance, meDLLPath, MAX_PATH) + 2; // 2 chars for quotes
    g_exec = (TCHAR *)GlobalAlloc(GPTR, sizeof(TCHAR) * (g_stringsize+nComSpecSize+2)); // 1 for space, 1 for null
    p = meDLLPath + nComSpecSize - 2; // point p at null char of meDLLPath
    *g_exec = _T('"');
    executor = g_exec + 1;

    // Look for the last '\' in path.
    do
    {
      if (*p == _T('\\'))
        break;
      p = CharPrev(meDLLPath, p);
    }
    while (p > meDLLPath);
    if (p == meDLLPath)
    {
      // bad path
      pushstring(_T("error"));
      GlobalFree(g_exec);
      return;
    }

    *p = 0;
    GetTempFileName(meDLLPath, _T("ns"), 0, executor);  // executor = new temp file name in module path.
    *p = _T('\\');
    if (CopyFile(meDLLPath, executor, FALSE))  // copy current DLL to temp file in module path.
    {
      HANDLE hFile, hMapping;
      LPBYTE pMapView;
      PIMAGE_NT_HEADERS pNTHeaders;
      hFile = CreateFile(executor, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING,0, 0);
      hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
      pMapView = MapViewOfFile(hMapping, FILE_MAP_WRITE, 0, 0, 0);
      if (pMapView)
      {
        pNTHeaders = (PIMAGE_NT_HEADERS)(pMapView + ((PIMAGE_DOS_HEADER)pMapView)->e_lfanew);
        // Turning the copied DLL into a stripped down executable.
        pNTHeaders->FileHeader.Characteristics = IMAGE_FILE_32BIT_MACHINE | IMAGE_FILE_LOCAL_SYMS_STRIPPED |
          IMAGE_FILE_LINE_NUMS_STRIPPED | IMAGE_FILE_EXECUTABLE_IMAGE;
        // Windows character-mode user interface (CUI) subsystem.
        pNTHeaders->OptionalHeader.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_CUI;
        // g_hInst is assumed to be the very base of the DLL in memory.
        // WinMain will have the address of the WinMain function in memory.
        // Getting the difference gets you the relative location of the
        // WinMain function.
        pNTHeaders->OptionalHeader.AddressOfEntryPoint = (DWORD) ((DWORD_PTR)AsExeWinMain - (DWORD_PTR)g_hInstance);
        UnmapViewOfFile(pMapView);
      }
      CloseHandle(hMapping);
      CloseHandle(hFile);
    }

    lstrcat(g_exec, _T("\""));

    // add space
    pExec = g_exec + lstrlen(g_exec);
    *pExec = _T(' ');
    pExec++;
  } else {
    executor = NULL;
    g_exec = (TCHAR *)GlobalAlloc(GPTR, sizeof(TCHAR) * (g_stringsize+1)); // 1 for NULL
    pExec = g_exec;
  }

  to = 0; // default is no timeout
  bOEM = FALSE; // default is no OEM->ANSI conversion

  g_hwndList = NULL;

  // g_hwndParent = the caller, usually NSIS installer.
  if (g_hwndParent) // The window class name for dialog boxes is "#32770"
    g_hwndList = FindWindowEx(FindWindowEx(g_hwndParent, NULL, _T("#32770"), NULL), NULL, _T("SysListView32"), NULL);

  // g_exec is the complete command to run: It has the copy of this DLL turned
  // into an executable right now.

params:
  // Get the command I need to run from the NSIS stack.
  popstring(pExec);
  if (my_strstr(pExec, _T("/TIMEOUT=")) == pExec) {
    TCHAR *szTimeout = pExec + 9;
    to = my_atoi(szTimeout);
    *pExec = 0;
    goto params;
  }
  if (!lstrcmpi(pExec, _T("/OEM"))) {
    bOEM = forceNarrowInput = TRUE;
    *pExec = 0;
    goto params;
  }
  if (!lstrcmpi(pExec, _T("/MBCS"))) {
    forceNarrowInput = TRUE;
    *pExec = 0;
    goto params;
  }

  if (!pExec[0])
  {
    pushstring(_T("error"));
    if (pExec-2 >= g_exec)
      *(pExec-2) = _T('\0'); // skip space and quote
    if (executor) DeleteFile(executor);
    GlobalFree(g_exec);
    return;
  }

  // Got all the params off the stack.

  {
    STARTUPINFO si = {0};
    si.cb = sizeof(si);
    SECURITY_ATTRIBUTES sa = {0};
    sa.nLength = sizeof(sa);
    SECURITY_DESCRIPTOR sd = {0};
    PROCESS_INFORMATION pi;
    const BOOL isNT = sizeof(void*) > 4 || (GetVersion() < 0x80000000);
    HANDLE newstdout = 0, read_stdout = 0;
    HANDLE newstdin = 0, read_stdin = 0;
    int utfSource = sizeof(TCHAR) > 1 && !forceNarrowInput ? -1 : FALSE, utfOutput = sizeof(TCHAR) > 1;
    DWORD cbRead, dwLastOutput;
    DWORD dwExit = 0, waitResult = WAIT_TIMEOUT;
    static BYTE bufSrc[NSIS_MAX_STRLEN];
    BYTE *pSrc;
    SIZE_T cbSrcTot = sizeof(bufSrc), cbSrc = 0, cbSrcFree;
    TCHAR *bufOutput = 0, *pNewAlloc, *pD;
    SIZE_T cchAlloc, cbAlloc, cchFree;
#ifndef _MSC_VER // Avoid GCC "may be used uninitialized in this function" warnings
    pD = NULL;
    cchAlloc = 0;
#endif

    pi.hProcess = pi.hThread = NULL;
    codepage = bOEM ? CP_OEMCP : CP_ACP;

    if (!ignoreData) {
      cbAlloc = stackMode ? (g_stringsize * sizeof(TCHAR)) : sizeof(bufSrc) * 4, cchAlloc = cbAlloc / sizeof(TCHAR);
      pD = bufOutput = GlobalAlloc(GPTR, cbAlloc + sizeof(TCHAR)); // Include "hidden" space for a \0
      if (!bufOutput) {
        lstrcpy(szRet, _T("error"));
        goto done;
      }
      *bufOutput = _T('\0');
    }

    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    if (isNT) {
      InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
      SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);
      sa.lpSecurityDescriptor = &sd;
    }

    if (!CreatePipe(&read_stdout, &newstdout, &sa, 0)) {
      lstrcpy(szRet, _T("error"));
      goto done;
    }
    if (!CreatePipe(&read_stdin, &newstdin, &sa, 0)) {
      lstrcpy(szRet, _T("error"));
      goto done;
    }

    GetStartupInfo(&si); // Why?
    si.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = newstdin;
    si.hStdOutput = newstdout;
    si.hStdError = newstdout;
    if (!CreateProcess(NULL, g_exec, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
      lstrcpy(szRet, _T("error"));
      goto done;
    }

    // Now I'm talking with an executable copy of myself.
    dwLastOutput = GetTickCount();
    for (;;) {
      TCHAR bufCh[2];
waitForProcess:
      waitResult = WaitForSingleObject(pi.hProcess, 0);
      GetExitCodeProcess(pi.hProcess, &dwExit);
readMore:
      PeekNamedPipe(read_stdout, 0, 0, 0, &cbRead, NULL);
      if (!cbRead) {
        if (waitResult == WAIT_OBJECT_0) {
          break; // No data in the pipe and process ended, we are done
        }
        if (to && GetTickCount() > dwLastOutput+to) {
          TerminateProcess(pi.hProcess, -1);
          lstrcpy(szRet, _T("timeout"));
        }
        else {
          Sleep(LOOPTIMEOUT);
        }
        continue;
      }

      dwLastOutput = GetTickCount();
      ReadFile(read_stdout, bufSrc + cbSrc, (DWORD) (cbSrcFree = cbSrcTot - cbSrc), &cbRead, NULL);
      cbSrcFree -= cbRead, cbSrc = cbSrcTot - cbSrcFree;
      pSrc = bufSrc;

      if (utfSource < 0 && cbSrc) { // Simple UTF-16LE detection
#ifdef UNICODE
        utfSource = IsTextUnicode(pSrc, (UINT) (cbSrc & ~1), NULL) != FALSE;
#else
        utfSource = (cbSrc >= 3 && pSrc[0] && !pSrc[1]) || (cbSrc > 4 && pSrc[2] && !pSrc[3]); // Lame latin-only test
        utfSource |= (cbSrc > 3 && pSrc[0] == 0xFF && pSrc[1] == 0xFE && (pSrc[2] | pSrc[3])); // Lame BOM test
#endif
      }

      if (ignoreData) {
        cbSrc = 0; // Overwrite the whole buffer every read
        continue;
      }

      if (!cbRead) {
        continue; // No new data, read more before trying to parse
      }

parseLines:
      cchFree = cchAlloc - (pD - bufOutput);
      for (;;) {
        DWORD cbSrcChar = 1, cchDstChar, i;
        *pD = _T('\0'); // Terminate output buffer because we can unexpectedly run out of data
        if (!cbSrc) {
          goto readMore;
        }

        if (utfSource) { // UTF-16LE --> ?:
          if (cbSrc < 2) {
            goto readMore;
          }
          if (utfOutput) { // UTF-16LE --> UTF-16LE:
            bufCh[0] = ((TCHAR*)pSrc)[0], cbSrcChar = sizeof(WCHAR), cchDstChar = 1; // We only care about certain ASCII characters so we don't bother dealing with surrogate pairs.
          }
          else { // UTF-16LE --> DBCS
            // TODO: This is tricky because we need the complete base character (or surrogate pair) and all the trailing combining characters for a grapheme in the buffer before we can call WideCharToMultiByte.
            utfOutput = FALSE; // For now we just treat it as DBCS
            continue;
          }
        }
        else { // DBCS --> ?:
          if (utfOutput) { // DBCS --> UTF-16LE:
            BOOL isMb = IsDBCSLeadByteEx(codepage, ((CHAR*)pSrc)[0]);
            if (isMb && cbSrc < ++cbSrcChar) {
              goto readMore;
            }
            cchDstChar = MultiByteToWideChar(codepage, 0, (CHAR*)pSrc, cbSrcChar, (WCHAR*) bufCh, 2);
          }
          else { // DBCS --> DBCS:
            bufCh[0] = ((CHAR*)pSrc)[0], cchDstChar = 1; // Note: OEM codepage will be converted by LogMessage
          }
        }

        if (bufCh[0] == _T('\t') && tabExpandLength) { // Expand tab to spaces?
          if (cchFree < tabExpandLength) {
            goto resizeOutputBuffer;
          }
          lstrcpy(pD, TAB_REPLACE);
          pD += tabExpandLength, cchFree -= tabExpandLength;
        }
        else if (bufCh[0] == _T('\r') && logMode) {
          // Eating it
        }
        else if (bufCh[0] == _T('\n') && logMode) {
          LogMessage(bufOutput, bOEM); // Output has already been \0 terminated
          *(pD = bufOutput) = _T('\0'), cchFree = cchAlloc;
        }
        else {
          if (cchFree < cchDstChar) {
            SIZE_T cchOrgOffset;
resizeOutputBuffer:
            if (stackMode) {
              ignoreData = TRUE; // Buffer was already maximum for the NSIS stack, we cannot handle more data
              if (utfOutput)
                TruncateStringUTF16LE((LPWSTR) bufOutput, pD - bufOutput, (LPCWSTR) bufCh, cchDstChar);
              else
                TruncateStringMB(codepage, (LPSTR) bufOutput, pD - bufOutput, bufCh[0]);
              goto waitForProcess;
            }
            cchAlloc += 1024, cbAlloc = cchAlloc / sizeof(TCHAR);
            pNewAlloc = GlobalReAlloc(bufOutput, cbAlloc + sizeof(TCHAR),GPTR|GMEM_MOVEABLE); // Include "hidden" space for a \0
            if (!pNewAlloc) {
              lstrcpy(szRet, _T("error"));
              ignoreData = TRUE;
              goto waitForProcess;
            }
            cchOrgOffset = pD - bufOutput;
            *(pD = (bufOutput = pNewAlloc) + cchOrgOffset) = _T('\0');
            goto parseLines;
          }
          for (i = 0; i < cchDstChar; ++i) {
            *pD++ = bufCh[i], --cchFree;
          }
        }
        pSrc += cbSrcChar, cbSrc -= cbSrcChar;
      }
    }

done:
    if (stackMode) pushstring(bufOutput);
    if (logMode && *bufOutput) LogMessage(bufOutput,bOEM); // Write remaining output
    if (dwExit == STATUS_ILLEGAL_INSTRUCTION)
      lstrcpy(szRet, _T("error"));
    if (!szRet[0]) wsprintf(szRet,_T("%d"),dwExit);
    pushstring(szRet);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    CloseHandle(newstdout);
    CloseHandle(read_stdout);
    CloseHandle(newstdin);
    CloseHandle(read_stdin);
    if (pExec-2 >= g_exec)
      *(pExec-2) = _T('\0'); // skip space and quote
    if (executor)
      DeleteFile(executor);
    GlobalFree(g_exec);
    if (bufOutput)
      GlobalFree(bufOutput);
  }
}

static TCHAR *my_strstr(TCHAR *a, TCHAR *b)
{
  int l = lstrlen(b);
  while (lstrlen(a) >= l)
  {
    TCHAR c = a[l];
    a[l] = 0;
    if (!lstrcmpi(a, b))
    {
      a[l] = c;
      return a;
    }
    a[l] = c;
    a = CharNext(a);
  }
  return NULL;
}

static unsigned int my_atoi(TCHAR *s) {
  unsigned int v=0;
  if (*s == _T('0') && (s[1] == _T('x') || s[1] == _T('X'))) {
    s+=2;
    for (;;) {
      int c=*s++;
      if (c >= _T('0') && c <= _T('9')) c-=_T('0');
      else if (c >= _T('a') && c <= _T('f')) c-=_T('a')-10;
      else if (c >= _T('A') && c <= _T('F')) c-=_T('A')-10;
      else break;
      v<<=4;
      v+=c;
    }
  }
  else if (*s == _T('0') && s[1] <= _T('7') && s[1] >= _T('0')) {
    s++;
    for (;;) {
      int c=*s++;
      if (c >= _T('0') && c <= _T('7')) c-=_T('0');
      else break;
      v<<=3;
      v+=c;
    }
  }
  else {
    for (;;) {
      int c=*s++ - _T('0');
      if (c < 0 || c > 9) break;
      v*=10;
      v+=c;
    }
  }
  return (int)v;
}

int WINAPI AsExeWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
  DWORD               Ret;
  STARTUPINFO         si   = {0};
  PROCESS_INFORMATION pi   = {0};
  TCHAR command_line[NSIS_MAX_STRLEN]; //BUGBUG
  TCHAR seekchar=_T(' ');
  TCHAR *cmdline;

  si.cb = sizeof(si);
  // Make child process use this app's standard files. Not needed because the handles
  // we created when executing this process were inheritable.
  //si.dwFlags    = STARTF_USESTDHANDLES;
  //si.hStdInput  = GetStdHandle (STD_INPUT_HANDLE);
  //si.hStdOutput = GetStdHandle (STD_OUTPUT_HANDLE);
  //si.hStdError  = GetStdHandle (STD_ERROR_HANDLE);
  lstrcpyn(command_line, GetCommandLine(), 1024);

  cmdline = command_line;
  if (*cmdline == _T('\"')) seekchar = *cmdline++;

  while (*cmdline && *cmdline != seekchar) cmdline=CharNext(cmdline);
  cmdline=CharNext(cmdline);
  // skip any spaces before the arguments
  while (*cmdline && *cmdline == _T(' ')) cmdline++;

  Ret = CreateProcess (NULL, cmdline,
    NULL, NULL,
    TRUE, 0,
    NULL, NULL,
    &si, &pi
    );

  if (Ret)
  {
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &Ret);
    CloseHandle (pi.hProcess);
    CloseHandle (pi.hThread);
    ExitProcess(Ret);
  }
  else
  {
    ExitProcess(STATUS_ILLEGAL_INSTRUCTION);
  }

  return 0; // dummy
}
