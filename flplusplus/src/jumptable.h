#ifndef _JUMPTABLE_H_
#define _JUMPTABLE_H_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// Macro magic to produce a compact set of functions to
// manually import functions
#ifdef _MSC_VER
#define JUMPTABLE( def, table, index ) __declspec(naked) def { __asm jmp table[index*8+4] }
#else
#define JUMPTABLE( def, table, index ) __attribute__((naked)) def { \
__asm(".intel_syntax noprefix\n"); \
__asm__("jmp _" #table  "[" #index "*8+4]\n"); \
__asm__(".att_syntax"); \
}
#endif
#define FUNC_INITIALIZER(f) \
        static void f(void); \
        struct f##_t_ { f##_t_(void) { f(); } }; static f##_t_ f##_; \
        static void f(void)
#define JUMPTABLE_INIT( dll, table ) \
FUNC_INITIALIZER( table ## _Load) \
{ \
  HMODULE library = LoadLibraryA(dll); \
  for(int i = 0; i < (sizeof(table) / sizeof(const char*)); i+=2) \
  { \
    table[i + 1] = (const char*)GetProcAddress(library, table[i]); \
  } \
}
#endif
