#ifndef _FLPLUSPLUS_H_
#define _FLPLUSPLUS_H_

#define FLPEXPORT __declspec(dllexport)
#ifdef __cplusplus
extern "C" {
#endif
    
typedef void (*flplusplus_cblatehook)(void*);    
FLPEXPORT void flplusplus_add_latehook(flplusplus_cblatehook hkfunc, void *userData);

#ifdef __cplusplus
}
#endif
#endif
