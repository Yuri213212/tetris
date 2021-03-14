HICON drawIcon(){
	const BYTE ANDmaskIcon[]={
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00,
		0x00,0x00
	};
	const BITMAP bitmapmask={0,16,16,2,1,1,(void *)ANDmaskIcon};
	static ICONINFO iconinfo={TRUE};

	HDC hdc,hdcMem;
	HBRUSH hBrushRed,hBrushBlue;
	HICON hIcon;

	hdc=GetDC(NULL);
	hdcMem=CreateCompatibleDC(hdc);
	iconinfo.hbmMask=CreateBitmapIndirect(&bitmapmask);
	iconinfo.hbmColor=CreateCompatibleBitmap(hdc,16,16);
	ReleaseDC(NULL,hdc);
	SelectObject(hdcMem,iconinfo.hbmColor);
	hBrushRed=CreateSolidBrush(RGB(255,0,0));
	hBrushBlue=CreateSolidBrush(RGB(0,0,255));
	SelectObject(hdcMem,GetStockObject(NULL_PEN));
	SelectObject(hdcMem,GetStockObject(WHITE_BRUSH));
	Rectangle(hdcMem,0,0,17,17);
	SelectObject(hdcMem,hBrushRed);
	Rectangle(hdcMem,0,8,5,17);
	Rectangle(hdcMem,8,12,17,17);
	SelectObject(hdcMem,hBrushBlue);
	Rectangle(hdcMem,4,0,9,13);
	Rectangle(hdcMem,8,4,13,9);
	SelectObject(hdcMem,GetStockObject(GRAY_BRUSH));
	Rectangle(hdcMem,5,1,8,4);
	Rectangle(hdcMem,5,5,8,8);
	Rectangle(hdcMem,9,5,12,8);
	Rectangle(hdcMem,1,9,4,12);
	Rectangle(hdcMem,5,9,8,12);
	Rectangle(hdcMem,1,13,4,16);
	Rectangle(hdcMem,9,13,12,16);
	Rectangle(hdcMem,13,13,16,16);
	DeleteObject(hBrushRed);
	DeleteObject(hBrushBlue);
	hIcon=CreateIconIndirect(&iconinfo);
	DeleteDC(hdcMem);
	DeleteObject(iconinfo.hbmMask);
	DeleteObject(iconinfo.hbmColor);
	return hIcon;
}
