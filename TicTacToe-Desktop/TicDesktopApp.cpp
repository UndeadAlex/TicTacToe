// TicDesktopApp.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "TicDesktopApp.h"
#include <windowsx.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TICDESKTOPAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	// Handles Hotkeys.
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TICDESKTOPAPP));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



/*
  FUNCTION: MyRegisterClass()

  PURPOSE: Registers the window class.
*/
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TICDESKTOPAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.hbrBackground = (HBRUSH)(GetStockObject(DKGRAY_BRUSH));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TICDESKTOPAPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_TICDESKTOPAPP));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   //HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, 0 , 0,  10 , 10, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
/*
//
// Global Variables for the game
// setup.
// players.
//
*/

const int BOX_SIZE = 100;
HBRUSH hb1, hb2;
HICON hIcon1, hIcon2;
int currentPlayer = 1;
int gameBoard[9] = {0,0,0, 0,0,0, 0,0,0};
int winner = 0;
int wins[3];


bool GetGameBoardRect(HWND hwnd, RECT* pRect)
{
	RECT rc;
	if (GetClientRect(hwnd, &rc))
	{
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;

		pRect->left = (width - BOX_SIZE * 3) / 2;
		pRect->top = (height - BOX_SIZE * 3) / 2;

		pRect->right = pRect->left + BOX_SIZE * 3;
		pRect->bottom = pRect->top + BOX_SIZE * 3;

		return TRUE;
	}

	SetRectEmpty(pRect);
	return FALSE;
}

void DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	MoveToEx(hdc, x1, y1, NULL);
	LineTo(hdc, x2, y2);
}

int GetCellFromMouse(HWND hwnd, int x, int y) 
{
	POINT pt = {x,y};
	RECT rc;
	if (GetGameBoardRect(hwnd, &rc))
	{
		if (PtInRect(&rc, pt))
		{
			// User clicked in gameboard rect.
			x = pt.x - rc.left;
			y = pt.y - rc.top;

			int col = x / BOX_SIZE;
			int row = y / BOX_SIZE;

			return col + row * 3;
		}
	}
	return -1; // Fail because user clicked out side gameboard rect;
}

bool GetCellRect(HWND hWnd, int index, RECT* pRect)
{
	RECT rcBoard;

	SetRectEmpty(pRect);

	if (index < 0 || index > 8)
	{
		return FALSE;
	}
	if (GetGameBoardRect(hWnd, &rcBoard))
	{
		int y = index / 3; // Row Number.
		int x = index % 3; // Col Number.

		pRect->left = rcBoard.left + x * BOX_SIZE + 5;
		pRect->top = rcBoard.top + y * BOX_SIZE + 5;

		pRect->right = pRect->left + BOX_SIZE - 10;
		pRect->bottom = pRect->top + BOX_SIZE - 10;

		return TRUE;
	}

	return FALSE;
}

/*
 This function will return.
 0 - No one won.
 1 - Player 1 Won.
 2 - Player 2 Won.
 3 - Draw.

 Board:

 0 1 2
 3 4 5
 6 7 8

*/
int CheckWinner(int wins[3])
{
	int cells[] = { 0,1,2, 3,4,5, 6,7,8, 0,3,6, 1,4,7, 2,5,8, 0,4,8, 2,4,6 };

	//Check cells for win.
	for (int i = 0; i < ARRAYSIZE(cells); i += 3)
	{
		if ( (gameBoard[cells[i]] != 0) && gameBoard[cells[i]] == gameBoard[cells[i + 1]] && gameBoard[cells[i]] == gameBoard[cells[i + 2]] )
		{
			wins[0] = cells[i];
			wins[1] = cells[i + 1];
			wins[2] = cells[i + 2];

			return gameBoard[cells[i]];
		}
	}
	// Check for empty cells.
	for (int i = 0; i < ARRAYSIZE(gameBoard); i++)
		if (gameBoard[i] == 0)
			return 0; // Continue.

	return 3;
}

void ShowTurn(HWND hwnd, HDC hdc)
{
	static const WCHAR szTurn1[] = L"Turn: Player 1";
	static const WCHAR szTurn2[] = L"Turn: Player 2";

	const WCHAR* pszTurnText = (currentPlayer == 1) ? szTurn1 : szTurn2;

	switch (winner)
	{
	case 0: // Continue.
		pszTurnText = (currentPlayer == 1) ? szTurn1 : szTurn2;
		break;
	case 1: // Player 1 Wins.
		pszTurnText = L"Player 1 is the winner!";
		break;
	case 2: // Player 2 Wins.
		pszTurnText = L"Player 2 is the winner!";
		break;
	case 3: // Draw
		pszTurnText = L"Its a draw !";
		break;
	}


	RECT rc;

	if (pszTurnText != NULL && GetClientRect(hwnd, &rc))
	{
		rc.top = rc.bottom - 48;
		FillRect(hdc, &rc, (HBRUSH)(GetStockObject(DKGRAY_BRUSH)));
		SetTextColor(hdc, RGB(255,255,255));
		SetBkMode(hdc, TRANSPARENT);
		DrawText(hdc, pszTurnText, lstrlen(pszTurnText), &rc, DT_CENTER);
	}
}

