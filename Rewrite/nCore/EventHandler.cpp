#include "EventHandler.hpp"

#include "../nUtilities/Macros.h"

EXPORT_CDECL(IEventHandler*) CreateEventHandler(const ISettingsReader *settingsReader)
{
  return new EventHandler(settingsReader);
}


EventHandler::EventHandler(const ISettingsReader *settingsReader)
{
}


void EventHandler::Destroy() {
  delete this;
}

LRESULT EventHandler::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam,
    IEventProcessor*)
{
  return DefWindowProc(window, msg, wParam, lParam);
};