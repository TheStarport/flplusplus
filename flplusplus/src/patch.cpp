#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

namespace patch {
    
void detour(unsigned char* pOFunc, void* pHkFunc, unsigned char* originalData)
{
	DWORD dwOldProtection = 0; // Create a DWORD for VirtualProtect calls to allow us to write.
	BYTE bPatch[5]; // We need to change 5 bytes and I'm going to use memcpy so this is the simplest way.
	bPatch[0] = 0xE9; // Set the first byte of the byte array to the op code for the JMP instruction.
	VirtualProtect((void*)pOFunc, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection); // Allow us to write to the memory we need to change
	DWORD dwRelativeAddress = (DWORD)pHkFunc - (DWORD)pOFunc - 5; // Calculate the relative JMP address.
	memcpy(&bPatch[1], &dwRelativeAddress, 4); // Copy the relative address to the byte array.
	memcpy(originalData, pOFunc, 5);
	memcpy(pOFunc, bPatch, 5); // Change the first 5 bytes to the JMP instruction.
	VirtualProtect((void*)pOFunc, 5, dwOldProtection, NULL); // Set the protection back to what it was.
}
void undetour(unsigned char* pOFunc, unsigned char* originalData)
{
	DWORD dwOldProtection = 0; // Create a DWORD for VirtualProtect calls to allow us to write.
	VirtualProtect((void*)pOFunc, 5, PAGE_EXECUTE_READWRITE, &dwOldProtection); // Allow us to write to the memory we need to change
	memcpy(pOFunc, originalData, 5);
	VirtualProtect((void*)pOFunc, 5, dwOldProtection, NULL); // Set the protection back to what it was.
}

void patch_bytes(unsigned int address, void* pData, unsigned int pSize)
{
    DWORD dwOldProtection = 0;
    VirtualProtect((void*)address, pSize, PAGE_READWRITE, &dwOldProtection);
    memcpy((void*)address, pData, pSize);
    VirtualProtect((void*)address, pSize, dwOldProtection, NULL);
}

}
