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

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

///
///Macro section
///

#if defined(WIN64) || defined(_WIN64) || defined(__MINGW64__)
    #define WINDOWS_64
#elif defined(WIN32) || defined(_WIN32) || defined(__MINGW32__)
    #define WINDOWS_32
#endif

#if defined(WINDOWS_64) || defined(WINDOWS_32)
    #define WINDOWS
#endif

typedef enum {
    Success = 0,
    Error,
    Info,
    Warning,
    Custom
}LogType;

///
///End Macro section
///

#if defined(__linux__)

    #define LOG_COLOR_RESET  "\x1B[0m"
    #define LOG_COLOR_RED    "\x1B[1;31m"
    #define LOG_COLOR_GREEN  "\x1B[1;32m"
    #define LOG_COLOR_YELLOW "\x1B[1;33m"
    #define LOG_COLOR_BLUE   "\x1B[1;34m"
    #define LOG_COLOR_ORANGE "\x1B[1;93m"

    void info(const char* format, ...);
    void debug(const char* format, ...);
    void okay(const char* format, ...);
    void warning(const char* format, ...);
    void error(const char* format, ...);
    void critical(const char* format, ...);

    void log_file(LogType type, const char* file,  const char* format, ...);

#elif defined(WINDOWS)
/*
    #define NOGDICAPMASKS
    #define NOVIRTUALKEYCODES
    #define NOWINMESSAGES
    #define NOWINSTYLES
    #define NOSYSMETRICS
    #define NOMENUS
    #define NOICONS
    #define NOKEYSTATES
    #define NOSYSCOMMANDS
    #define NORASTEROPS
    #define NOSHOWWINDOW
    #define OEMRESOURCE
    #define NOATOM
    #define NOCLIPBOARD
    #define NOCTLMGR
    #define NODRAWTEXT
    #define NOGDI
    #define NOKERNEL
    #define NOUSER
    #define NONLS
    #define NOMB
    #define NOMEMMGR
    #define NOMETAFILE
    #define NOMSG
    #define NOOPENFILE
    #define NOSCROLL
    #define NOSERVICE
    #define NOSOUND
    #define NOTEXTMETRIC
    #define NOWH
    #define NOWINOFFSETS
    #define NOCOMM
    #define NOHELP
    #define NOPROFILER
    #define NODEFERWINDOWPOS
    #define NOMCX
*/  
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN 
    #include <Windows.h>


    #define LOG_COLOR_RESET  7
    #define LOG_COLOR_RED    4
    #define LOG_COLOR_GREEN  2
    #define LOG_COLOR_YELLOW 6
    #define LOG_COLOR_BLUE   1
    #define LOG_COLOR_ACQUA FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY

    void info(CONST WCHAR* format, ...);
    void debug(CONST WCHAR* format, ...);
    void okay(CONST WCHAR* format, ...);
    void warning(CONST WCHAR* format, ...);
    void error(CONST WCHAR* format, ...);
    void critical(CONST WCHAR* format, ...);

    void log_file(LogType type, CONST WCHAR* file, CONST WCHAR* format, ...);
#endif

#ifdef LIB_LOG_IMPLEMENTATION

#ifdef __linux__
void info(const char* format, ...)
{
    if(format == NULL){
        error("%s: format is NULL!", __func__);
        return;
    }

    va_list args;
    va_start(args, format);

    fprintf(stdout, "%s[INFO]%s ", LOG_COLOR_BLUE, LOG_COLOR_RESET);
    vfprintf(stdout, format, args);
    va_end(args);
}

void debug(const char* format, ...)
{
    if(format == NULL){
        error("%s: format is NULL!", __func__);
        return;
    }

    va_list args;
    va_start(args, format);

    fprintf(stdout, "%s[DEBUG]%s ", LOG_COLOR_BLUE, LOG_COLOR_RESET);
    vfprintf(stdout, format, args);
    va_end(args);
}

void okay(const char* format, ...)
{
    if(format == NULL){
        error("%s: format is NULL!", __func__);
        return;
    }

    va_list args;
    va_start(args, format);

    fprintf(stdout, "%s[SUCCESS]%s ", LOG_COLOR_GREEN, LOG_COLOR_RESET);
    vfprintf(stdout, format, args);
    va_end(args);
}

void warning(const char* format, ...)
{
    if(format == NULL){
        error("%s: format is NULL!", __func__);
        return;
    }

    va_list args;
    va_start(args, format);

    fprintf(stdout, "%s[WARNING]%s ", LOG_COLOR_YELLOW, LOG_COLOR_RESET);
    vfprintf(stdout, format, args);
    va_end(args);
}

void error(const char* format, ...)
{
    if(format == NULL){
        fprintf(stdout, "%s[ERROR]%s %s format is NULL!", LOG_COLOR_RED, LOG_COLOR_RESET, __func__);
        return;
    }
    
    va_list args;
    va_start(args, format);

    fprintf(stdout, "%s[ERROR]%s ", LOG_COLOR_RED, LOG_COLOR_RESET);
    vfprintf(stdout, format, args);
    va_end(args);
}

