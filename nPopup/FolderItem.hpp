/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  FolderItem.hpp
 *  The nModules Project
 *
 *  A static folder.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "PopupItem.hpp"
#include "Popup.hpp"

class FolderItem : public PopupItem {
public:
    explicit FolderItem(LPCSTR title, Popup* popup, LPCSTR customIcon = NULL);
    virtual ~FolderItem();

private:
    Popup* popup;
};
