#pragma once
#include "adoxa.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define NAKED	 __declspec(naked)
#define FASTCALL __fastcall


static DWORD x_dummy;
#define ProtectX( addr, size ) \
  VirtualProtect( addr, size, PAGE_EXECUTE_READWRITE, &x_dummy );
#define ProtectW( addr, size ) \
  VirtualProtect( addr, size, PAGE_READWRITE, &x_dummy );

#define RELOFS( from, to ) \
  *(PDWORD)(from) = (DWORD)(to) - (DWORD)(from) - 4

#define REL2ABS( addr ) ((DWORD)(addr) + *(PDWORD)(addr) + 4)

#define NEWOFS( from, to ) \
  to##_Old = REL2ABS( from ); \
  RELOFS( from, to##_Hook )

#define NEWABS( from, to ) \
  to##_Old = *from; \
  *from = (DWORD)(to##_Hook)

#define CALL( from, to ) \
  ((PBYTE)(from))[0] = 0xE8; \
  RELOFS( (PBYTE)(from)+1, to##_Hook )

#define CALL1( from, to ) \
  CALL( from, to ); \
  ((PBYTE)(from))[5] = 0x90; \

#define INDIRECT( from, to ) \
  to##_Old = **(PDWORD*)(from); \
  to##_New = (DWORD)to##_Hook; \
  *(PDWORD*)(from) = &to##_New

