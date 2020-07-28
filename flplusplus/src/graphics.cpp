#include "graphics.h"
#include "flplusplus.h"
#include "patch.h"
#include "offsets.h"
#include "config.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int patch_lodranges(int scale)
{
    if(scale < 0 || scale > 8) return 0;
    
    
    if(scale == 0) {
        //1x lods
        unsigned char og1[5] = { 0x83, 0xFE, 0x08, 0x7D, 0x16 };
        patch::patch_bytes(OF_LODS_P1, (void*)og1, 5);
        patch::patch_uint16(OF_LODS_P2, 0x9090);
        patch::patch_uint16(OF_LODS_P3, 0x9090);
        patch::patch_x3(OF_LODS_P4, 0x90, 0x90, 0x90);
        //original draw distances
        patch::patch_float(OF_REN_DIST0, 10000.0);
        patch::patch_float(OF_REN_DIST1, 20000.0);
        return 1;
    }
    //distances
    float ren_dist0 = 10000.0;
    float ren_dist1 = 20000.0; //This one doesn't seem to change for now
    //lod scale
    unsigned char patch1[5] = { 0xE8, 0x7B, 0xFF, 0xFF, 0xFF };
    patch::patch_bytes(OF_LODS_P1, (void*)patch1, 5);
    patch::patch_uint16(OF_LODS_P2, 0x0DD8);
    patch::patch_uint16(OF_LODS_P3, 0xC300);
    switch(scale) {
        case 1:
            patch::patch_x3(OF_LODS_P4, 0x4B, 0x1A, 0x47); //1.5x
            ren_dist0 *= 1.5;
            break;
        case 2:
            patch::patch_x3(OF_LODS_P4, 0xB4, 0x55, 0x5D); //2.0x
            ren_dist0 *= 2;
            break;
        case 3:
            patch::patch_x3(OF_LODS_P4, 0x28, 0x4F, 0x5D); //3.0x
            ren_dist0 *= 3;
            break;
        case 4:
            patch::patch_x3(OF_LODS_P4, 0x9C, 0xFC, 0x5C); //4.0x
            ren_dist0 *= 4;
            break;
        case 5:
            patch::patch_x3(OF_LODS_P4, 0x64, 0x84, 0x5D); //5.0x
            ren_dist0 *= 5;
            break;
        case 6:
            patch::patch_x3(OF_LODS_P4, 0x08, 0x4F, 0x5D); //6.0x
            ren_dist0 *= 6;
            break;
        case 7:
            patch::patch_x3(OF_LODS_P4, 0x50, 0x88, 0x5D); //7.0x
            ren_dist0 *= 7;
            break;
        case 8:
            patch::patch_x3(OF_LODS_P4, 0x54, 0x23, 0x44); //8.0x
            ren_dist0 *= 8;
            break;
    }
    patch::patch_float(OF_REN_DIST0, ren_dist0);
    patch::patch_float(OF_REN_DIST1, ren_dist1);
    return 1;
}

void graphics::init()
{
    patch::patch_uint8(OF_VIDEODIALOG, 0x33); //disable unsupported video dialog
    patch::patch_uint16(OF_MAXTEXSIZE, 0x2000); //texture size bug fix
    //replace "Vibrocentric" string
    //FL tries to load this font over Agency FB, screws up UI if it finds it
    //if you have a font named '\b' you have big problems
    const char *garbageFont = "\b\0"; 
    HMODULE common = GetModuleHandleA("common");
    patch::patch_bytes((DWORD)common + 0x143DC0, (void*)garbageFont, 2);
    //lod 0
    patch_lodranges(config::get_config().lodscale);
}
