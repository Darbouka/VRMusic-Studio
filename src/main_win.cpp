#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <memory>
#include <string>
#include "ui/UIComponents.hpp"
#include <spdlog/spdlog.h>

using namespace VRMusicStudio::UI;

// Globale Variablen
HWND g_hwnd = nullptr;
std::unique_ptr<WindowsUIComponent> g_uiComponent;

// Forward-Deklarationen
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
bool InitializeWindow(HINSTANCE hInstance, int nCmdShow);
void Cleanup();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Logger initialisieren
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("VRMusic Studio wird gestartet...");

    // Fenster erstellen
    if (!InitializeWindow(hInstance, nCmdShow)) {
        spdlog::error("Fenster-Initialisierung fehlgeschlagen");
        return 1;
    }

    // UI-Komponente erstellen
    g_uiComponent = std::make_unique<WindowsUIComponent>();
    if (!g_uiComponent->initialize(g_hwnd)) {
        spdlog::error("UI-Komponenten-Initialisierung fehlgeschlagen");
        return 1;
    }

    // Nachrichtenschleife
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            // Rendern
            g_uiComponent->render();
        }
    }

    Cleanup();
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_SIZE:
            if (g_uiComponent) {
                g_uiComponent->resize(LOWORD(lParam), HIWORD(lParam));
            }
            return 0;

        case WM_PAINT:
            if (g_uiComponent) {
                g_uiComponent->render();
            }
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

bool InitializeWindow(HINSTANCE hInstance, int nCmdShow) {
    // Fensterklasse registrieren
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"VRMusicStudioClass";

    if (!RegisterClassEx(&wc)) {
        spdlog::error("Fensterklassen-Registrierung fehlgeschlagen");
        return false;
    }

    // Fenster erstellen
    RECT rc = { 0, 0, 800, 600 };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    g_hwnd = CreateWindowEx(
        0,
        L"VRMusicStudioClass",
        L"VRMusic Studio",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left,
        rc.bottom - rc.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!g_hwnd) {
        spdlog::error("Fenster-Erstellung fehlgeschlagen");
        return false;
    }

    ShowWindow(g_hwnd, nCmdShow);
    return true;
}

void Cleanup() {
    g_uiComponent.reset();
    if (g_hwnd) {
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
    }
} 