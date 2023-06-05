#include "startlocation.h"
#include "offsets.h"
#include "patch.h"
#include "config.h"

void startlocation::init()
{
    if (config::get_config().removestartlocationwarning) {
        //Patch out "Failed to get start location" warning
        patch::patch_uint8(OF_STARTLOCATION, 0xEB);
    }
}
