#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <utility>

using namespace std;

#include <Windows.h>
#include <stdio.h>

// Console size
int nScreenWidth = 120;
int nScreenHeigth = 40;

// Player position
float fPlayerX = 8.0f;
float fPlayerY = 8.0f;
float fPlayerA = 0.0f;

// Map size
int nMapHeigth = 16;
int nMapWidth = 16;

// FOV
float fFOV = 3.14159f / 4.0f;
float fDepth = 16;


int main() {
    
    // Screen buffer
    wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeigth];
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    wstring map;

    map += L"################";
    map += L"#......#.......#";
    map += L"#......#.......#";
    map += L"#..........#...#";
    map += L"############..##";
    map += L"#........#.....#";
    map += L"#........#.....#";
    map += L"#..#######.....#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"###...######..##";
    map += L"#..........#...#";
    map += L"#..........#...#";
    map += L"#..........#...#";
    map += L"#..........#...#";
    map += L"################";

    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();


    // Game loop
    while (true) {

        // Timing
        tp2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float fElapsedTime = elapsedTime.count();

        
        // Controls
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
            fPlayerA -= 1.5f * fElapsedTime; // turn left

        if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
            fPlayerA += 1.5f * fElapsedTime; // turn right

        if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
            // forward and update position
            fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
            // collision check
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
                fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
            }
        }

        if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
            // backwards and update position
            fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
            fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
            // collision check
            if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
                fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
                fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
            }
        }


        for (int x = 0; x < nScreenWidth; x++) {

            // Calculate projected ray angle for each column
            float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

            float fDistanceToWall = 0.0f;
            bool bHitWall = false;
            bool bBoundary = false;

            // Unit vector
            float fEyeX = sinf(fRayAngle);
            float fEyeY = cosf(fRayAngle);

            while (!bHitWall && fDistanceToWall < fDepth) {

                fDistanceToWall += 0.1f;

                int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
                int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

                // Test for out-of-bounds ray
                if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeigth) {
                    bHitWall = true;
                    fDistanceToWall = fDepth;
                }
                else {
                    // Ray is in-bounds so do a map check
                    if (map[nTestY * nMapWidth + nTestX] == '#') {
                        bHitWall = true;

                        vector<pair<float, float>> p;

                        for (int tx = 0; tx < 2; tx++)
                            for (int ty = 0; ty < 2; ty++) {
                                float vy = (float)nTestY + ty - fPlayerY;
                                float vx = (float)nTestX + tx - fPlayerX;
                                float d = sqrt(vx * vx + vy * vy);
                                float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
                                p.push_back(make_pair(d, dot));
                            }

                        sort(p.begin(), p.end(), [](const pair<float, float> &left, const pair<float, float> &right) {
                            return left.first < right.first;
                            });

                        float fBound = 0.01; // threshold for corner boundary detection
                        if (acos(p.at(0).second) < fBound) bBoundary = true;
                        if (acos(p.at(1).second) < fBound) bBoundary = true;
                        if (acos(p.at(2).second) < fBound) bBoundary = true;
                    }
                }
            }

            // Calculate distance to ceiling and floor
            int nCeiling = (float)(nScreenHeigth / 2.0) - nScreenHeigth / ((float)fDistanceToWall);
            int nFloor = nScreenHeigth - nCeiling;

            short nWallShade = ' ';
            short nFloorShade = ' ';

            // Unicode shading with distance
            if (fDistanceToWall <= fDepth / 4.0f)
                nWallShade = 0x2588;
            else if (fDistanceToWall < fDepth / 3.0f)
                nWallShade = 0x2593;
            else if (fDistanceToWall < fDepth / 2.0f)
                nWallShade = 0x2592;
            else if (fDistanceToWall < fDepth)
                nWallShade = 0x2591;
            else
                nWallShade = ' ';

            if (bBoundary)
                nWallShade = ' ';


            for (int y = 0; y < nScreenHeigth; y++) {
                if (y < nCeiling)
                    screen[y * nScreenWidth + x] = ' '; // ceiling
                else if (y > nCeiling && y <= nFloor)
                    screen[y * nScreenWidth + x] = nWallShade; // wall
                else {
                    // floor shading
                    float b = 1.0f - (((float)y - nScreenHeigth / 2.0f) / ((float)nScreenHeigth / 2.0f));
                    if (b < 0.25)
                        nFloorShade = '#';
                    else if (b < 0.5)
                        nFloorShade = 'x';
                    else if (b < 0.75)
                        nFloorShade = '.';
                    else if (b < 0.9)
                        nFloorShade = '-';
                    else
                        nFloorShade = ' ';
                    screen[y * nScreenWidth + x] = nFloorShade;
                }
            }
        }

        // Display stats
        swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f, FPS=%3.2f", fPlayerX, fPlayerY, fPlayerA, 1.0f / fElapsedTime);

        // Display map
        for (int nx = 0; nx < nMapWidth; nx++)
            for (int ny = 0; ny < nMapWidth; ny++) {
                screen[(ny + 1) * nScreenWidth + nx] = map[ny * nMapWidth + (nMapWidth - nx - 1)];
            }

        // Display player
        screen[((int)fPlayerY + 1) * nScreenWidth + (int)(nMapWidth - fPlayerX)] = 'P';

        screen[nScreenWidth * nScreenHeigth - 1] = '\0';
        WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeigth, { 0, 0 }, &dwBytesWritten);
    }
    return 0;
}