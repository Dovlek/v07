#include "main.h"
#include "rc.h"
#include <filesystem>

using namespace Gdiplus;

void main_window::on_paint(HDC hdc) 
{
	RECT client;
	GetClientRect(*this, &client);
	HDC hdcMem = CreateCompatibleDC(hdc);
	
	if (bmpimage)
	{
		SelectObject(hdcMem, bmpimage);
		SetStretchBltMode(hdc, COLORONCOLOR);
		StretchBlt(hdc, 0, 0, client.right, client.bottom, hdcMem, 0, 0, bitmap_width, bitmap_height, SRCCOPY);

		Graphics gr(hdc);
		RectF imageText(client.left, client.top, client.right, client.bottom);
		RectF textShadow(imageText);
		textShadow.Offset(-2.5, -2.5);
		
		StringFormat sf;
		sf.SetAlignment(StringAlignmentCenter);
		sf.SetLineAlignment(StringAlignmentFar);
		Font textFont(_T("Helvetica"), 24, FontStyleBold);
		
		SolidBrush textBrush(Color::Black);
		gr.DrawString(file_name.c_str(), -1, &textFont, imageText, &sf, &textBrush);
		
		textBrush.SetColor(Color::White);
		gr.DrawString(file_name.c_str(), -1, &textFont, textShadow, &sf, &textBrush);
	}

	DeleteDC(hdcMem);
}

void main_window::on_command(int id) 
{
	switch (id) 
	{
		case ID_OPEN:
		{
			TCHAR path[MAX_PATH]; *path = 0;
			TCHAR filter[] = _T("All Image Files \0*.jpg;*.jpeg;*.png;*.bmp;*.gif;*.tiff;*.emf;\0All Files (*.*)\0*.*\0");
			OPENFILENAME ofn{ sizeof OPENFILENAME };
			ofn.lpstrFile = path;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = filter;
			ofn.Flags = OFN_HIDEREADONLY;
			if (::GetOpenFileName(&ofn))
			{
				Bitmap bitmap(path);
				if (bitmap.GetLastStatus() == Ok)
				{
					bitmap_height = bitmap.GetHeight();
					bitmap_width = bitmap.GetWidth();

					if (bmpimage)
						DeleteObject(bmpimage);
					
					bitmap.GetHBITMAP(Color::White, &bmpimage);
					file_name = std::filesystem::path(path).filename();
				}
				InvalidateRect(*this, nullptr, true);
			}
			break;
		}
		case ID_EXIT:
			DestroyWindow(*this);
			break;
	}
}

void main_window::on_destroy() 
{
	::PostQuitMessage(0);
}

int WINAPI _tWinMain(HINSTANCE instance, HINSTANCE, LPTSTR, int)
{
	gdiplus_application app;
	main_window wnd;
	wnd.create(0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, _T("NWP 7"), (int64_t)LoadMenu(instance, MAKEINTRESOURCE(IDM_MAIN)));
	return app.run();
}
