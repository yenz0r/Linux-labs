#include <windows.h>
#include <string>

using namespace std;

const int maxRows = 30;
const int maxColumns = 30;

int rows = 2, columns = 3;

int top = 0, bottom = 0;
int letterWidth = 6, letterHeight = 20;
int textSpacing = 0;
int linesSpacing = 0, letterAngle = 0;

string** matrix;

void GenerateMatrix()
{
	columns = (columns < 1) ? 1 : columns;
	columns = (columns > maxColumns) ? maxColumns : columns;
	rows = (rows < 1) ? 1 : rows;
	rows = (rows > maxRows) ? maxRows : rows;

	matrix = new string*[rows];
	for (int i = 0; i < rows; i++)
	{
		matrix[i] = new string[columns];
		for (int j = 0; j < columns; j++)
		{
			string line = "";
			int randcounter = rand() % 150 + 1;
			for (int k = 0; k < randcounter; k++)
				line += "*";
			matrix[i][j] = line;
		}
	}
}

void RemoveMatrix()
{
	for (int i = 0; i < rows; i++)
	{
		delete[] matrix[i];
	}
}

float GetRowHeight(int rowNumber, float hx)
{
	int maxLength = 0;
	int lettersCount = 0, length = 0;
	int rows = 0;

	for (int i = 0; i < columns; i++)
	{
		lettersCount = matrix[rowNumber][i].length();
		length = lettersCount*letterWidth + (lettersCount - 1)*textSpacing;
		if (length > maxLength)
		{
			maxLength = length;
		}
	}
	rows = (int)((maxLength / hx) +1);
	return rows*(letterHeight)+linesSpacing*(rows);
}

void DrawTable(HDC hdc,int sx,int sy,int borderSize)
{
	float posX = 0, posY = (float)top;
	float hx = (float)sx / columns - 1;
	float hy = 0;
	HFONT hFont;
	RECT rect;
	string str;

	textSpacing = (int)((hx - 2 * borderSize) / 50);
	hFont = CreateFont(
		letterHeight,
		letterWidth,
		letterAngle, 
		0, 
		FW_DONTCARE, 
		FALSE,
		FALSE,
		FALSE, 
		DEFAULT_CHARSET, 
		OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, 
		CLEARTYPE_QUALITY, 
		VARIABLE_PITCH,
		TEXT("Times New Roman"));
	SelectObject(hdc, hFont);

	for (int i = 0; i < rows; i++)
	{
		hy = GetRowHeight(i, hx);
		posX = 1;
		for (int j = 0; j < columns; j++)
		{
			MoveToEx(hdc, (int)posX, (int)posY, NULL);
			LineTo(hdc, (int)posX, (int)(posY+hy));
			rect.top = (long)(posY+borderSize);
			rect.left = (long)(posX + borderSize);
			rect.right = (long)(posX + hx -borderSize);
			rect.bottom = (long)(posY+hy - borderSize);	
			DrawText(hdc, matrix[i][j].c_str(), matrix[i][j].length(), &rect, DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT);
			posX += hx;
		}
		
		MoveToEx(hdc, (int)(posX-1), (int)posY, NULL);
		LineTo(hdc, (int)(posX-1), (int)(posY + hy));

		MoveToEx(hdc, 0, (int)posY, NULL);
		LineTo(hdc, sx, (int)posY);
		posY += hy;
	}
	MoveToEx(hdc, 0, (int)posY, NULL);
	LineTo(hdc, sx, (int)posY);
	bottom = (int)posY;

	DeleteObject(hFont);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	TEXTMETRIC tm;
	int step = 0;
	int wheelDelta, MB_RESULT;
	bool isPressed;
	static int width = 0, height = 0;

	switch (message)
	{
	case WM_CREATE:
	{
		hdc = GetDC(hWnd);
		GetTextMetrics(hdc, &tm);
		ReleaseDC(hWnd, hdc);
		break;
	}
	case WM_KEYDOWN:
		isPressed = false;
		if (wParam == 39) 
			if (letterWidth < 20)
			{
				letterWidth++;
				isPressed = true;
			}
		if (wParam == 37) 
			if (letterWidth > 3)
			{
				letterWidth--;
				isPressed = true;
			}
		if (wParam == 40)
			if (letterHeight < 35)
			{
				letterHeight++;
				isPressed = true;
			}
		if (wParam == 38)
			if (letterHeight > 10)
			{
				letterHeight--;
				isPressed = true;
			}
		if (wParam == 27)
		{
			MB_RESULT = MessageBox(hWnd, "Do you want to close app?", "Exit..", MB_YESNO);
			if (MB_RESULT == 6)
				SendMessage(hWnd, WM_DESTROY, wParam, lParam);
		}
		if (isPressed)
			InvalidateRect(hWnd, NULL, TRUE);
		break;
	case WM_SIZE: 
		width = LOWORD(lParam);
		height = HIWORD(lParam);		
		ScrollWindow(hWnd, 0, -top, NULL, NULL);
		UpdateWindow(hWnd);
		top = 0;
		InvalidateRect(hWnd, NULL, TRUE);		
		break;
	case WM_MOUSEWHEEL:
		wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (wheelDelta > 0)
			SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
		if (wheelDelta < 0)
			SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
		break;
	case WM_VSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			step = 30;
			break;
		case SB_LINEDOWN:
			step = -30;
			break;
		}
		top += step;
		if ( ((top != 30) && (step>0)) || ((step<0) && (bottom >= height)) )
		{
			ScrollWindow(hWnd, 0, step, NULL, NULL);
			UpdateWindow(hWnd);
		}
		else
			top -= step;		
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		DrawTable(hdc, width, height,3);
		ReleaseDC(hWnd, hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		RemoveMatrix();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_CROSS);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "lab 2";
	wcex.hIconSm = wcex.hIcon;
	
	RegisterClassEx(&wcex);
	GenerateMatrix();

	hWnd = CreateWindow("lab 2", "******************",
		WS_OVERLAPPEDWINDOW|WS_VSCROLL, CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
