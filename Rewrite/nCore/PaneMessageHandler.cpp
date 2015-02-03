#include "Pane.hpp"

#include "../nUtilities/Windows.h"

#include <assert.h>
#include <windowsx.h>


LRESULT WINAPI Pane::ExternWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  return ((Pane*)GetWindowLongPtr(window, 0))->HandleMessage(window, message, wParam, lParam, 0);
}


LRESULT WINAPI Pane::InitWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  if (message == WM_CREATE) {
    SetWindowLongPtr(window, 0, (LONG_PTR)((LPCREATESTRUCT)lParam)->lpCreateParams);
    SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)&Pane::ExternWindowProc);
    return ExternWindowProc(window, message, wParam, lParam);
  }
  return DefWindowProc(window, message, wParam, lParam);
}


LRESULT Pane::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, NPARAM) {
  // Forward mouse messages to the lowest level child window which the mouse is over.
  if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) {
    int xPos = GET_X_LPARAM(lParam);
    int yPos = GET_Y_LPARAM(lParam);

    IMessageHandler *handler = nullptr; // TODO(Erik): Mouse capture

    if (handler == nullptr) {
      for (Pane *child : mChildren) {
        // TODO(Erik): ClickThrough
        D2D1_RECT_F pos = child->mRenderingPosition;
        if (xPos >= pos.left && xPos <= pos.right && yPos >= pos.top && yPos <= pos.bottom) {
          handler = child;
          break;
        }
      }
    }

    // Just let our message handler deal with it.
    if (handler == nullptr) {
      handler = mMessageHandler;
    }

    return handler->HandleMessage(window, msg, wParam, lParam, (NPARAM)this);
  }

  switch (msg) {
  case WM_ERASEBKGND:
    return 1;

  case WM_NCPAINT:
    return 0;

  case WM_PAINT:
    {
      RECT updateRect;
      if (GetUpdateRect(window, &updateRect, FALSE) != FALSE) {
        if (ReCreateDeviceResources() == S_OK) {
          D2D1_RECT_F d2dUpdateRect = D2D1::RectF((FLOAT)updateRect.left, (FLOAT)updateRect.top,
            (FLOAT)updateRect.right, (FLOAT)updateRect.bottom);

          mRenderTarget->BeginDraw();
          mRenderTarget->PushAxisAlignedClip(&d2dUpdateRect, D2D1_ANTIALIAS_MODE_ALIASED);
          mRenderTarget->Clear();
          Paint(mRenderTarget, &d2dUpdateRect);
          mRenderTarget->PopAxisAlignedClip();

          HRESULT hr = mRenderTarget->EndDraw();

          if (hr == D2DERR_RECREATE_TARGET) {
            DiscardDeviceResources();
            InvalidateRect(mWindow, nullptr, FALSE);
            UpdateWindow(mWindow);
          } else if (SUCCEEDED(hr)) {
            ValidateRect(window, &updateRect);
          } else {
            // TODO(Erik): Deal with this.
            assert(false);
          }
        }
      }
    }
    return 0;

  case WM_WINDOWPOSCHANGING:
    {
      if (mSettings.alwaysOnTop) {
        LPWINDOWPOS windowPos = LPWINDOWPOS(lParam);
        // TODO(Erik): Handle covered by fullscreen.
        windowPos->hwndInsertAfter = HWND_TOPMOST;
      }
    }
    break;
  }

  return mMessageHandler->HandleMessage(window, msg, wParam, lParam, (NPARAM)(IPane*)this);
}