void DrawIconCentered(HDC hdc, RECT * pRect , HICON hIcon)
{
	const int ICON_W = 26;
	const int ICON_H = 26;
	if (pRect != NULL)
	{
		int left = pRect->left + ((pRect->right - pRect->left) - ICON_W) / 2;
		int top = pRect->top + ((pRect->bottom - pRect->top) - ICON_H) / 2;
		DrawIcon(hdc, left, top, hIcon);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		// Create custom brushes.
		hb1 = CreateSolidBrush(RGB(128, 128, 128));
		hb2 = CreateSolidBrush(RGB(0, 0, 0));

		// Custom Icons.

		hIcon1 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER1));
		hIcon2 = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PLAYER2));
	}
	break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_FILE_NEWGAME:
			{
				int responce = MessageBox(hWnd, L"Are you sure you want to start a new game ?", L"New Game?", MB_YESNO | MB_ICONQUESTION);

				if (IDYES == responce)
				{
					// Reset and create a new game.
					currentPlayer = 1;
					winner = 0;
					ZeroMemory(gameBoard, sizeof(gameBoard));
					// Force Paint Reset.
					InvalidateRect(hWnd, NULL, TRUE);  // Queues msg
					UpdateWindow(hWnd); // Forces window update.
				}
				
			}
			break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_LBUTTONDOWN:
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		// Continue Only if game is still being played.
		if (currentPlayer == 0)
			break;

		int index = GetCellFromMouse(hWnd, xPos, yPos);

		HDC hdc = GetDC(hWnd);
		if (NULL != hdc)
		{
			/*
			WCHAR temp[100];
			wsprintf(temp, L"Index = %d", index);
			TextOut(hdc, xPos, yPos, temp, lstrlen(temp));
			*/

			// Get cell size.
			if (index != -1)
			{
				RECT rcCell;
				if (gameBoard[index] == 0 && GetCellRect(hWnd, index, &rcCell))
				{
					gameBoard[index] = currentPlayer;
					//FillRect(hdc, &rcCell, (currentPlayer==1) ? hb1 : hb2);
					DrawIconCentered(hdc, &rcCell, (currentPlayer == 1) ? hIcon1 : hIcon2);

					winner = CheckWinner(wins);
					if (winner == 1 || winner == 2)
					{
						//Found Win.

						int resp = MessageBox(hWnd, (winner == 1) ? L"Player 1 Wins!, Would you like to play again ?" : L"Player 2 Wins!, Would you like to play again ?", L"You Win!", MB_YESNO | MB_ICONINFORMATION);

						currentPlayer = 0;

						if (IDYES == resp)
						{
							// Reset and create a new game.
							currentPlayer = 1;
							winner = 0;
							ZeroMemory(gameBoard, sizeof(gameBoard));
							// Force Paint Reset.
							InvalidateRect(hWnd, NULL, TRUE);  // Queues msg
							UpdateWindow(hWnd); // Forces window update.
						}
						else
						{
							DestroyWindow(hWnd);
							break;
						}
					}
					else if (winner == 3)
					{
						int resp = MessageBox(hWnd, L"It was a draw!, Would you like to play again ?", L"No Winner!", MB_YESNO | MB_ICONINFORMATION);

						currentPlayer = 0;

						if (IDYES == resp)
						{
							// Reset and create a new game.
							currentPlayer = 1;
							winner = 0;
							ZeroMemory(gameBoard, sizeof(gameBoard));
							// Force Paint Reset.
							InvalidateRect(hWnd, NULL, TRUE);  // Queues msg
							UpdateWindow(hWnd); // Forces window update.
						}
						else
						{
							DestroyWindow(hWnd);
							break;
						}
					}
					else if (winner == 0)
					{
						currentPlayer = (currentPlayer == 1) ? 2 : 1;
					}

					// Display current player.
					ShowTurn(hWnd, hdc);

					
				}
			}
			ReleaseDC(hWnd, hdc);
		}
	}
	break;

	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* pMinMax = (MINMAXINFO*)lParam;

		pMinMax->ptMinTrackSize.x = BOX_SIZE * 5;
		pMinMax->ptMinTrackSize.y = BOX_SIZE * 5;

	}
	break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // Custom drawing code. :START:
			RECT rc;


			if (GetGameBoardRect(hWnd, &rc))
			{
				RECT rcClient;
				ShowTurn(hWnd, hdc);

				if (GetClientRect(hWnd, &rcClient))
				{
					SetBkMode(hdc, TRANSPARENT);
					// Draw Current player text.
					TextOut(hdc, 16, 16, L"Player 1", 8);
					TextOut(hdc, rcClient.right - 70, 16, L"Player 2", 8);
				}

				FillRect(hdc, &rc, (HBRUSH)(GetStockObject(WHITE_BRUSH)));

				//Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

				for (int i = 1; i < 3; i++)
				{
					// Draw Vertical lines.
					DrawLine(hdc, rc.left + BOX_SIZE * i, rc.top, rc.left + BOX_SIZE * i, rc.bottom);
					// Draw Horizontal lines.
					DrawLine(hdc, rc.left, rc.top + BOX_SIZE * i, rc.right, rc.top + BOX_SIZE * i);
				}


				RECT rcCell;
				for (int i = 0; i < ARRAYSIZE(gameBoard); i++)
				{
					if (gameBoard[i] != 0 && GetCellRect(hWnd, i, &rcCell))
					{
						//FillRect(hdc, &rcCell, (gameBoard[i]==1) ? hb1 : hb2);
						DrawIconCentered(hdc, &rcCell, (gameBoard[i] == 1) ? hIcon1 : hIcon2);
					}
				}
			}
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		DeleteObject(hb1);
		DeleteObject(hb2);
		DestroyIcon(hIcon1);
		DestroyIcon(hIcon2);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
