//Based off code by Laz

#include "screenshot.h"
#include "patch.h"
#include "offsets.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ctime>
#include <string>
#include <shlwapi.h>
#include <gdiplus.h>
#include <wchar.h>

using namespace Gdiplus;

typedef bool (*screenshot_path_t)(char * const);
screenshot_path_t GetScreenShotPath;

std::wstring stows(std::string str)
{
    return std::wstring(str.begin(), str.end());
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = nullptr;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == nullptr)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}
	free(pImageCodecInfo);
	return -1;  // Failure
}

static DWORD OnScreenshot()
{
    char directory[MAX_PATH];
    if(!GetScreenShotPath(directory))
    {
        return DWORD(-1);
    }
    // get the device context of the screen
	HDC hScreenDC = GetDC(nullptr);
	
	// and a device context to put it in
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

	int width = GetDeviceCaps(hScreenDC, HORZRES);
	int height = GetDeviceCaps(hScreenDC, VERTRES);

	// maybe worth checking these are positive values
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);

	// get a new bitmap
	HBITMAP hOldBitmap = HBITMAP(SelectObject(hMemoryDC, hBitmap));

	BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hMemoryDC, hOldBitmap);
    
    std::time_t rawtime;
	std::tm* timeinfo;
	char buffer[100];

	std::time(&rawtime);
	timeinfo = std::localtime(&rawtime);

	std::strftime(buffer, 80, "%Y-%m-%d_%H-%M-%S", timeinfo);
	std::puts(buffer);
    
    std::string outfile = std::string(directory) + "/" + std::string(buffer);
    std::string suffix = std::string("");
    int i = 0;
    while(PathFileExistsA((outfile + suffix + std::string(".png")).c_str())) {
        i++;
        suffix = std::string("_") + std::to_string(i);
    }
    outfile = outfile + suffix + std::string(".png");
    
    GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
	Bitmap* image = new Bitmap(hBitmap, nullptr);

	CLSID myClsId;
	GetEncoderClsid(L"image/png", &myClsId);

	Status status = image->Save(stows(outfile).c_str(), &myClsId, nullptr);
	delete image;

	GdiplusShutdown(gdiplusToken);

	// clean up
	DeleteDC(hMemoryDC);
	DeleteDC(hScreenDC);
	
	return DWORD(-1);
}


void screenshot::init()
{
    HMODULE common = GetModuleHandleA("common.dll");
    GetScreenShotPath = (screenshot_path_t)GetProcAddress(common, "?GetScreenShotPath@@YA_NQAD@Z");
    unsigned char buffer[5];
    patch::detour((unsigned char*)OF_PRINTSCREEN, (void*)OnScreenshot, buffer);
}
