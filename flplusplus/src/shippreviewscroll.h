#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace shippreviewscroll
{
    enum ScrollDirection : int
    {
        up = 1,
        down = -1
    };

    struct ShipPreviewWindow
    {
        BYTE x00[0x3EC];
        float zoomLevel;
    };

    void init();
}
