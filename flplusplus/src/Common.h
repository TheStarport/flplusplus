#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define Import __declspec(dllimport)

extern "C" class Import INI_Reader
{
public:
    INI_Reader();
    ~INI_Reader();

    bool open(LPCSTR, bool);
    bool read_header(void);
    bool is_header(LPCSTR);
    bool read_value(void);
    bool is_value(LPCSTR);
    bool get_value_bool(UINT);
    int get_value_int(UINT);
    LPCSTR get_value_string(UINT);
    void close(void);

private:
    BYTE data[0x1568];
};