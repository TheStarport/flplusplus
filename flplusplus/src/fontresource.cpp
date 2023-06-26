#include "fontresource.h"
#include "config.h"
#include "log.h"

#include <shlwapi.h>

void fontresource::init(LPCSTR fontDirectory)
{
    // Add font resources
    for (const auto &fontFile : config::get_config().fontfiles) {
        char path[MAX_PATH];

        // Create full path to font file
        strcpy_s(path, sizeof(path), fontDirectory);
        PathAppendA(path, fontFile.c_str());

        if (!PathFileExists(path)) {
            logger::writeformat("path to font %s does not exist (%s)", fontFile.c_str(), path);
            continue;
        }

        if (AddFontResourceEx(path, FR_PRIVATE, nullptr))
            logger::writeformat("successfully added font %s", fontFile.c_str());
        else
            logger::writeformat("error adding font %s", fontFile.c_str());
    }
}
