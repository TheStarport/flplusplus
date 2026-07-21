#include "msndump.h"
#include "log.h"
#include "patch.h"
#include <math.h>
#include "config.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern "C" 
{ 
    int print_rand; 
    int rand_override_offset;
}

char names[512];
const char *call_name(DWORD retaddr)
{
    DWORD content = (DWORD)GetModuleHandleA("content");
    const char *friendly = NULL;
    if(retaddr == 0)
    {
        friendly = "leaf";
    }
    if(retaddr ==(DWORD)content + 0xfd245)
    {
        friendly = "choices";
    }
    if(retaddr ==(DWORD)content + 0xb15b6)
    {
        friendly = "firstname";
    }
    if(retaddr ==(DWORD)content + 0xb1603)
    {
        friendly = "lastname";
    }
    if(retaddr ==(DWORD)content + 0xf8ae1)
    {
        friendly = "shipweightcount";
    }
    if(retaddr ==(DWORD)content + 0xf7af7)
    {
        friendly = "< bonuslootdropchance";
    }
    if(retaddr < content)
    {
        sprintf_s(names, sizeof(names), "unk %x", retaddr);
        return names;
    }
    else if(friendly != NULL)
    {
        sprintf_s(names, sizeof(names), "content+%x (%s)", retaddr - content, friendly);
        return names;
    }
    else
    {
        sprintf_s(names, sizeof(names), "content+%x", retaddr - content);
        return names;
    }
    return names;
}

static long holdrand = 1L;
extern "C" int __fastcall rand_impl (DWORD retaddr)
{
    int retval = (((holdrand = holdrand * 214013L + 2531011L)>>16) & 0x7fff);
    if(print_rand)
    {
        if(rand_override_offset < config::get_config().srandoverrides.size())
        {
            int ov = config::get_config().srandoverrides[rand_override_offset];
            rand_override_offset++;
            logger::writeformat("%s: rand() = %d (originally %d)", call_name(retaddr), ov, retval);
            retval = ov;
        }
        else
        {
            logger::writeformat("%s: rand() = %d", call_name(retaddr), retval);
        }
    }
    return retval;
}

__attribute__((naked)) void our_rand() 
{
    __asm__(".intel_syntax noprefix\n");
    
    // 1. The return address is currently at the top of the stack.
    // Move it into ECX (which __fastcall expects for argument 1)
    __asm__("mov ecx, [esp]\n"); 
    
    // 2. Tail-call into our_rand. 
    // We use JMP instead of CALL so our_rand returns directly to the game.
    // Note: __fastcall functions are usually name-mangled with @ and the argument byte size.
    __asm__("jmp @rand_impl@4\n"); 
    
    __asm__(".att_syntax\n");
}


void __cdecl srand_startlog (unsigned int seed)
{
    logger::writeformat("srand(%d)\n", seed);
    holdrand = (long)seed;
    print_rand = 1;
    rand_override_offset = 0;
}

void __cdecl regular_srand (unsigned int seed)
{
    holdrand = (long)seed;
}

#ifdef _MSC_VER
__declspec(naked) disable_log_rand() 
{ 
    __asm {
        mov dword ptr [_print_rand], 0x0
        add esp, 0x4c
        ret 0x4
    }
}
#else
__attribute__((naked)) disable_log_rand() 
{
    __asm__(".intel_syntax noprefix\n");
    __asm__("mov dword ptr [_print_rand], 0x0\n");
    __asm__("add esp, 0x4c\n");
    __asm__("ret 0x4\n");
    __asm__(".att_syntax\n");
}
#endif


struct CandidateItem {
    int id;       // Or void* pointer, depending on what param_3 holds
    float weight; // The random selection weight
};

struct CandidateVector {
    void* _Myproxy;               // 0x0
    CandidateItem* _M_start;      // 0x4
    CandidateItem* _M_finish;     // 0x8
    CandidateItem* _M_end_of_storage; // 0xC
};

struct CandidateChain {
    int unknown_00;
    int unknown_04;
    char *chain_start;  // Changed from void* to char*
    char *chain_finish; // Changed from void* to char*
    int unknown_10;
};

void iterate_choice_tree(CandidateChain *vec)
{
    char *s = vec->chain_start;
    char *end = vec->chain_finish;
    // Create a fixed-size buffer to hold our path string
    char path_buffer[256]; 
    int index = 0;
    while (s < end && index < 255)
    {
        // Dereference 's' to get the byte, assign 'T' for 1, 'F' for 0
        if (*s == 1) {
            path_buffer[index] = 'T';
        } else {
            path_buffer[index] = 'F';
        }
        
        index++;
        s++;
    }
    // Null-terminate the C-string
    path_buffer[index] = '\0';
    // Output the formatted string (replace with your specific C logger)
    logger::writeformat("P: %s", path_buffer);
}

extern "C" int __fastcall on_iterate (CandidateVector* vec, DWORD variant)
{
    CandidateItem* s = vec->_M_start;
    logger::writeformat("Candidate vector %u %x,%x", variant, vec->_M_start, vec->_M_finish);
    while(s < vec->_M_finish)
    {
        if(variant == 1)
        {
            logger::writeformat("CHAIN %x: weight %.7f", s->id, s->weight);
            iterate_choice_tree((CandidateChain*)s->id);
        }
        else
        {
            logger::writeformat("Candidate %d, weight %.7f", s->id, s->weight);
        }
        s++;
    }
    return rand_impl(0);
}



