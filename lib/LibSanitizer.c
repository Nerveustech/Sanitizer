/*
 * MIT License
 *
 * Copyright (c) 2024 Andrea Michael M. Molino
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
#include "LibSanitizer.h"
#include "LibPrint.h"

/// @brief Call this function before exit and do all the necessary cleanup.
void before_exit(void)
{
    int enter = 0;
    
    print_log(LOG_EMPTY, L"Press Enter for exit.");
    while(enter != '\r' && enter != '\n'){
        enter = getchar();
    }
}

void print_banner(void)
{
    print_log(LOG_INFO, L"Sanitizer Collection\n");
    print_log(LOG_INFO, L"Created By Nerveustech\n");
    print_log(LOG_INFO, L"https://github.com/Nerveustech/Sanitizer\n");
}

BOOL is_usb_mode(void)
{
    WCHAR full_path[MAX_PATH] = {0},  volume_path[MAX_PATH] = {0};

    if(FAILED(GetModuleFileNameW(NULL, full_path, MAX_PATH)))
        return FALSE;
    
    if(FAILED(GetVolumePathNameW(full_path, volume_path, MAX_PATH)))
        return FALSE;

    if(GetDriveTypeW(volume_path) == 2)
        return TRUE;
    
    return FALSE;
}

BOOL does_folder_exist(const WCHAR* directory)
{
    DWORD fileattributes_ret = GetFileAttributesW(directory);
    if(fileattributes_ret == INVALID_FILE_ATTRIBUTES)
        return FALSE;
    
    if(fileattributes_ret & FILE_ATTRIBUTE_DIRECTORY)
        return TRUE;
    
    return FALSE;
}

BOOL remove_folder(const WCHAR* directory)
{    
    SHFILEOPSTRUCTW shfo = {
        .hwnd = NULL,
        .wFunc = FO_DELETE,
        .pFrom = directory,
        .pTo = NULL,
        .fFlags = FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION,
        .fAnyOperationsAborted = FALSE,
        .hNameMappings = NULL,
        .lpszProgressTitle = NULL};

    if(SUCCEEDED(SHFileOperationW(&shfo))){
        return TRUE;
    }
    else{
        return FALSE;
    }
}

int kill_process(const WCHAR* procname)
{
    HANDLE snapshot = NULL, process_to_kill = NULL;
    DWORD process_pid = 0;
    PROCESSENTRY32W pe32w = {0};

    pe32w.dwSize = sizeof(PROCESSENTRY32W);
    
    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(snapshot == INVALID_HANDLE_VALUE) 
        return 4;

    if(!Process32FirstW(snapshot, &pe32w)){
        CloseHandle(snapshot);
        return 4;
    }

    do{
        if(wcscmp(procname, pe32w.szExeFile) == 0){
            process_pid = pe32w.th32ProcessID;
            process_to_kill = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_pid);
            if(!TerminateProcess(process_to_kill, 0))
                return 2;
            CloseHandle(process_to_kill);
            return 3;
        }
    }
    while(Process32NextW(snapshot, &pe32w));

    CloseHandle(snapshot);
    
    return 1;
}