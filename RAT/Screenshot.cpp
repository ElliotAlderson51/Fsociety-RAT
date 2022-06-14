#include "Tools.h"

#include <GdiPlus.h>
#pragma comment( lib, "gdiplus" )

BOOL Tools::gdiscreen(std::string savePath)
{
	using namespace Gdiplus;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	{
		HDC screenDc, hDC;
		HBITMAP hBitmap;
		screenDc = GetDC(0); // GetDC retrieves the DC for the entire screen.
		int Height = GetSystemMetrics(SM_CYSCREEN);
		int Width = GetSystemMetrics(SM_CXSCREEN);

		std::wstring widestr = std::wstring(savePath.begin(), savePath.end());
		const wchar_t* pwcSavePath = widestr.c_str();

		// The CreateCompatibleDC function creates a memory 
		// device context (DC) compatible with the specified device.
		hDC = CreateCompatibleDC(screenDc);

		// The CreateCompatibleBitmap function creates a bitmap compatible 
		// with the device that is associated with the specified device context.
		hBitmap = CreateCompatibleBitmap(screenDc, Width, Height);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDC, hBitmap);
		BitBlt(hDC, 0, 0, Width, Height, screenDc, 0, 0, SRCCOPY);

		Gdiplus::Bitmap bitmap(hBitmap, NULL);
		CLSID clsid;
		this->GetEncoderClsid(L"image/jpeg", &clsid);
		bitmap.Save(pwcSavePath, &clsid);

		SelectObject(hDC, hOldBitmap);
		DeleteObject(hDC);
		DeleteObject(hBitmap);
		ReleaseDC(0, screenDc);
	}

	GdiplusShutdown(gdiplusToken);

	if (this->FileExists(savePath))
		return TRUE;
	return FALSE;

}

// convert from object to image
int Tools::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	using namespace Gdiplus;
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, pImageCodecInfo);
			return j;  // Success
		}
	}

	HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, pImageCodecInfo);
	return 0;
}


BOOL Tools::TakeScreenshot(const std::string& save_path)
{
	if (gdiscreen(save_path))
		return TRUE;
	return FALSE;
}