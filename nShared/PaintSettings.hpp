/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PaintSettings.hpp                                               July, 2012
 *  The nModules Project
 *
 *  Function declarations for the PaintSettings class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef PAINTSETTINGS_HPP
#define PAINTSETTINGS_HPP

#include <d2d1.h>
#include <dwrite.h>
#include "Settings.hpp"

class PaintSettings {
public:
    explicit PaintSettings(LPCSTR prefix);
    virtual ~PaintSettings();

    PaintSettings* CreateChild(LPCSTR prefix);

    void Load();
    void OverLoad(LPCSTR prefix);

    // True if the window should be topmost.
    bool m_bAlwaysOnTop;

    RECT position;
    bool DWMBlur;
    D2D_COLOR_F backColor;

    LPCWSTR text;
    LPCWSTR font;
    D2D_COLOR_F fontColor;
    float fontSize;

    D2D_RECT_F textOffset;

    DWRITE_TEXT_ALIGNMENT textAlignment;
    DWRITE_PARAGRAPH_ALIGNMENT textVerticalAlignment;

    void setText(LPCWSTR);

    D2D_COLOR_F ARGBToD2DColor(DWORD argb);

    Settings* GetSettings();

private:
    explicit PaintSettings(LPCSTR prefix, Settings* settings);

	Settings* m_pSettings;
    LPCSTR m_pszPrefix;

};

#endif /* PAINTSETTINGS_HPP */
