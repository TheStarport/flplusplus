// Code for wrapping the FL INI_Reader class in a GCC compatible class
#ifndef _MSC_VER

#include "Common.h"
#include <stdlib.h>

static int Loaded = 0;

// Function Defs
// Second int parameter = dummy
typedef void (__fastcall *pIniVoid)(void*, int); //create, destroy, close

typedef bool (__fastcall *pIniOpen)(void*, int, LPCSTR, bool);
typedef bool (__fastcall *pIniRead)(void*, int); //read_header, read_value
typedef bool (__fastcall *pIniIs)(void*, int, LPCSTR); //is_header, is_value

typedef bool (__fastcall *pIniGetValueBool)(void*, int, UINT); 
typedef int (__fastcall *pIniGetValueInt)(void*, int, UINT);
typedef float (__fastcall *pIniGetValueFloat)(void*, int, UINT);
typedef LPCSTR (__fastcall *pIniGetValueString)(void*, int, UINT);
//Functions

static pIniVoid IniCreate;
static pIniVoid IniDestroy;

static pIniOpen IniOpen;
static pIniRead IniReadHeader;
static pIniIs IniIsHeader;
static pIniRead IniReadValue;
static pIniIs IniIsValue;
static pIniGetValueBool IniGetValueBool;
static pIniGetValueInt IniGetValueInt;
static pIniGetValueFloat IniGetValueFloat;
static pIniGetValueString IniGetValueString;
static pIniVoid IniClose;


static void LoadFunctions()
{
    if(Loaded) return;
    
    HMODULE common = LoadLibraryA("common.dll");
    IniCreate = (pIniVoid)GetProcAddress(common, "??0INI_Reader@@QAE@XZ");
    IniDestroy = (pIniVoid)GetProcAddress(common, "??1INI_Reader@@QAE@XZ");
    IniOpen = (pIniOpen)GetProcAddress(common, "?open@INI_Reader@@QAE_NPBD_N@Z");
    IniReadHeader = (pIniRead)GetProcAddress(common, "?read_header@INI_Reader@@QAE_NXZ");
    IniIsHeader = (pIniIs)GetProcAddress(common, "?is_header@INI_Reader@@QAE_NPBD@Z");
    IniReadValue = (pIniRead)GetProcAddress(common, "?read_value@INI_Reader@@QAE_NXZ");
    IniIsValue = (pIniIs)GetProcAddress(common, "?is_value@INI_Reader@@QAE_NPBD@Z");
    IniGetValueBool = (pIniGetValueBool)GetProcAddress(common, "?get_value_bool@INI_Reader@@QAE_NI@Z");
    IniGetValueInt = (pIniGetValueInt)GetProcAddress(common, "?get_value_int@INI_Reader@@QAEHI@Z");
    IniGetValueFloat = (pIniGetValueFloat)GetProcAddress(common, "?get_value_float@INI_Reader@@QAEMI@Z");
    IniGetValueString = (pIniGetValueString)GetProcAddress(common, "?get_value_string@INI_Reader@@QAEPBDI@Z");
    IniClose = (pIniVoid)GetProcAddress(common, "?close@INI_Reader@@QAEXXZ");
    Loaded = 1;
}

#define SELF (*(void**)&this->data[0])

INI_Reader::~INI_Reader()
{
    IniDestroy(SELF, 0);
    free(SELF);
}

INI_Reader::INI_Reader()
{
    LoadFunctions();
    SELF = (void*)malloc(0x1580);
    IniCreate(SELF, 0);
}

bool INI_Reader::open(LPCSTR path, bool throwExceptionOnFail)
{
    return IniOpen(SELF, 0, path, throwExceptionOnFail);
}

bool INI_Reader::read_header()
{
    return IniReadHeader(SELF, 0);    
}

bool INI_Reader::is_header(LPCSTR header)
{
    return IniIsHeader(SELF, 0, header);
}

bool INI_Reader::read_value()
{
    return IniReadValue(SELF, 0);    
}

bool INI_Reader::is_value(LPCSTR value)
{
    return IniIsValue(SELF, 0, value);
}

bool INI_Reader::get_value_bool(UINT index)
{
    return IniGetValueBool(SELF, 0, index);
}

int INI_Reader::get_value_int(UINT index)
{
    return IniGetValueInt(SELF, 0, index);
}

float INI_Reader::get_value_float(UINT index)
{
    return IniGetValueFloat(SELF, 0, index);
}

LPCSTR INI_Reader::get_value_string(UINT index)
{
    return IniGetValueString(SELF, 0, index);
}

void INI_Reader::close()
{
    IniClose(SELF, 0);
}
#endif
