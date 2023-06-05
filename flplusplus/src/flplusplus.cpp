#include "flplusplus.h"
#include "patch.h"
#include "offsets.h"
#include "graphics.h"
#include "config.h"
#include "screenshot.h"
#include "savegame.h"
#include "codec.h"
#include "startlocation.h"
#include "log.h"
#include "adoxa/adoxa.h"

#include <windows.h>
#include <cstdlib>
#include <shlwapi.h>
#include <vector>


unsigned char *thornLoadData;
typedef void *(__cdecl *ScriptLoadPtr)(const char*);
ScriptLoadPtr _ThornScriptLoad;

struct LateHookEntry {
    flplusplus_cblatehook func;
    void *data;
};
std::vector<LateHookEntry> lh;

FLPEXPORT void flplusplus_add_latehook(flplusplus_cblatehook hkfunc, void *userData)
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
        logger::writeformat("opening flplusplus.ini at %s", path);
        config::init_from_file(path);
    }
}

void init_patches()
{
    logger::patch_fdump();
    logger::writeline("flplusplus: installing patches");
    init_config();
    graphics::init();
    screenshot::init();
    savegame::init();
    codec::init();
    startlocation::init();
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
	HMODULE common = GetModuleHandleA("common");
	_ThornScriptLoad = (ScriptLoadPtr)GetProcAddress(common, "?ThornScriptLoad@@YAPAUIScriptEngine@@PBD@Z");
	thornLoadData = (unsigned char*)malloc(5);
	patch::detour((unsigned char*)_ThornScriptLoad, (void*)script_load_hook, thornLoadData);
}



int check_nocd(void)
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
        if(check_nocd()) {
            init_patches();
            install_latehook();
        } else {
            logger::writeline("flplusplus: Couldn't detect No-CD EXE, not installing");
            return FALSE;
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


