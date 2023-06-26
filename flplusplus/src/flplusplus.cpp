#include "flplusplus.h"
#include "patch.h"
#include "offsets.h"
#include "graphics.h"
#include "config.h"
#include "screenshot.h"
#include "savegame.h"
#include "codec.h"
#include "fontresource.h"
#include "startlocation.h"
#include "log.h"
#include "adoxa/adoxa.h"

#include <windows.h>
#include <shlwapi.h>
#include <vector>


unsigned char thornLoadData[5];
typedef void *(__cdecl *ScriptLoadPtr)(const char*);
ScriptLoadPtr _ThornScriptLoad;

struct LateHookEntry {
    LateHookEntry(flplusplus_cblatehook func, void *data)
        : func(func), data(data)
    {}

    flplusplus_cblatehook func;
    void *data;
};
std::vector<LateHookEntry> lh;

FLPEXPORT void flplusplus_add_latehook(flplusplus_cblatehook hkfunc, void *userData)
{
    lh.emplace_back(hkfunc, userData);
}

void init_config()
{
    config::init_defaults();
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    PathRemoveFileSpecA(path);
    PathAppendA(path, "flplusplus.ini");
    if(PathFileExistsA(path)) {
        logger::writeformat("opening flplusplus.ini at %s", path);
        config::init_from_file(path);
    }

    LPCSTR fontsPath = "../DATA/FONTS/fonts.ini";

    if (PathFileExistsA(fontsPath)) {
        logger::writeformat("opening fonts.ini at %s", fontsPath);
        config::read_font_files(fontsPath);
    }
}

void init_patches(bool version11)
{
    logger::patch_fdump();
    logger::writeline("flplusplus: installing patches");
    init_config();
    graphics::init(version11);
    screenshot::init();
    savegame::init();
    codec::init();
    startlocation::init();
    fontresource::init("../DATA/FONTS");
    adoxa::patch();
    logger::writeline("flplusplus: all patched");
}

void late_init()
{
   for(const auto& entry: lh) {
        entry.func(entry.data);
   }
}


void * __cdecl script_load_hook(const char *script)
{
	late_init();
	patch::undetour((unsigned char*)_ThornScriptLoad, thornLoadData);
	return _ThornScriptLoad(script);
}

void install_latehook(void)
{	
	HMODULE common = GetModuleHandleA("common.dll");
	_ThornScriptLoad = (ScriptLoadPtr)GetProcAddress(common, "?ThornScriptLoad@@YAPAUIScriptEngine@@PBD@Z");
	patch::detour((unsigned char*)_ThornScriptLoad, (void*)script_load_hook, thornLoadData);
}

bool check_version11(void)
{
    auto common = (DWORD) GetModuleHandleA("common.dll");

    BYTE* vibrocentricFontOffset = (BYTE*) (common + F_OF_VIBROCENTRICFONT_V11);
    return (*vibrocentricFontOffset) == 0x56;
}

bool check_nocd(void)
{
    BYTE* videoDialogOffset = (BYTE*)OF_VIDEODIALOG;
    return (*videoDialogOffset) == 0x84 ||
           (*videoDialogOffset) == 0x33;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        if (check_nocd()) {
            init_patches(check_version11());
            install_latehook();
        } else {
            logger::writeline("flplusplus: Couldn't detect No-CD EXE, not installing");
            return FALSE;
        }
    }

    return TRUE;
}


