#include "savegame.h"
#include "config.h"
#include "patch.h"

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <cstring>
#include <shlwapi.h>
#include <shlobj.h>

bool UserDataPath(char *outputBuffer)
{
    char path[MAX_PATH];
    if(config::get_config().saveindirectory) {
        GetModuleFileNameA(NULL, path, MAX_PATH);
        PathRemoveFileSpecA(path);
        PathAppendA(path, "..\\SAVE");
    } else {
        SHGetFolderPathA(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, 0, path);
        PathAppendA(path, "My Games");
        PathAppendA(path, config::get_config().savefoldername.c_str());
    }
    strcpy(outputBuffer, path);
    return true;
}

void savegame::init()
{
    HMODULE common = GetModuleHandleA("common.dll");
    auto *origFunc = (unsigned char*)GetProcAddress(common, "?GetUserDataPath@@YA_NQAD@Z");
    unsigned char buffer[5];
    patch::detour(origFunc, (void*)UserDataPath, buffer);
}

void savegame::get_save_folder(char *buffer)
{
    UserDataPath(buffer);
}
