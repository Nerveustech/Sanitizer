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

#include <Windows.h>
#include <string.h>
#include <Shlwapi.h>
#include <ShlObj.h>

#define LIB_PRINT_IMPLEMENTATION
#include "LibPrint.h"
#include "../../lib/LibSanitizer.h"

/*Module struct*/
typedef struct
{
    const WCHAR mmr_lnk_argument[47];
    const WCHAR mmr_vbs_helper[11];
    const WCHAR mmr_lnk_helper[12];
    const WCHAR mmr_vbs_installer[14];
    const WCHAR mmr_vbs_main[16];
    const WCHAR mmr_exe_miner[20];
    const WCHAR mmr_folder[16];
    const WCHAR mmr_process[12];
    
    BOOL usb_mode;
} Sanitizer_s;

Sanitizer_s Sanitizer = {
    .mmr_lnk_argument  = L"/C .\\WindowsServices\\movemenoreg.vbs",
    .mmr_vbs_helper    = L"helper.vbs",
    .mmr_lnk_helper    = L"helper.lnk",
    .mmr_vbs_installer = L"installer.vbs",
    .mmr_vbs_main      = L"movemenoreg.vbs",
    .mmr_exe_miner     = L"WindowsServices.exe",
    .mmr_folder        = L"WindowsServices",
    .mmr_process       = L"wscript.exe"
};

/*function prototypes*/
void InitSanitizer(void);
void match_lnk_sig(WIN32_FIND_DATAW file_data);
void find_lnk_remove(void);
void restore_original_file(void);
/*End function prototypes*/


void InitSanitizer(void)
{
    if(is_usb_mode() == TRUE){
        Sanitizer.usb_mode = TRUE;
        return;
    }

    Sanitizer.usb_mode = FALSE;
}

void match_lnk_sig(WIN32_FIND_DATAW file_data)
{
    IShellLinkW* psl = {0};
    HRESULT hres;
    IPersistFile* ppf = {0};
    WIN32_FIND_DATAW wfd = {0};
    WCHAR wc_target_lnk[MAX_PATH] = {0};
    WCHAR wc_lnk_args[MAX_PATH] = {0};

    hres = CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLink, (LPVOID*)&psl);
            
    if(SUCCEEDED(hres)){
        IPersistFile* ppf = {0};
        psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, (LPVOID*)&ppf);
        
        LPCOLESTR lnk_file = SysAllocString(file_data.cFileName);
        ppf->lpVtbl->Load(ppf, file_data.cFileName, STGM_READ);
        SysFreeString((void*)lnk_file);

        psl->lpVtbl->Resolve(psl, NULL, SLR_NO_UI);


        psl->lpVtbl->GetArguments(psl, wc_target_lnk, MAX_PATH);
        MultiByteToWideChar(CP_ACP, 0, (LPCCH)wc_target_lnk, -1, wc_lnk_args, MAX_PATH);

        if(wcscmp(wc_lnk_args, Sanitizer.mmr_lnk_argument) == 0){
            print_log(LOG_WARNING, L"Malevolent LNK detected: %s\n", file_data.cFileName);

            if(DeleteFileW(file_data.cFileName))
                print_log(LOG_SUCCESS, L"Malevolent LNK removed: %s\n", file_data.cFileName);
            else
                print_log(LOG_WARNING, L"Malevolent LNK could not be removed: %s\n", file_data.cFileName);
        }
        
        ppf->lpVtbl->Release(ppf);
    }

    psl->lpVtbl->Release(psl);

}

void find_lnk_remove(void)
{
    WIN32_FIND_DATAW wfd_file = {0};
    HANDLE hfile = NULL;

    if((hfile = FindFirstFileW(L"*.lnk", &wfd_file)) == INVALID_HANDLE_VALUE){
        return;
    }
    
    do
    {
        if(wcscmp(wfd_file.cFileName, L".") != 0 && wcscmp(wfd_file.cFileName, L"..") != 0){
            match_lnk_sig(wfd_file);
        }
    }
    while(FindNextFileW(hfile, &wfd_file));
    FindClose(hfile);
    
    return;
}

void restore_original_file(void)
{
    if(!does_folder_exist(L"_")){
        print_log(LOG_INFO, L"There are no files to restore.\n");
        return;
    }

    WIN32_FIND_DATAW wfd_file = {0};
    HANDLE hfile = NULL;

    if((hfile = FindFirstFileW(L"_\\*", &wfd_file)) == INVALID_HANDLE_VALUE){
        return;
    }

    do
    {
        if(wcscmp(wfd_file.cFileName, L".") != 0 && wcscmp(wfd_file.cFileName, L"..") != 0){
            WCHAR source_file[MAX_PATH + 1];
            WCHAR dest_file[MAX_PATH + 1];

            swprintf(source_file, MAX_PATH, L"_\\%s", wfd_file.cFileName);
            swprintf(dest_file, MAX_PATH, L".\\%s", wfd_file.cFileName);

            if(wfd_file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
                MoveFileW(source_file, dest_file);
            }
            else{
                MoveFileW(source_file, dest_file);
            }
        }
    }
    while(FindNextFileW(hfile, &wfd_file) != 0);
    FindClose(hfile);
    
    if(RemoveDirectoryW(L"_")){
        print_log(LOG_SUCCESS, L"All files have been restored.\n");
    }
    else{
        print_log(LOG_WARNING, L"Some files could not been restored.\n");
    }
    return;
}

