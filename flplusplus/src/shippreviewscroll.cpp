#include "shippreviewscroll.h"
#include "offsets.h"
#include "patch.h"

using namespace shippreviewscroll;

bool __fastcall ShipPreviewWindowScroll(ShipPreviewWindow* window, PVOID _edx, ScrollDirection dir)
{
    if (dir == ScrollDirection::up)
        window->zoomLevel += 2;
    else if (dir == ScrollDirection::down)
        window->zoomLevel -= 2;

    // The scroll function should just return false
    return false;
}

void shippreviewscroll::init()
{
    // Every window in Freelancer has a virtual "scroll" function
    // In the case of the ship preview window, this function does basically nothing
    // We replace the pointer to this dummy function in the ship preview window's vftable with our own
    patch::patch_uint32(OF_SHIP_PREVIEW_WINDOW_SCROLL, (UINT) ShipPreviewWindowScroll);
}