__attribute__((naked)) iterate_vector() 
{
    __asm__(".intel_syntax noprefix\n");
    __asm__("lea ecx, [esp+0x2c]\n"); 
    __asm__("xor edx, edx\n");
    __asm__("call @on_iterate@8\n"); 
    __asm__("ret\n");                
    __asm__(".att_syntax\n");
}

__attribute__((naked)) iterate_vector_2() 
{
    __asm__(".intel_syntax noprefix\n");
    __asm__("sub esp, 112\n");      
    __asm__("fnsave [esp]\n");      
    __asm__("lea ecx, [esp+0xB0]\n"); 
    __asm__("mov edx, 1\n");
    __asm__("call @on_iterate@8\n"); 
    __asm__("frstor [esp]\n");      
    __asm__("add esp, 112\n");      
    __asm__("ret\n");                
    __asm__(".att_syntax\n");
}

typedef float (__cdecl *diff_func)(DWORD, float, float);

static diff_func FUN_06f4ad60;

float __cdecl diff_hook(DWORD param_1, float param_2, float param_3)
{
    float retval = FUN_06f4ad60(param_1, param_2, param_3);
    logger::writeformat("difficulty_function(%x, %f, %f) = %7f", param_1, param_2, param_3, retval);
    return retval;
}

typedef DWORD (__fastcall *pick_path)(DWORD, DWORD, DWORD);

DWORD __fastcall pick_path_hook(DWORD param_1, DWORD param_2, DWORD param_3)
{
    pick_path original = (pick_path)((DWORD)GetModuleHandleA("content") + 0xfd070);
    DWORD retval = original(param_1, param_2, param_3);
    logger::writeformat("pick_path(%x, %x, %x) = %x", param_1, param_2, param_3, retval);
    return retval;
}

DWORD __fastcall pick_leaf_hook(DWORD param_1, DWORD param_2, DWORD param_3)
{
    pick_path original = (pick_path)((DWORD)GetModuleHandleA("content") + 0xfcb00);
    DWORD retval = original(param_1, param_2, param_3);
    logger::writeformat("pick_leaf(%x, %x, %x) = %d", param_1, param_2, param_3, retval);
    return retval;
}

void patch_content(HMODULE content)
{
    unsigned char temp[5];
    HMODULE msvcrt = GetModuleHandleA("msvcrt");
    patch::detour((unsigned char*)GetProcAddress(msvcrt, "srand"), (void*)regular_srand, temp);
    patch::detour((unsigned char*)GetProcAddress(msvcrt, "rand"), (void*)our_rand, temp);

    patch::patch_call((DWORD)content + 0x50a53, (unsigned int)srand_startlog);
    patch::patch_uint8((DWORD)content + 0x50a58, 0x90); //nop 6th byte
    patch::patch_call((DWORD)content + 0xfcc8b, (unsigned int)iterate_vector);
    patch::patch_uint8((DWORD)content + 0xfcc90, 0x90); //nop 6th byte
    patch::patch_call((DWORD)content + 0xfd23f, (unsigned int)iterate_vector_2);
    patch::patch_uint8((DWORD)content + 0xfd244, 0x90); //nop 6th byte
    patch::patch_jmp((unsigned char*)((DWORD)content + 0x50af7), (void*)disable_log_rand);

    FUN_06f4ad60 = (diff_func)((DWORD)content + 0xAAD60);
    patch::patch_call((DWORD)content + 0x64258, (unsigned int)diff_hook);
    patch::patch_call((DWORD)content + 0x646eb, (unsigned int)diff_hook);

    patch::patch_call((DWORD)content + 0xfd34f, (unsigned int)pick_path_hook);
    patch::patch_call((DWORD)content + 0xff7ba, (unsigned int)pick_leaf_hook);
}

typedef HINSTANCE (__cdecl *DllLoadPtr)(const char*);

DllLoadPtr _dllLoad;
static unsigned char dllLoadData[5];

HINSTANCE DllLoadHook(const char *name)
{
    HMODULE common = GetModuleHandleA("common");
    GetProcAddress(common, "?begin_mad_lib@FmtStr@@QAEXI@Z");

    patch::undetour((unsigned char*)_dllLoad, dllLoadData);
    HINSTANCE retval = _dllLoad(name);
    patch::detour((unsigned char*)_dllLoad, (void*)DllLoadHook, dllLoadData);
    HMODULE content = GetModuleHandleA("content");
    if(content)
    {
        patch_content(content);
    }
    return retval;
}

void msndump::init()
{
    print_rand = 0;
    HMODULE server = GetModuleHandleA("server");
    _dllLoad = (DllLoadPtr)GetProcAddress(server, "?DLL_LoadLibrary@pub@@YAPAUHINSTANCE__@@PBD@Z");
    patch::detour((unsigned char*)_dllLoad, (void*)DllLoadHook, dllLoadData);
}