int main(void)
{
    SetConsoleTitleW(L"Movemenoreg Sanitizer");

    print_log(LOG_INFO, L"Initializing Sanitizer...\n");
    CoInitialize(NULL);
    InitSanitizer();

    //Usb section
    if(Sanitizer.usb_mode){
        print_log(LOG_INFO, L"Running Sanitizer on USB...\n");
        print_log(LOG_INFO, L"Module: %s\n", Sanitizer.mmr_vbs_main);
        
        print_log(LOG_INFO, L"Searching for LNK file.\n");
        find_lnk_remove();
        print_log(LOG_SUCCESS, L"Removed All LNK file.\n");

        print_log(LOG_INFO, L"Searching for folder where reside the malware.\n");
        if(does_folder_exist(Sanitizer.mmr_folder)){
            print_log(LOG_WARNING, L"Malware folder detected: %s.\n", Sanitizer.mmr_folder);
            print_log(LOG_INFO, L"Attempting to remove: %s.\n", Sanitizer.mmr_folder);
            
            if(remove_folder(Sanitizer.mmr_folder)){
                print_log(LOG_SUCCESS, L"Malware folder removed.\n");
            }
            print_log(LOG_WARNING, L"Malware folder could not be removed.\n");
        }
        else{
            print_log(LOG_SUCCESS, L"Malware folder not present.\n");
        }

        print_log(LOG_INFO, L"Attempting to restore the original files it may take some time.\n");
        restore_original_file();


        before_exit();
        CoUninitialize();
        return 0;
    }
    //Desktop section
    else
    {
        PWSTR roaming_appdata = {0}, startup_path = {0};
        WCHAR roaming_appdata_formatted[MAX_PATH] = {0}, startup_formatted[MAX_PATH] = {0};
        int kill_proc_ret = 0;

        print_log(LOG_INFO, L"Running Sanitizer on Desktop...\n");
        print_log(LOG_INFO, L"Module: %s\n", Sanitizer.mmr_vbs_main);

        print_log(LOG_INFO, L"Searching for malevolent process: %s.\n", Sanitizer.mmr_process);
        kill_proc_ret = kill_process(Sanitizer.mmr_process);
        switch (kill_proc_ret)
        {
            case 1:
                print_log(LOG_SUCCESS, L"Malevolent process: %s not present.\n", Sanitizer.mmr_process);
                break;
            case 2:
                print_log(LOG_WARNING, L"Malevolent process: %s could not be killed.\n", Sanitizer.mmr_process);
                break;
            case 3:
                print_log(LOG_SUCCESS, L"Malevolent process: %s killed.\n", Sanitizer.mmr_process);
                break;
        }
        
        print_log(LOG_INFO, L"Searching for folder where reside the malware.\n");
        SHGetKnownFolderPath(&FOLDERID_RoamingAppData, 0, NULL, &roaming_appdata);
        swprintf(roaming_appdata_formatted, MAX_PATH, L"%s\\%s\0", roaming_appdata, Sanitizer.mmr_folder);
        
        if(does_folder_exist(roaming_appdata_formatted)){
            print_log(LOG_WARNING, L"Malware folder detected: %s.\n", Sanitizer.mmr_folder);
            print_log(LOG_INFO, L"Attempting to remove: %s.\n", Sanitizer.mmr_folder);
            
            if(remove_folder(roaming_appdata_formatted)){
                print_log(LOG_SUCCESS, L"Malware folder removed.\n");
            }
            else{
                print_log(LOG_WARNING, L"Malware folder could not be removed.\n");
            }
        }
        else{
            print_log(LOG_SUCCESS, L"Malware folder not present.\n");
        }
        
        SHGetKnownFolderPath(&FOLDERID_Startup, 0, NULL, &startup_path);
        swprintf(startup_formatted, MAX_PATH, L"%s\\%s\0", startup_path, Sanitizer.mmr_lnk_helper);
        
        if(does_folder_exist(startup_formatted)){
            print_log(LOG_WARNING, L"LNK startup detected: %s.\n", Sanitizer.mmr_lnk_helper);
            print_log(LOG_INFO, L"Attempting to remove: %s.\n", Sanitizer.mmr_lnk_helper);
            
            if(DeleteFileW(startup_formatted)){
                print_log(LOG_SUCCESS, L"LNK folder removed.\n");
            }
            else{
                print_log(LOG_WARNING, L"LNK startup could not be removed.\n");
            }
        }
        else{
            print_log(LOG_SUCCESS, L"LNK startup not present.\n");
        }

        before_exit();
        CoUninitialize();
        return 0;
    }

    return 0;
}