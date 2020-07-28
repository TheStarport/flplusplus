#pragma once

#define FLPEXPORT __declspec(dllexport)

namespace flplusplus {
typedef void (*cb_latehook)(void*);    
FLPEXPORT void add_latehook(cb_latehook hkfunc, void *userData);
}
