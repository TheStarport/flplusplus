#include "fontresource.h"
#include "config.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Shlwapi.h>

void fontresource::init(LPCSTR fontDirectory)
{
    // Add font resources
    for (const auto &fontFile : config::get_config().fontfiles) {
        char path[MAX_PATH];

        // Create full path to font file
        strcpy_s(path, sizeof(path), fontDirectory);
        PathAppendA(path, fontFile.c_str());

        AddFontResourceEx(path, FR_PRIVATE, nullptr);
    }
}
