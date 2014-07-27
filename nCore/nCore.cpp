/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nCore.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nCore module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "nCore.h"
#include "../nCoreCom/Core.h"
#include "../nShared/ErrorHandler.h"
#include "../nShared/MonitorInfo.hpp"
#include "../Utilities/Versioning.h"
#include "../nShared/WindowBangs.h"
#include "../nShared/StateBangs.h"
#include "../nShared/BrushBangs.h"
#include "TextFunctions.h"
#include "ParsedText.hpp"
#include "Version.h"
#include "Scripting.h"

#include <strsafe.h>


// The messages we want from the core
static const UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, 0 };
static HWND ghWndMsgHandler;

// Class pointers
MonitorInfo * g_pMonitorInfo;

// Constants
static const LPCWSTR gMsgHandler = L"LSnCore";

// When the [time] text function should send out change notifications.
UINT_PTR timeTimer;

// 
EXPORT_CDECL(Window*) FindRegisteredWindow(LPCWSTR prefix);


/// <summary>
/// Gets the current core version.
/// </summary>
EXPORT_CDECL(VERSION) GetCoreVersion()
{
    return MakeVersion(MODULE_VERSION);
}


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
/// <param name="parent"></param>
/// <param name="instance">Handle to this module's instance.</param>
/// <param name="path">Path to the LiteStep directory.</param>
/// <returns>0 on success, non-zero on error.</returns>
/// <remarks>
/// If this function returns non-zero, the module will be unloaded immediately, without
/// going through quitModule.
/// </remarks>
int initModuleW(HWND parent, HINSTANCE instance, LPCWSTR path)
{
    UNREFERENCED_PARAMETER(parent);
    UNREFERENCED_PARAMETER(path);

    // Initialize
    if (!CreateMainWindow(instance))
    {
        return 1;
    }

    g_pMonitorInfo = new MonitorInfo();
    TextFunctions::_Register();
    timeTimer = SetTimer(ghWndMsgHandler, 1, 1000, nullptr);

    // We need to be connected to the core for some of the functions in nShared to work... xD
    nCore::Connect(MakeVersion(MODULE_VERSION));

    // Register window bangs
    WindowBangs::Register(L"n", FindRegisteredWindow);
    StateBangs::Register(L"n", FindRegisteredWindow);
    BrushBangs::Register(L"n", FindRegisteredWindow);

    // Start the scripting manager
    Scripting::Initialize();

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
/// <param name="instance">Handle to this module's instance.</param>
void quitModule(HINSTANCE instance)
{
    Scripting::Shutdown();

    WindowBangs::UnRegister(L"n");
    StateBangs::UnRegister(L"n");
    BrushBangs::UnRegister(L"n");

    // Deinitalize
    if (ghWndMsgHandler)
    {
        KillTimer(ghWndMsgHandler, timeTimer);
        SendMessageW(LiteStep::GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)ghWndMsgHandler, (LPARAM)gLSMessages);
        DestroyWindow(ghWndMsgHandler);
    }

    TextFunctions::_Unregister();

    UnregisterClassW(gMsgHandler, instance);
}


/// <summary>
/// Creates the main message handler.
/// </summary>
/// <param name="instance">The instance to attach this message handler to.</param>
bool CreateMainWindow(HINSTANCE instance)
{
    WNDCLASSEXW wc;
    ZeroMemory(&wc, sizeof(WNDCLASSEXW));
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = MainProc;
    wc.hInstance = instance;
    wc.lpszClassName = gMsgHandler;
    wc.style = CS_NOCLOSE;

    if (!RegisterClassExW(&wc))
    {
        ErrorHandler::Error(ErrorHandler::Level::Critical, L"Failed to register nCore's window class!");
        return false;
    }

    ghWndMsgHandler = CreateWindowExW(WS_EX_TOOLWINDOW, gMsgHandler, L"",
        WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0, 0, 0, 0, nullptr, nullptr, instance, nullptr);

    if (!ghWndMsgHandler)
    {
        ErrorHandler::Error(ErrorHandler::Level::Critical, L"Failed to create nCore's message handler!");
        UnregisterClassW(gMsgHandler, instance);
        return false;
    }

    // Let other modules get the instance using FindWindow + GetWindowLongPtr()
    SetWindowLongPtrW(ghWndMsgHandler, GWLP_USERDATA, (LONG_PTR)instance);
    SendMessageW(LiteStep::GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)ghWndMsgHandler, (LPARAM)gLSMessages);

    return true;
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="hWnd">The window the message is for.</param>
/// <param name="uMsg">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case LM_GETREVID:
        {
            size_t length;
            StringCchPrintfW((LPWSTR)lParam, 64, L"%s: ", TEXT(MODULE_NAME));
			length = lstrlenW((LPWSTR)lParam);
            GetVersionString(MakeVersion(MODULE_VERSION), (LPWSTR)lParam + length, 64 - length, false);
            
            if (SUCCEEDED(StringCchLengthW((LPWSTR)lParam, 64, &length)))
            {
                return length;
            }

            lParam = NULL;
        }
        return 0;

    case LM_REFRESH:
        {
        }
        return 0;

    case WM_TIMER:
        {
            if (wParam == timeTimer)
            {
                DynamicTextChangeNotification(L"Time", 0);
                DynamicTextChangeNotification(L"Time", 1);
                DynamicTextChangeNotification(L"WindowTitle", 1);
            }
        }
        return 0;
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}
