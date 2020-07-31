#include "flplusplus.h"
#include "patch.h"
#include "offsets.h"
#include "graphics.h"
#include "config.h"
#include "screenshot.h"
#include "savegame.h"
#include "codec.h"
#include "log.h"
#include "adoxa/adoxa.h"

#include <windows.h>
#include <stdlib.h>
#include <shlwapi.h>
#include <vector>


unsigned char *thornLoadData;
typedef void *(__cdecl *ScriptLoadPtr)(const char*);
ScriptLoadPtr _ThornScriptLoad;

struct LateHookEntry {
    flplusplus::cb_latehook func;
    void *data;
};
std::vector<LateHookEntry> lh;

FLPEXPORT void flplusplus::add_latehook(flplusplus::cb_latehook hkfunc, void *userData)
{
    LateHookEntry e;
    e.func = hkfunc;
    e.data = userData;
    lh.push_back(e);
}

void init_config()
{
    config::init_defaults();
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    PathRemoveFileSpecA(path);
    PathAppendA(path, "flplusplus.ini");
    if(PathFileExistsA(path)) {
        log::writeformat("opening flplusplus.ini at %s", path);
        config::init_from_file(path);
    }
}

void init_patches()
{
    log::patch_fdump();
    log::writeline("flplusplus: installing patches");
    init_config();
    graphics::init();
    screenshot::init();
    savegame::init();
    codec::init();
    adoxa::patch();
    log::writeline("flplusplus: all patched");
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
	HMODULE common = GetModuleHandleA("common");
	_ThornScriptLoad = (ScriptLoadPtr)GetProcAddress(common, "?ThornScriptLoad@@YAPAUIScriptEngine@@PBD@Z");
	thornLoadData = (unsigned char*)malloc(5);
	patch::detour((unsigned char*)_ThornScriptLoad, (void*)script_load_hook, thornLoadData);
}



int check_version11(void)
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
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if(check_version11()) {
            init_patches();
            install_latehook();
        } else {
            log::writeline("flplusplus: Version not 1.1, not installing");
        }
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


