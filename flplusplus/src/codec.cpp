#include "codec.h"
#include "offsets.h"
#include "patch.h"

void codec::init()
{
    //Patch out MP3 warnings
    patch::patch_uint8(OF_SOUNDMAN_MP3, 0xC3);
    patch::patch_uint8(OF_SOUNDSTR_MP3, 0xC3);
}
