#include "Common.h"
#include "jumptable.h"
#include <stdlib.h>

const char* INI_Reader_JumpTable[] = {
    "??0INI_Reader@@QAE@XZ", 0,
    "??1INI_Reader@@QAE@XZ", 0,
    "?open@INI_Reader@@QAE_NPBD_N@Z", 0,
    "?read_header@INI_Reader@@QAE_NXZ", 0,
    "?is_header@INI_Reader@@QAE_NPBD@Z", 0,
    "?read_value@INI_Reader@@QAE_NXZ", 0,
    "?is_value@INI_Reader@@QAE_NPBD@Z", 0,
    "?get_value_bool@INI_Reader@@QAE_NI@Z", 0,
    "?get_value_int@INI_Reader@@QAEHI@Z", 0,
    "?get_value_float@INI_Reader@@QAEMI@Z", 0,
    "?get_value_string@INI_Reader@@QAEPBDI@Z", 0,
    "?close@INI_Reader@@QAEXXZ", 0
};
JUMPTABLE_INIT("common.dll", INI_Reader_JumpTable)

#define FUNC( def, index ) JUMPTABLE(def, INI_Reader_JumpTable, index)
FUNC(INI_Reader::INI_Reader(), 0)
FUNC(INI_Reader::~INI_Reader(), 1)
FUNC(bool INI_Reader::open(LPCSTR path, bool throwExceptionOnFail), 2)
FUNC(bool INI_Reader::read_header(), 3)
FUNC(bool INI_Reader::is_header(LPCSTR header), 4)
FUNC(bool INI_Reader::read_value(), 5)
FUNC(bool INI_Reader::is_value(LPCSTR value), 6)
FUNC(bool INI_Reader::get_value_bool(UINT index), 7)
FUNC(int INI_Reader::get_value_int(UINT index), 8)
FUNC(float INI_Reader::get_value_float(UINT index), 9)
FUNC(LPCSTR INI_Reader::get_value_string(UINT index), 10)
FUNC(void INI_Reader::close(), 11)