void critical(const char* format, ...)
{
    if(format == NULL){
        error("%s: format is NULL!", __func__);
        return;
    }
    
    va_list args;
    va_start(args, format);

    fprintf(stdout, "%s[CRITICAL]%s ", LOG_COLOR_RED, LOG_COLOR_RESET);
    vfprintf(stdout, format, args);
    va_end(args);
    abort();
}

void log_file(LogType type, const char* file,  const char* format, ...)
{
    FILE* file_to_open = fopen(file, "a");

    if(file_to_open == NULL){
        error("Could not open: %s for log\n", file);
        return;
    }

    va_list args;
    va_start(args, format);

    switch(type){
        case Success:
            fprintf(file_to_open, "[SUCCESS] ");
            vfprintf(file_to_open, format, args);
            break;
    
        case Error:
            fprintf(file_to_open, "[ERROR] ");
            vfprintf(file_to_open, format, args);
            break;
        
        case Info:
            fprintf(file_to_open, "[INFO] ");
            vfprintf(file_to_open, format, args);
            break;
        
        case Warning:
            fprintf(file_to_open, "[WARNING] ");
            vfprintf(file_to_open, format, args);
            break;
        
        case Custom:
            vfprintf(file_to_open, format, args);
            break;
        
        default:
            __builtin_unreachable();
    }

    va_end(args);
    fclose(file_to_open);
    
    return;
}

#endif

#ifdef WINDOWS
void info(CONST WCHAR* format, ...)
{
    if(format == NULL){
        error(L"%s: format is NULL!", __func__);
        return;
    }

    va_list args;
    va_start(args, format);
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsole, LOG_COLOR_ACQUA);
    fwprintf(stdout, L"[INFO] ");
    SetConsoleTextAttribute(hConsole, LOG_COLOR_RESET);
    vfwprintf(stdout, format, args);
    
    va_end(args);
}

void debug(CONST WCHAR* format, ...)
{
    if(format == NULL){
        error(L"%s: format is NULL!", __func__);
        return;
    }

    va_list args;
    va_start(args, format);
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsole, LOG_COLOR_ACQUA);
    fwprintf(stdout, L"[DEBUG] ");
    SetConsoleTextAttribute(hConsole, LOG_COLOR_RESET);
    vfwprintf(stdout, format, args);
    
    va_end(args);
}

void okay(CONST WCHAR* format, ...)
{
    if(format == NULL){
        error(L"%s: format is NULL!", __func__);
        return;
    }

    va_list args;
    va_start(args, format);
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsole, LOG_COLOR_GREEN);
    fwprintf(stdout, L"[SUCCESS] ");
    SetConsoleTextAttribute(hConsole, LOG_COLOR_RESET);
    vfwprintf(stdout, format, args);
    
    va_end(args);
}

void warning(CONST WCHAR* format, ...)
{
    if(format == NULL){
        error(L"%s: format is NULL!", __func__);
        return;
    }

    va_list args;
    va_start(args, format);
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsole, LOG_COLOR_YELLOW);
    fwprintf(stdout, L"[WARNING] ");
    SetConsoleTextAttribute(hConsole, LOG_COLOR_RESET);
    vfwprintf(stdout, format, args);
    
    va_end(args);
}

void error(CONST WCHAR* format, ...)
{
    if(format == NULL){
        error(L"%s: format is NULL!", __func__);
        return;
    }

    va_list args;
    va_start(args, format);
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsole, LOG_COLOR_RED);
    fwprintf(stdout, L"[ERROR] ");
    SetConsoleTextAttribute(hConsole, LOG_COLOR_RESET);
    vfwprintf(stdout, format, args);
    
    va_end(args);
}

void critical(CONST WCHAR* format, ...)
{
    if(format == NULL){
        error(L"%s: format is NULL!", __func__);
        return;
    }

    va_list args;
    va_start(args, format);
    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleTextAttribute(hConsole, LOG_COLOR_RED);
    fwprintf(stdout, L"[CRITICAL] ");
    SetConsoleTextAttribute(hConsole, LOG_COLOR_RESET);
    vfwprintf(stdout, format, args);
    
    va_end(args);
    abort();
}


void log_file(LogType type, CONST WCHAR* file, CONST WCHAR* format, ...)
{
    FILE* file_to_open = NULL; 
    _wfopen_s(&file_to_open, file, L"a");

    if(file_to_open == NULL){
        error(L"Could not open: %s for log\n", file);
        return;
    }

    va_list args;
    va_start(args, format);

    switch(type){
        case Success:
            fwprintf(file_to_open, L"[SUCCESS] ");
            vfwprintf(file_to_open, format, args);
            break;
    
        case Error:
            fwprintf(file_to_open, L"[ERROR] ");
            vfwprintf(file_to_open, format, args);
            break;
        
        case Info:
            fwprintf(file_to_open, L"[INFO] ");
            vfwprintf(file_to_open, format, args);
            break;
        
        case Warning:
            fwprintf(file_to_open, L"[WARNING] ");
            vfwprintf(file_to_open, format, args);
            break;
        
        case Custom:
            vfwprintf(file_to_open, format, args);
            break;
        
        default:
            __assume(0);
    }

    va_end(args);
    fclose(file_to_open);
    
    return;
}
#endif

#endif /*LIB_PRINT_IMPLEMENTATION*/