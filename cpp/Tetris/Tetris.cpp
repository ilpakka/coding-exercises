#include <iostream>
#include <Windows.h>

using namespace std;

// Tetromino
wstring tetromino[7];

// Board
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char* pField = nullptr;

// Screen
int nScreenWidth = 80;
int nScreenHeight = 30;


// Rotation of tetromino
int Rotate(int px, int py, int r) {
    switch (r % 4) {
    case 0: return py * 4 + px; // 0 degrees
    case 1: return 12 + py - (px * 4); // 90 degrees
    case 2: return 15 - (py * 4) - px; // 180 degrees
    case 3: return 3 - py + (px * 4); // 270 degrees
    }
    return 0;
}

int main()
{
    // Create assets
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");

    tetromino[1].append(L"..X.");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L".X..");
    tetromino[1].append(L"....");

    tetromino[2].append(L".X..");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L"..X.");
    tetromino[2].append(L"....");

    tetromino[3].append(L"....");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L"....");

    tetromino[4].append(L"..X.");
    tetromino[4].append(L".XX.");
    tetromino[4].append(L"..X.");
    tetromino[4].append(L"....");

    tetromino[5].append(L"....");
    tetromino[5].append(L".XX.");
    tetromino[5].append(L"..X.");
    tetromino[5].append(L"..X.");

    tetromino[6].append(L"....");
    tetromino[6].append(L".XX.");
    tetromino[6].append(L".X..");
    tetromino[6].append(L".X..");

    pField = new unsigned char[nFieldWidth * nFieldHeight]; // Create buffer
    for (int x = 0; x < nFieldWidth; x++) // Board boundary
        for (int y = 0; y < nFieldHeight; y++)
            pField[x + nFieldWidth * y] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
    for (int i = 0; i < nScreenWidth * nScreenHeight; i++)
        screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    // Display frame
    WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0, 0 }, &dwBytesWritten);


    return 0;
}
