#include "shippreviewscroll.h"
#include "offsets.h"
#include "patch.h"
#include "config.h"

using namespace shippreviewscroll;

#define MIN_SCROLLING_SPEED 0.0f
#define MAX_SCROLLING_SPEED 50.0f

float scrollingSpeed;

bool __fastcall ShipPreviewWindowScroll(ShipPreviewWindow* window, PVOID _edx, ScrollDirection dir)
{
    if (dir == ScrollDirection::up)
        window->zoomLevel += scrollingSpeed;
    else if (dir == ScrollDirection::down)
        window->zoomLevel -= scrollingSpeed;

    // The scroll function should just return false
    return false;
}

void shippreviewscroll::init()
{
    if (config::get_config().shippreviewscrollingspeed < MIN_SCROLLING_SPEED)
        scrollingSpeed = MIN_SCROLLING_SPEED;
    else if (config::get_config().shippreviewscrollingspeed > MAX_SCROLLING_SPEED)
        scrollingSpeed = MAX_SCROLLING_SPEED;
    else
        scrollingSpeed = config::get_config().shippreviewscrollingspeed;

    if (config::get_config().shippreviewscrollinginverse)
        scrollingSpeed *= -1;

    // Every window in Freelancer has a virtual "scroll" function
    // In the case of the ship preview window, this function does basically nothing
    // We replace the pointer to this dummy function in the ship preview window's vftable with our own
    patch::patch_uint32(OF_SHIP_PREVIEW_WINDOW_SCROLL, (UINT) ShipPreviewWindowScroll);
}
