#include "savegame.h"
#include "config.h"
#include "patch.h"
#include "log.h"

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <cstring>
#include <shlwapi.h>
#include <shlobj.h>
#include <io.h>
#include <direct.h>

void HandleUserDataPathFail(char * const outputBuffer)
{
    *outputBuffer = '\0';
    logger::writeline("flplusplus: failed to access the saves directory. Freelancer may not be able to properly store save files.");
}

bool UserDataPath(char * const outputBuffer)
{
    char path[MAX_PATH];
    if(config::get_config().saveindirectory) {
        GetModuleFileNameA(NULL, path, MAX_PATH);
        PathRemoveFileSpecA(path);
        PathAppendA(path, "..\\SAVE");
    } else {
        if (SHGetFolderPathA(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, 0, path) != S_OK) {
            HandleUserDataPathFail(outputBuffer);
            return false;
        }

        PathAppendA(path, "My Games");

        if (_access(path, 0) != 0) {
            if (_mkdir(path) != 0) {
                HandleUserDataPathFail(outputBuffer);
                return false;
            }
        }

        PathAppendA(path, config::get_config().savefoldername.c_str());
    }

    if (_access(path, 0) != 0) {
        if (_mkdir(path) != 0) {
            HandleUserDataPathFail(outputBuffer);
            return false;
        }
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
