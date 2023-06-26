#include "fontresource.h"
#include "config.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Shlwapi.h>

void fontresource::init(const std::string &fontDirectory)
{
    // Add font resources
    for (const auto &fontFile : config::get_config().fontfiles) {
        char path[MAX_PATH];

        // Create full path to font file
        strcpy_s(path, MAX_PATH, fontDirectory.c_str());
        PathAppendA(path, fontFile.c_str());

        AddFontResourceEx(path, FR_PRIVATE, nullptr);
    }
}
