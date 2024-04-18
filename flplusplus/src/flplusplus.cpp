#include "flplusplus.h"
#include "patch.h"
#include "offsets.h"
#include "graphics.h"
#include "config.h"
#include "screenshot.h"
#include "savegame.h"
#include "fontresource.h"
#include "startlocation.h"
#include "log.h"
#include "thnplayer.h"
#include <windows.h>
#include <shlwapi.h>
#include <vector>
#include "consolewindow.h"
#include "shippreviewscroll.h"

static unsigned char thornLoadData[5];
typedef void *(__cdecl *ScriptLoadPtr)(const char*);
static ScriptLoadPtr _ThornScriptLoad;

char dataPath[MAX_PATH];

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

    // Get EXE folder path
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    PathRemoveFileSpecA(exePath);

    // Get flplusplus.ini path
    char configPath[MAX_PATH];
    strcpy_s(configPath, sizeof(configPath), exePath);
    PathAppendA(configPath, "flplusplus.ini");

    if(PathFileExistsA(configPath)) {
        logger::writeformat("opening flplusplus.ini at %s", configPath);
        config::init_from_file(configPath);
    }

    // Get DATA folder path
    strcpy_s(dataPath, sizeof(dataPath), exePath);
    PathRemoveFileSpecA(dataPath);
    PathAppendA(dataPath, "DATA\\");

    // Get fonts.ini path
    char fontsIniPath[MAX_PATH];
    strcpy_s(fontsIniPath, sizeof(fontsIniPath), dataPath);
    PathAppendA(fontsIniPath, "FONTS\\fonts.ini");

    if (PathFileExistsA(fontsIniPath)) {
        logger::writeformat("opening fonts.ini at %s", fontsIniPath);
        config::read_font_files(fontsIniPath);
    }
}

void init_patches(bool version11)
{
    logger::patch_fdump();
    init_config();
    if(config::get_config().logtoconsole)
        RedirectIOToConsole();
    logger::writeline("flplusplus: installing patches");
    graphics::init(version11);
    screenshot::init();
    savegame::init();
    startlocation::init();
    fontresource::init(dataPath);
    thnplayer::init();
    shippreviewscroll::init();
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
	//HMODULE common = GetModuleHandleA("common.dll");
	//_ThornScriptLoad = (ScriptLoadPtr)GetProcAddress(common, "?ThornScriptLoad@@YAPAUIScriptEngine@@PBD@Z");
	//patch::detour((unsigned char*)_ThornScriptLoad, (void*)script_load_hook, thornLoadData);
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


