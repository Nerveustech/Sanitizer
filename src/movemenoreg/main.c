/*
 * MIT License
 *
 * Copyright (c) 2024-2025 Andrea Michael M. Molino
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

#include <ShlObj.h>
#include <Shlwapi.h>
#include <string.h>
#include <Windows.h>

#define LIB_LOG_IMPLEMENTATION

#include "../../include/core/log.h"
#include "../../include/core/apollyon.h"
#include "movemenoreg.h"

/*function prototypes*/
void match_lnk_sig(WIN32_FIND_DATAW file_data);
void find_lnk_remove(void);
void restore_original_file(void);
void before_exit(void);
/*End function prototypes*/


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

        if(wcscmp(wc_lnk_args, movemenoreg.mmr_lnk_argument) == 0){
            warning(L"Malevolent LNK detected: %s\n", file_data.cFileName);

            if(DeleteFileW(file_data.cFileName))
                okay(L"Malevolent LNK removed: %s\n", file_data.cFileName);
            else
                warning(L"Malevolent LNK could not be removed: %s\n", file_data.cFileName);
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
        info(L"There are no files to restore.\n");
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
        okay(L"All files have been restored.\n");
    }
    else{
        warning(L"Some files could not been restored.\n");
    }
    return;
}



int wmain(void)
{
    SetConsoleTitleW(L"Apollyon - Movemenoreg");

    info(L"Initializing Apollyon system...\n");
    info(L"Version: %s\n", L"2025.06.23");

    apollyon_pause(L"Press Enter for start.");
    CoInitialize(NULL);
    
    if(is_usb_mode()){// Usb section
        info(L"Running on USB...\n");
        info(L"Module: %s\n", movemenoreg.mmr_vbs_main);
        
        info(L"Searching for LNK file.\n");
        find_lnk_remove();
        okay(L"Removed All LNK file.\n");

        info(L"Searching for folder where reside the malware.\n");
        if(does_folder_exist(movemenoreg.mmr_folder)){
            warning(L"Malware folder detected: %s.\n", movemenoreg.mmr_folder);
            info(L"Attempting to remove: %s.\n", movemenoreg.mmr_folder);
            
            if(remove_folder(movemenoreg.mmr_folder)){
                okay(L"Malware folder removed.\n");
            }
            warning(L"Malware folder could not be removed.\n");
        }
        else{
            okay(L"Malware folder not present.\n");
        }

        info(L"Attempting to restore the original files it may take some time.\n");
        restore_original_file();


        apollyon_pause(L"Press Enter for exit.");
        CoUninitialize();
        return 0;
    }
    
    else// Desktop section
    {
        PWSTR roaming_appdata = {0}, startup_path = {0};
        WCHAR roaming_appdata_formatted[MAX_PATH] = {0}, startup_formatted[MAX_PATH] = {0};
        int kill_proc_ret = 0;

        info(L"Running on Desktop...\n");
        info(L"Module: %s\n", movemenoreg.mmr_vbs_main);

        info(L"Searching for malevolent process: %s.\n", movemenoreg.mmr_process);
        kill_proc_ret = kill_process(movemenoreg.mmr_process);
        switch (kill_proc_ret)
        {
            case 1:
                okay(L"Malevolent process: %s not present.\n", movemenoreg.mmr_process);
                break;
            case 2:
                warning(L"Malevolent process: %s could not be killed.\n", movemenoreg.mmr_process);
                break;
            case 3:
                okay(L"Malevolent process: %s killed.\n", movemenoreg.mmr_process);
                break;
        }
        
        info(L"Searching for folder where reside the malware.\n");
        SHGetKnownFolderPath(&FOLDERID_RoamingAppData, 0, NULL, &roaming_appdata);
        swprintf(roaming_appdata_formatted, MAX_PATH, L"%s\\%s\0", roaming_appdata, movemenoreg.mmr_folder);
        
        if(does_folder_exist(roaming_appdata_formatted)){
            warning(L"Malware folder detected: %s.\n", movemenoreg.mmr_folder);
            info(L"Attempting to remove: %s.\n", movemenoreg.mmr_folder);
            
            if(remove_folder(roaming_appdata_formatted)){
                okay(L"Malware folder removed.\n");
            }
            else{
                warning(L"Malware folder could not be removed.\n");
            }
        }
        else{
            okay(L"Malware folder not present.\n");
        }
        
        SHGetKnownFolderPath(&FOLDERID_Startup, 0, NULL, &startup_path);
        swprintf(startup_formatted, MAX_PATH, L"%s\\%s\0", startup_path, movemenoreg.mmr_lnk_helper);
        
        if(does_folder_exist(startup_formatted)){
            warning(L"LNK startup detected: %s.\n", movemenoreg.mmr_lnk_helper);
            info(L"Attempting to remove: %s.\n", movemenoreg.mmr_lnk_helper);
            
            if(DeleteFileW(startup_formatted)){
                okay(L"LNK folder removed.\n");
            }
            else{
                warning(L"LNK startup could not be removed.\n");
            }
        }
        else{
            info(L"LNK startup not present.\n");
        }

        apollyon_pause(L"Press Enter for exit.");
        CoUninitialize();
        return 0;
    }
}