#include "log.h"
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <ctime>
#include <stdarg.h>

static bool linked = false;

typedef DWORD (*pFDUMP)(DWORD, const char *, ...);
static pFDUMP *FDUMP;
static pFDUMP fdump_original;

static void do_linking()
{
    if(linked) return;
    linked = true;
    
    HMODULE dacom = GetModuleHandleA("dacom.dll");
    FDUMP = (pFDUMP*)GetProcAddress(dacom, "FDUMP");
}

void log::writeline(const char *line)
{
    do_linking();
    (*FDUMP)(1048578, "%s\n", line);
}

static DWORD fdump_timestamped(DWORD unk, const char *fmt, ...)
{
    char buffer[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, 4096, fmt, args);
    va_end(args);
    std::time_t rawtime;
	std::tm* timeinfo;
    char timestamp[100];
	std::time(&rawtime);
	timeinfo = std::localtime(&rawtime);
	std::strftime(timestamp, 80, "%Y%m%d-%H:%M:%S", timeinfo);
    return fdump_original(unk, "[%s] %s", timestamp, buffer);
}

void log::patch_fdump()
{
    do_linking();
    fdump_original = *FDUMP;
    *FDUMP = (pFDUMP)fdump_timestamped;
}

void log::writeformat(const char *fmt, ...)
{
    char buffer[4096];
    va_list args;
    va_start (args, fmt);
    vsnprintf(buffer, 4096, fmt, args);
    va_end (args);
    log::writeline(buffer);
}
