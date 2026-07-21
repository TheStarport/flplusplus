#pragma once

namespace patch {
    
void detour(unsigned char* pOFunc, void* pHkFunc, unsigned char* originalData);
void undetour(unsigned char* pOFunc, unsigned char* originalData);
void patch_bytes(unsigned int address, void* pData, unsigned int pSize);
void patch_jmp(unsigned char* pOFunc, void* pHkFunc);
inline void patch_uint32(unsigned int address, unsigned int data)
{
    patch_bytes(address, (void*)&data, 4);
}
inline void patch_uint16(unsigned int address, unsigned short data)
{
    patch_bytes(address, (void*)&data, 2);
}
inline void patch_uint8(unsigned int address, unsigned char data)
{
    patch_bytes(address, (void*)&data, 1);
}
inline void patch_float(unsigned int address, float data)
{
    patch_bytes(address, (void*)&data, 4);
}
inline void patch_x3(unsigned int address, unsigned char a, unsigned char b, unsigned char c)
{
    unsigned char bytes[] = { a, b, c };
    patch_bytes(address, (void*)bytes, 3);
}
inline void patch_call(unsigned int address, unsigned int new_function)
{
    unsigned char bytes[] = { 0xE8, 0, 0, 0, 0 };
    *((unsigned int*)&bytes[1]) = new_function - address - 5;
    patch_bytes(address, (void*)bytes, 5);
}
}

