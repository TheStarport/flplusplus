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

void HandleUserDataPathFail(char * const outputBuffer, char * failedSavesDirectory)
{
    logger::writeformat("flplusplus: failed to access the saves directory for reading and writing (%s). Freelancer may not be able to properly load and store save files.", failedSavesDirectory);
    *outputBuffer = '\0';
}

void GetSavesInDirectoryPath(char * path)
{
    GetModuleFileNameA(NULL, path, MAX_PATH);
    PathRemoveFileSpecA(path);
    PathAppendA(path, "..\\SAVE");
}

bool TryGetMyGamesPath(char * path)
{
    if (SHGetFolderPathA(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, 0, path) != S_OK) {
        return false;
    }

    PathAppendA(path, "My Games");

    if (_access(path, 6) != 0) {
        if (_mkdir(path) != 0) {
            return false;
        }
    }

    PathAppendA(path, config::get_config().savefoldername.c_str());

    if (_access(path, 6) != 0) {
        if (_mkdir(path) != 0) {
            return false;
        }
    }

    return true;
}

bool UserDataPath(char * const outputBuffer)
{
    char path[MAX_PATH];

    if(config::get_config().saveindirectory) {
        GetSavesInDirectoryPath(path);
    } else {
        if (!TryGetMyGamesPath(path)) {
            HandleUserDataPathFail(outputBuffer, path);

            logger::writeline("flplusplus: saveindirectory option not set but trying to access the root SAVE directory regardless (fallback).");

            // Fallback
            GetSavesInDirectoryPath(path);
        } else {
            strncpy(outputBuffer, path, MAX_PATH);
            return true;
        }
    }

    if (_access(path, 6) != 0) {
        if (_mkdir(path) != 0) {
            HandleUserDataPathFail(outputBuffer, path);
            return false;
        }
    }

    strncpy(outputBuffer, path, MAX_PATH);
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
