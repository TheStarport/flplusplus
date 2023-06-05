#include "codec.h"
#include "offsets.h"
#include "patch.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

void codec::init()
{
    auto soundManager = (DWORD) GetModuleHandleA("soundmanager.dll");
    auto soundStreamer = (DWORD) GetModuleHandleA("soundstreamer.dll");

    //Patch out MP3 warnings
    patch::patch_uint8(soundManager + F_OF_SOUNDMAN_MP3, 0xC3);
    patch::patch_uint8(soundStreamer + F_OF_SOUNDSTR_MP3, 0xC3);
}
