#include "thnplayer.h"
#include "savegame.h"
#include "log.h"
#include "patch.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <shlobj.h>
#include <flplusplus.h>
#include <stdlib.h>
#include <commdlg.h>

char *ScriptOverride = NULL;

static bool ParseArguments(LPWSTR* args, int numArgs, LPWSTR* ret)
{
    for(int i = 0; i < numArgs; i++) {
        if(wcscmp(args[i], L"-thn") == 0) {
            if(i + 1 < numArgs && PathFileExistsW(args[i + 1])) {
                *ret = _wcsdup(args[i + 1]);
            }
            return true;
        }
    }
    return false;
}

const unsigned char MenuButtonsRemove[] = {
    0xE8, 0x47, 0xE5, 0xEA, 0xFF, 0x83, 0x7C, 0x24, 0x50,
    0x04, 0x77, 0x11, 0xDB, 0x05, 0x4C, 0x46, 0x57, 0x00,
    0xEB, 0x0D
};

typedef bool (__fastcall *pBExit)(void*);
typedef bool (__fastcall *pBEnter)(void*,int,UINT);
typedef void (__cdecl *pUpdateTime)(double delta);
typedef void(__cdecl *pFlushConsole)(int ident, char* data, DWORD len);

void* FL = (void*)0x668708;
static pBEnter FL_BaseEnter = (pBEnter)0x43b290;
static pBExit FL_BaseExit = (pBExit)0x43b3e0;
static pUpdateTime UpdateTime;
static pFlushConsole FlushConsole = (pFlushConsole)0x46A150;

static unsigned char thornLoadData[5];
typedef void *(__cdecl *ScriptLoadPtr)(const char*);
static ScriptLoadPtr _ThornScriptLoad;

static bool firstLoad = true;

struct Chat {
    UINT Type1;
    UINT Len1;
    UINT Mask;
    UINT Data;
    UINT Type2;
    UINT Len2;
    wchar_t Message[1024];
};

static void PrintText(const wchar_t *text)
{
    Chat chat;
    memset(&chat, 0, sizeof(Chat));
    chat.Type1 = 0x1;
    chat.Len1 = 0x8;
    chat.Mask = 0xFFFFFF00;
    chat.Data = 0xFFFFFF00;
    chat.Type2 = 0x2;
    chat.Len2 = wcslen(text) * 2 + 2;
    wcscpy(chat.Message, text);
    FlushConsole(0, (char*)&chat, chat.Len2 + 24);
}


void * __cdecl OnThornLoad(const char *script)
{
	patch::undetour((unsigned char*)_ThornScriptLoad, thornLoadData);
    logger::writeline(ScriptOverride ? (const char*)ScriptOverride : script);
	void* retval = _ThornScriptLoad(ScriptOverride ? (const char*)ScriptOverride : script);
    patch::detour((unsigned char*)_ThornScriptLoad, (void*)OnThornLoad, thornLoadData);
    if(firstLoad) {
        PrintText(L"THN Player");
        PrintText(L"F5 - Refresh");
        PrintText(L"F9 - Open");
        PrintText(L"Esc - Exit");
        firstLoad = false;
    }
    return retval;
}

static void SceneReload()
{
    FL_BaseExit(FL);
    FL_BaseEnter(FL, 0, 0xA3BC3888); //intro1_base
}

static int reloadFrames = -1;
static void OpenFile()
{
    OPENFILENAME ofn;       // common dialog box structure
    char szFile[260];       // buffer for file name

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "THN\0*.THN;*.LUA\0All\0*.*\0";
    ofn.nFilterIndex = 0;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    // Display the Open dialog box. 
    if (GetOpenFileName(&ofn)==TRUE) {
        logger::writeline("Opening");
        logger::writeline(szFile);
        ScriptOverride = _strdup(szFile);
        reloadFrames = 4;
    }
}

unsigned char updateData[5];

static void Update(const double delta)
{
    if((GetAsyncKeyState(VK_F5) & 1)) {
        SceneReload();
    }
    if((GetAsyncKeyState(VK_F9) & 1)) {
        OpenFile();
    }
    // Skip hitching
    if(reloadFrames > 0) {
        reloadFrames--;
    } else if(reloadFrames == 0) {
        reloadFrames = -1;
        SceneReload();
    }
    patch::undetour((unsigned char*)UpdateTime, updateData);
    UpdateTime(delta);
    patch::detour((unsigned char*)UpdateTime, (void*)Update, updateData);
}

void thnplayer::init()
{
    int numArgs;
    LPWSTR *args = CommandLineToArgvW(GetCommandLineW(), &numArgs);
    LPWSTR thnscript = NULL;
    if(ParseArguments(args, numArgs, &thnscript)) {
        if(thnscript) {
            char buffer[500];
            wcstombs(buffer, thnscript, 500);
            free((void*)thnscript);
            ScriptOverride = _strdup(buffer);
        }
        DWORD FL = (DWORD)(GetModuleHandleA("freelancer.exe"));
        //Remove all menu buttons
        patch::patch_bytes(FL + 0x174634, (void*)MenuButtonsRemove, 20);
        patch::patch_uint16(FL + 0x1746CA, 0x0);
        patch::patch_uint16(FL + 0x174707, 0x0);
        patch::patch_uint16(FL + 0x174744, 0x0);
        patch::patch_uint16(FL + 0x174781, 0x0);
        patch::patch_uint16(FL + 0x1747BE, 0x0);
        patch::patch_bytes(FL + 0x1E23DC, (void*)"null", 5); //Disable ui_motion_swish
        //Remove version text
        patch::patch_uint32(FL + 0x16DDEC, 0x1);
        patch::patch_uint32(FL + 0x174890, 0x1);
        //Remove logo
        patch::patch_uint32(FL + 0x1E266C, 0x0);
        //Persistent text
        patch::patch_uint32(FL + 0x0691CA, 0x7FFFFFFE);
        //Hook update
        HMODULE common = GetModuleHandleA("common");
        UpdateTime = (pUpdateTime)GetProcAddress(common, "?UpdateGlobalTime@Timing@@YAXN@Z");
        patch::detour((unsigned char*)UpdateTime, (void*)Update, updateData);
        //Permanent hook load function
	    _ThornScriptLoad = (ScriptLoadPtr)GetProcAddress(common, "?ThornScriptLoad@@YAPAUIScriptEngine@@PBD@Z");
	    patch::detour((unsigned char*)_ThornScriptLoad, (void*)OnThornLoad, thornLoadData);
    }
    LocalFree(args);
}
