#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if defined(_MSC_VER)
#define Import __declspec(dllimport)
#else
#define Import
#endif

extern "C" class Import INI_Reader
{
public:
    INI_Reader();
    ~INI_Reader();

    bool open(LPCSTR path, bool throwExceptionOnFail);
    bool read_header();
    bool is_header(LPCSTR header);
    bool read_value();
    bool is_value(LPCSTR value);
    bool get_value_bool(UINT index);
    int get_value_int(UINT index);
    float get_value_float(UINT index);
    LPCSTR get_value_string(UINT index);
    void close();

private:
    BYTE data[0x1568];
};
