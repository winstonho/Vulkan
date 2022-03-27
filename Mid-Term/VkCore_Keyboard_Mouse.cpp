#include "VkCore_Keyboard_Mouse.hpp"
#include "VkCore_WindowCopy.hpp"

#include "Shobjidl.h"
#include "dwmapi.h"
#include <windowsx.h>

//#include <wintab/wintab.h>
//#include <wintab/PKTDEF.h>

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT)0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT)0x02)
#endif

RAWINPUTDEVICE Rid[1];

// some sizing border definitions

#define MINX 200
#define MINY 200
#define BORDERWIDTH 5
#define TITLEBARHEIGHT 32

namespace VkHelper
{
    EventQueue::EventQueue() { initialized = false; }

    void EventQueue::update()
    {
        MSG msg = {};

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // Translate virtual key messages
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    LRESULT EventQueue::pushEvent(MSG msg, Window* window)
    {
        UINT message = msg.message;
        LRESULT result = 0;
        // TODO: hwnd to xwin::Window unordered_map, when xwin::Window closes, it
        // sends a message to the event queue to remove that hwnd
        // and any remaining events that match that xwin::Window

        if (!initialized)
        {
            initialized = true;
            Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
            Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
            Rid[0].dwFlags = RIDEV_INPUTSINK;
            Rid[0].hwndTarget = window->m_hwnd;
            RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
        }

        Event e = Event(EventType::None, window);

        switch (message)
        {
        //case WM_CREATE:
        //{
        //    e = Event(EventType::Create, window);
        //    // repaint window when borderless to avoid titlebar.
        //    if (!window->getDesc().frame)
        //    {
        //        RECT lpRect;
        //        GetWindowRect(window->m_hwnd, &lpRect);
        //        SetWindowPos(window->m_hwnd, 0, lpRect.left, lpRect.top,
        //            lpRect.right - lpRect.left,
        //            lpRect.bottom - lpRect.top - TITLEBARHEIGHT, 0);
        //    }
        //    break;
        //}
        //case WM_PAINT:
        //{
        //    PAINTSTRUCT ps;
        //    BeginPaint(window->m_hwnd, &ps);
        //    RECT rect;
        //    GetWindowRect(window->m_hwnd, &rect);
        //    int cxWidth = rect.right - rect.left;
        //    int cyHeight = rect.bottom - rect.top;
        //    unsigned bg = window->getDesc().backgroundColor;
        //    unsigned r = (bg & 0xff000000) >> 24;
        //    unsigned g = (bg & 0x00ff0000) >> 16;
        //    unsigned b = (bg & 0x0000ff00) >> 8;
        //    HBRUSH BorderBrush = CreateSolidBrush(RGB(r, g, b));
        //    FillRect(ps.hdc, &rect, BorderBrush);
        //    EndPaint(window->m_hwnd, &ps);

        //    e = VkHelper::Event(VkHelper::EventType::Paint, window);
        //    break;
        //}
        //case WM_ERASEBKGND:
        //{
        //    break;
        //}
        //case WM_CLOSE:
        //case WM_DESTROY:
        //{
        //    e = Event(EventType::Close, window);
        //    break;
        //}
        //case WM_SETFOCUS:
        //{
        //    e = Event(FocusData(true), window);
        //    break;
        //}
        //case WM_KILLFOCUS:
        //{
        //    e = Event(FocusData(false), window);
        //    break;
        //}

        case WM_MOUSEWHEEL:
        {
            short modifiers = LOWORD(msg.wParam);
            e = Event(
                MouseWheelData(
                    GET_WHEEL_DELTA_WPARAM(msg.wParam) / WHEEL_DELTA,
                    ModifierState(modifiers & MK_CONTROL, modifiers & MK_ALT,
                        modifiers & MK_SHIFT, modifiers & 0)),
                window);
            break;
        }
        case WM_LBUTTONDOWN:
        {
            short modifiers = LOWORD(msg.wParam);
            e = Event(
                MouseInputData(
                    MouseInput::Left, ButtonState::Pressed,
                    ModifierState(modifiers & MK_CONTROL, modifiers & MK_ALT,
                        modifiers & MK_SHIFT, modifiers & 0)),
                window);
            break;
        }
        case WM_MBUTTONDOWN:
        {
            short modifiers = LOWORD(msg.wParam);
            e = Event(
                MouseInputData(
                    MouseInput::Middle, ButtonState::Pressed,
                    ModifierState(modifiers & MK_CONTROL, modifiers & MK_ALT,
                        modifiers & MK_SHIFT, modifiers & 0)),
                window);
            break;
        }
        case WM_RBUTTONDOWN:
        {
            short modifiers = LOWORD(msg.wParam);
            e = Event(
                MouseInputData(
                    MouseInput::Right, ButtonState::Pressed,
                    ModifierState(modifiers & MK_CONTROL, modifiers & MK_ALT,
                        modifiers & MK_SHIFT, modifiers & 0)),
                window);
            break;
        }
        case WM_XBUTTONDOWN:
        {
            short modifiers = LOWORD(msg.wParam);
            short x = HIWORD(msg.wParam);
            e = Event(
                MouseInputData(
                    x & XBUTTON1 ? MouseInput::Button4 : MouseInput::Button5,
                    ButtonState::Pressed,
                    ModifierState(modifiers & MK_CONTROL, modifiers & MK_ALT,
                        modifiers & MK_SHIFT, modifiers & 0)),
                window);
            break;
        }
        case WM_XBUTTONUP:
        {
            short modifiers = LOWORD(msg.wParam);
            short x = HIWORD(msg.wParam);
            e = Event(
                MouseInputData(
                    x & XBUTTON1 ? MouseInput::Button4 : MouseInput::Button5,
                    ButtonState::Released,
                    ModifierState(modifiers & MK_CONTROL, modifiers & MK_ALT,
                        modifiers & MK_SHIFT, modifiers & 0)),
                window);
            break;
        }
        case WM_LBUTTONDBLCLK:
            // Perhaps there should be an event for this in the future
            break;
        case WM_LBUTTONUP:
        {
            short modifiers = LOWORD(msg.wParam);
            e = Event(
                MouseInputData(
                    MouseInput::Left, ButtonState::Released,
                    ModifierState(modifiers & MK_CONTROL, modifiers & MK_ALT,
                        modifiers & MK_SHIFT, modifiers & 0)),
                window);
            break;
        }
        case WM_MBUTTONUP:
        {
            short modifiers = LOWORD(msg.wParam);
            e = Event(
                MouseInputData(
                    MouseInput::Middle, ButtonState::Released,
                    ModifierState(modifiers & MK_CONTROL, modifiers & MK_ALT,
                        modifiers & MK_SHIFT, modifiers & 0)),
                window);
            break;
        }
        case WM_RBUTTONUP:
        {
            short modifiers = LOWORD(msg.wParam);
            e = Event(
                MouseInputData(
                    MouseInput::Right, ButtonState::Released,
                    ModifierState(modifiers & MK_CONTROL, modifiers & MK_ALT,
                        modifiers & MK_SHIFT, modifiers & 0)),
                window);
            break;
        }
        case WM_INPUT:
        {
            UINT dwSize;

            GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, NULL, &dwSize,
                sizeof(RAWINPUTHEADER));
            LPBYTE lpb = new BYTE[dwSize];
            if (lpb == NULL)
            {
                // return 0;
            }

            if (GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, lpb, &dwSize,
                sizeof(RAWINPUTHEADER)) != dwSize)
                OutputDebugString(
                    TEXT("GetRawInputData does not return correct size !\n"));

            RAWINPUT* raw = (RAWINPUT*)lpb;

            if (raw->header.dwType == RIM_TYPEKEYBOARD)
            {

                raw->data.keyboard.MakeCode, raw->data.keyboard.Flags,
                    raw->data.keyboard.Reserved,
                    raw->data.keyboard.ExtraInformation, raw->data.keyboard.Message,
                    raw->data.keyboard.VKey;
            }
            else if (raw->header.dwType == RIM_TYPEMOUSE)
            {

                raw->data.mouse.usFlags, raw->data.mouse.ulButtons,
                    raw->data.mouse.usButtonFlags, raw->data.mouse.usButtonData,
                    raw->data.mouse.ulRawButtons, raw->data.mouse.lLastX,
                    raw->data.mouse.lLastY, raw->data.mouse.ulExtraInformation;

                e = Event(
                    MouseRawData(static_cast<int>(raw->data.mouse.lLastX),
                        static_cast<int>(raw->data.mouse.lLastY)),
                    window);

                // printf("%.3f, %.3f\n",
                // raw->data.mouse.lLastX,raw->data.mouse.lLastY)
            }

            delete[] lpb;
            break;
        }
        case WM_MOUSEMOVE:
        {
            HWND hwnd = window->m_hwnd;
            // Extract the mouse local coordinates
            int x = static_cast<short>(LOWORD(msg.lParam));
            int y = static_cast<short>(HIWORD(msg.lParam));

            // Get the client area of the window
            RECT area;
            GetClientRect(hwnd, &area);
  

            e = Event(
                MouseMoveData(
                    static_cast<unsigned>(area.left <= x && x <= area.right
                        ? x - area.left
                        : 0xFFFFFFFF),
                    static_cast<unsigned>(area.top <= y && y <= area.bottom
                        ? y - area.top
                        : 0xFFFFFFFF),
                    static_cast<unsigned>(x), static_cast<unsigned>(y),
                    static_cast<int>(x - prevMouseX),
                    static_cast<int>(y - prevMouseY)),
                window);
            prevMouseX = static_cast<unsigned>(x);
            prevMouseY = static_cast<unsigned>(y);
            break;
        }
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_CHAR:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        {
            Key d;

            switch (msg.wParam)
            {
            case VK_ESCAPE:
                d = Key::Escape;
                break;
            case 0x30:
                d = Key::Num0;
                break;
            case 0x31:
                d = Key::Num1;
                break;
            case 0x32:
                d = Key::Num2;
                break;
            case 0x33:
                d = Key::Num3;
                break;
            case 0x34:
                d = Key::Num4;
                break;
            case 0x35:
                d = Key::Num5;
                break;
            case 0x36:
                d = Key::Num6;
                break;
            case 0x37:
                d = Key::Num7;
                break;
            case 0x38:
                d = Key::Num8;
                break;
            case 0x39:
                d = Key::Num9;
                break;
            case 0x41:
                d = Key::A;
                break;
            case 0x42:
                d = Key::B;
                break;
            case 0x43:
                d = Key::C;
                break;
            case 0x44:
                d = Key::D;
                break;
            case 0x45:
                d = Key::E;
                break;
            case 0x46:
                d = Key::F;
                break;
            case 0x47:
                d = Key::G;
                break;
            case 0x48:
                d = Key::H;
                break;
            case 0x49:
                d = Key::I;
                break;
            case 0x4A:
                d = Key::J;
                break;
            case 0x4B:
                d = Key::K;
                break;
            case 0x4C:
                d = Key::L;
                break;
            case 0x4D:
                d = Key::M;
                break;
            case 0x4E:
                d = Key::N;
                break;
            case 0x4F:
                d = Key::O;
                break;
            case 0x50:
                d = Key::P;
                break;
            case 0x51:
                d = Key::Q;
                break;
            case 0x52:
                d = Key::R;
                break;
            case 0x53:
                d = Key::S;
                break;
            case 0x54:
                d = Key::T;
                break;
            case 0x55:
                d = Key::U;
                break;
            case 0x56:
                d = Key::V;
                break;
            case 0x57:
                d = Key::W;
                break;
            case 0x58:
                d = Key::X;
                break;
            case 0x59:
                d = Key::Y;
                break;
            case 0x5A:
                d = Key::Z;
                break;
            case VK_SUBTRACT:
            case VK_OEM_MINUS:
                d = Key::Minus;
                break;
            case VK_ADD:
            case VK_OEM_PLUS:
                d = Key::Add;
                break;
            case VK_MULTIPLY:
                d = Key::Multiply;
                break;
            case VK_DIVIDE:
                d = Key::Divide;
                break;
            case VK_BACK:
                d = Key::Back;
                break;
            case VK_RETURN:
                d = Key::Enter;
                break;
            case VK_DELETE:
                d = Key::Del;
                break;
            case VK_TAB:
                d = Key::Tab;
                break;
            case VK_NUMPAD0:
                d = Key::Numpad0;
                break;
            case VK_NUMPAD1:
                d = Key::Numpad1;
                break;
            case VK_NUMPAD2:
                d = Key::Numpad2;
                break;
            case VK_NUMPAD3:
                d = Key::Numpad3;
                break;
            case VK_NUMPAD4:
                d = Key::Numpad4;
                break;
            case VK_NUMPAD5:
                d = Key::Numpad5;
                break;
            case VK_NUMPAD6:
                d = Key::Numpad6;
                break;
            case VK_NUMPAD7:
                d = Key::Numpad7;
                break;
            case VK_NUMPAD8:
                d = Key::Numpad8;
                break;
            case VK_NUMPAD9:
                d = Key::Numpad9;
                d = Key::Numpad9;
                break;
            case VK_UP:
                d = Key::Up;
                break;
            case VK_LEFT:
                d = Key::Left;
                break;
            case VK_DOWN:
                d = Key::Down;
                break;
            case VK_RIGHT:
                d = Key::Right;
                break;
            case VK_SPACE:
                d = Key::Space;
                break;
            case VK_HOME:
                d = Key::Home;
                break;
            case VK_F1:
                d = Key::F1;
                break;
            case VK_F2:
                d = Key::F2;
                break;
            case VK_F3:
                d = Key::F3;
                break;
            case VK_F4:
                d = Key::F4;
                break;
            case VK_F5:
                d = Key::F5;
                break;
            case VK_F6:
                d = Key::F6;
                break;
            case VK_F7:
                d = Key::F7;
                break;
            case VK_F8:
                d = Key::F8;
                break;
            case VK_F9:
                d = Key::F9;
                break;
            case VK_F10:
                d = Key::F10;
                break;
            case VK_F11:
                d = Key::F11;
                break;
            case VK_F12:
                d = Key::F12;
                break;
            case VK_SHIFT:
            case VK_LSHIFT:
            case VK_RSHIFT:
                d = Key::LShift;
                break;
            case VK_CONTROL:
            case VK_LCONTROL:
            case VK_RCONTROL:
                d = Key::LControl;
                break;
            case VK_MENU:
            case VK_LMENU:
            case VK_RMENU:
                d = Key::LAlt;
                break;
            case VK_OEM_PERIOD:
                d = Key::Period;
                break;
            case VK_OEM_COMMA:
                d = Key::Comma;
                break;
            case VK_OEM_1:
                d = Key::Semicolon;
                break;
            case VK_OEM_2:
                d = Key::Backslash;
                break;
            case VK_OEM_3:
                d = Key::Grave;
                break;
            case VK_OEM_4:
                d = Key::LBracket;
                break;
            case VK_OEM_6:
                d = Key::RBracket;
                break;
            case VK_OEM_7:
                d = Key::Apostrophe;
                break;
            default:
                d = Key::KeysMax;
                break;
            }
            if (d == Key::LControl && GetKeyState(VK_RCONTROL))
            {
                d = Key::RControl;
            }
            if (d == Key::LAlt && GetKeyState(VK_RMENU))
            {
                d = Key::RAlt;
            }
            if (d == Key::LShift && GetKeyState(VK_RSHIFT))
            {
                d = Key::RShift;
            }
            short modifiers = LOWORD(msg.wParam);
            ModifierState ms;
            ms.shift = (GetKeyState(VK_SHIFT) & 0x8000) |
                (GetKeyState(VK_CAPITAL) & 0x0001);
            ms.alt = GetKeyState(VK_MENU) & 0x8000;
            ms.ctrl = GetKeyState(VK_CONTROL) & 0x8000;
            ms.meta = false;

            // This may or may not be a good idea, adding combination keys
            // that happen with shift. Keyboards are also important to consider
            // here. ~ Alain
            if (ms.shift)
            {
                if (d == Key::Semicolon)
                {
                    d = Key::Colon;
                }
                if (d == Key::Apostrophe)
                {
                    d = Key::Quotation;
                }
            }

            if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
            {
                e = Event(KeyboardData(d, ButtonState::Pressed, ms), window);
            }
            else if (message == WM_KEYUP || message == WM_SYSKEYUP)
            {
                e = Event(KeyboardData(d, ButtonState::Released, ms), window);
            }
            break;
        }
        case WM_SIZE:
        {
            unsigned width, height;
            width = static_cast<unsigned>((UINT64)msg.lParam & 0xFFFF);
            height = static_cast<unsigned>((UINT64)msg.lParam >> 16);

            e = Event(ResizeData(width, height, false), window);
            break;
        }
        case WM_SIZING:
        {
            unsigned width, height;
            unsigned STEP = 1;
            PRECT rectp = (PRECT)msg.lParam;
            HWND hwnd = window->m_hwnd;

            // Get the window and client dimensions
            tagRECT wind, rect;
            GetWindowRect(hwnd, &wind);
            GetClientRect(hwnd, &rect);
            width = rectp->right - rectp->left;
            height = rectp->bottom - rectp->top;

            // Redraw window to refresh it while resizing
            RedrawWindow(hwnd, NULL, NULL,
                RDW_INVALIDATE | RDW_NOERASE | RDW_INTERNALPAINT);

            e = Event(ResizeData(width, height, true), window);
            result = WVR_REDRAW;
            break;
        }
        case WM_MOVING:
        {
            break;
        }
        // break;
        default:
            // Do nothing
            break;
        }
        if (e.type != EventType::None)
        {
            mQueue.emplace(e);
        }
        window->executeEventCallback(e);
        return result;
    }

    const Event& EventQueue::front() { return mQueue.front(); }

    void EventQueue::pop() { mQueue.pop(); }

    bool EventQueue::empty() { return mQueue.empty(); }
    size_t EventQueue::size() { return mQueue.size(); }
}