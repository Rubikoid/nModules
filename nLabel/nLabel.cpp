/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  nLabel.cpp
 *  The nModules Project
 *
 *  Main .cpp file for the nLabel module.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"
#include "../nShared/DrawableBangs.h"
#include "Label.hpp"
#include "nLabel.h"
#include <map>
#include <strsafe.h>
#include "Version.h"

using std::map;

// The LSModule class
LSModule gLSModule(MODULE_NAME, MODULE_AUTHOR, MakeVersion(MODULE_VERSION));

// The messages we want from the core
UINT gLSMessages[] = { LM_GETREVID, LM_REFRESH, 0 };

// All the top-level labels we currently have loaded
map<string, Label*> g_TopLevelLabels;

// All the labels we currently have loaded. Labels add and remove themselfs from this list.
map<string, Label*> g_AllLabels;


/// <summary>
/// Called by the LiteStep core when this module is loaded.
/// </summary>
int initModuleEx(HWND parent, HINSTANCE instance, LPCSTR /* path */) {
    if (!gLSModule.Initialize(parent, instance)) {
        return 1;
    }

    if (!gLSModule.ConnectToCore(MakeVersion(CORE_VERSION))) {
        return 1;
    }

    // Load settings
    LoadSettings();

    DrawableBangs::_Register("nLabel", FindDrawable);

    return 0;
}


/// <summary>
/// Called by the LiteStep core when this module is about to be unloaded.
/// </summary>
void quitModule(HINSTANCE /* instance */) {
    DrawableBangs::_UnRegister("nLabel");

    // Remove all labels
    for (auto label : g_TopLevelLabels) {
        delete label.second;
    }
    
    gLSModule.DeInitalize();

    g_TopLevelLabels.clear();
}


/// <summary>
/// Handles the main window's messages.
/// </summary>
/// <param name="window">The window the message is for.</param>
/// <param name="message">The type of message.</param>
/// <param name="wParam">wParam</param>
/// <param name="lParam">lParam</param>
LRESULT WINAPI LSMessageHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
    case WM_CREATE:
        {
            SendMessage(LiteStep::GetLitestepWnd(), LM_REGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
        }
        return 0;

    case WM_DESTROY:
        {
            SendMessage(LiteStep::GetLitestepWnd(), LM_UNREGISTERMESSAGE, (WPARAM)window, (LPARAM)gLSMessages);
        }
        return 0;

    case LM_REFRESH:
        {
        }
        return 0;
    }
    return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// Reads through the .rc files and creates labels.
/// </summary>
void LoadSettings() {
    char szLine[MAX_LINE_LENGTH], szLabel[256];
    LPSTR szTokens[] = { szLabel };
    LPVOID f = LiteStep::LCOpen(NULL);

    while (LiteStep::LCReadNextConfig(f, "*nLabel", szLine, sizeof(szLine))) {
        LiteStep::LCTokenize(szLine+strlen("*nLabel")+1, szTokens, 1, NULL);
        g_TopLevelLabels.insert(g_TopLevelLabels.begin(), std::pair<string, Label*>(string(szLabel), new Label(szLabel)));
    }
    LiteStep::LCClose(f);
}


Drawable *FindDrawable(LPCSTR prefix) {
    map<string, Label*>::iterator label = g_AllLabels.find(string(prefix));
    return label == g_AllLabels.end() ? nullptr : label->second;
}
