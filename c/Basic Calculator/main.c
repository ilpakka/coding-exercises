#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <Windows.h>

#define ID_EDIT 1
#define ID_BUTTON_ADD 2
#define ID_BUTTON_SUB 3
#define ID_BUTTON_MUL 4
#define ID_BUTTON_DIV 5
#define ID_BUTTON_EQUALS 6
#define ID_RESULT 7

HWND hInput, hResult;
float firstNumber = 0;
char currentOperator = 0;

float calculate(float number_1, float number_2, char operator, int *error);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE: {
            hInput = CreateWindowW(L"EDIT", L"",
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_RIGHT,
                50, 50, 200, 30,
                hwnd, (HMENU)ID_EDIT, NULL, NULL);

            hResult = CreateWindowW(L"EDIT", L"",
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_RIGHT | ES_READONLY,
                50, 100, 200, 30,
                hwnd, (HMENU)ID_RESULT, NULL, NULL);
                
            CreateWindowW(L"BUTTON", L"+",
                WS_VISIBLE | WS_CHILD,
                270, 50, 50, 50,
                hwnd, (HMENU)ID_BUTTON_ADD, NULL, NULL);

            CreateWindowW(L"BUTTON", L"-",
                WS_VISIBLE | WS_CHILD,
                330, 50, 50, 50,
                hwnd, (HMENU)ID_BUTTON_SUB, NULL, NULL);

            CreateWindowW(L"BUTTON", L"*",
                WS_VISIBLE | WS_CHILD,
                270, 110, 50, 50,
                hwnd, (HMENU)ID_BUTTON_MUL, NULL, NULL);

            CreateWindowW(L"BUTTON", L"/",
                WS_VISIBLE | WS_CHILD,
                330, 110, 50, 50,
                hwnd, (HMENU)ID_BUTTON_DIV, NULL, NULL);

            CreateWindowW(L"BUTTON", L"=",
                WS_VISIBLE | WS_CHILD,
                270, 170, 110, 50,
                hwnd, (HMENU)ID_BUTTON_EQUALS, NULL, NULL);
            break;
        }

        case WM_COMMAND: {
            if (HIWORD(wParam) == BN_CLICKED) {
                char buffer[256];
                GetWindowTextA(hInput, buffer, 256);
                float secondNumber = atof(buffer);
                
                switch(LOWORD(wParam)) {
                    case ID_BUTTON_ADD:
                        firstNumber = atof(buffer);
                        currentOperator = '+';
                        SetWindowTextA(hInput, "");
                        break;
                    case ID_BUTTON_SUB:
                        firstNumber = atof(buffer);
                        currentOperator = '-';
                        SetWindowTextA(hInput, "");
                        break;
                    case ID_BUTTON_MUL:
                        firstNumber = atof(buffer);
                        currentOperator = '*';
                        SetWindowTextA(hInput, "");
                        break;
                    case ID_BUTTON_DIV:
                        firstNumber = atof(buffer);
                        currentOperator = '/';
                        SetWindowTextA(hInput, "");
                        break;
                    case ID_BUTTON_EQUALS: {
                        int error = 0;
                        float result = calculate(firstNumber, secondNumber, currentOperator, &error);
                        if (!error) {
                            sprintf(buffer, "%.2f", result);
                            SetWindowTextA(hResult, buffer);
                        } else {
                            SetWindowTextA(hResult, "Error");
                        }
                        break;
                    }
                }
            }
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    HWND hwnd;
    MSG msg;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "Calculator";
    wc.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "Calculator", "Simple Calculator", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, hInstance, NULL);

    if(!hwnd) {
        MessageBox(NULL, "Window Creation Failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

float calculate(float number_1, float number_2, char operator, int *error) {

    float result;
    *error = 0;

    switch(operator) {
        case '+':
            result = number_1 + number_2;
            break;
        case '-':
            result = number_1 - number_2;
            break;
        case '*': // consolidation
        case 'x':
            result = number_1 * number_2;
            break;
        case '/':
            if (number_2 == 0) { // check divisor
                printf("Error: Division by zero\n");
                *error = 1;
                return 0;
            }
            result = number_1 / number_2;
            break;
        case '%':
            if (number_1 == 0 || number_2 == 0) {
                printf("Error: Module by zero\n");
                *error = 1;
                return 0;
            }
            result = fmod(number_1, number_2);
            break;
        case '^':
            result = pow(number_1, number_2);
            break;
        default:
            printf("Error: Invalid expression\n");
            *error = 1;
            return 0;
    }
    return result;
}