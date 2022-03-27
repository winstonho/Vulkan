#include "VkCore_WindowCopy.hpp"
#include <iostream>

namespace VkHelper
{


	Window* _this;

LRESULT CALLBACK WindowProcStatic(HWND hwnd, UINT msg, WPARAM wparam,
	LPARAM lparam)
{
	if (_windowBeingCreated != nullptr)
	{
		_hwndMap.emplace(hwnd, _windowBeingCreated);
		_windowBeingCreated->m_hwnd = hwnd;
		_this = _windowBeingCreated;
		_windowBeingCreated = nullptr;
	}
	else
	{
		auto existing = _hwndMap.find(hwnd);
		_this = existing->second;
	}

	return _this->WindowProc(msg, wparam, lparam);
}

LRESULT  Window::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//Window _this;
	MSG msg{};
	LRESULT result = 0;

	//std::cout << "step 2" << std::endl;
	Event e = Event(EventType::None, _this);

	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(m_hwnd);
		break;
	case WM_SIZE:
	{
		Window::_width = LOWORD(lParam);
		Window::_height = HIWORD(lParam);
		break;
	}
	case WM_SIZING:
	{
		unsigned width, height;
		unsigned STEP = 1;
		PRECT rectp = (PRECT)msg.lParam;
		//HWND hwnd = hwnd;

		// Get the window and client dimensions
		tagRECT wind, rect;
		GetWindowRect(m_hwnd, &wind);
		GetClientRect(m_hwnd, &rect);
		width = rectp->right - rectp->left;
		height = rectp->bottom - rectp->top;

		//redraw window to refresh it while resizing
		RedrawWindow(m_hwnd, NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_INTERNALPAINT);
		//std::cout << "step 3" << std::endl;
		e = VkHelper::Event(ResizeData(width, height, true), _this);
		//std::cout << "step 4" << std::endl;
		result = WVR_REDRAW;
		//std::cout << "step 5" << std::endl;
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	MSG message;
	message.hwnd = m_hwnd;
	message.lParam = lParam;
	message.wParam = wParam;
	message.message = uMsg;
	message.time = 0;

	if(mEventQueue != nullptr)
	{
		LRESULT result = mEventQueue->pushEvent(message, this);
			if (result > 0) return result;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void Create_PixelFormat(HDC& handleDCOut)
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		// -- means not very important
		sizeof(PIXELFORMATDESCRIPTOR), // -- size
		1, // nVersion
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //Flags
		PFD_TYPE_RGBA, // The kind of framebuffer. RGBA or palette.
		32, // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0, // -- Color bits ignored
		0, // -- No alpha buffer
		0, // -- Shift Bit Ignored
		0, // -- No Accumultion Buffer
		0, 0, 0, 0, // -- Accumulation Bits Ignored
		24, // Number of bits for the depthbuffer
		8, // Number of bits for the stencilbuffer
		0, // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE, // Main drawing layer
		0, // -- Reserved
		0, 0, 0 // -- layer mask ignored
	};



	int ourPixelFormat;
	//Attempt to Find A Matching Pixel Format
	ourPixelFormat = ChoosePixelFormat(handleDCOut, &pfd);
	if (ourPixelFormat == 0)
	{
		MessageBox(NULL, L"ChoosePixelFormat() failed", L"Error", MB_OK);
		exit(1);
	}
	//Attempt to set Pixel format
	if (!SetPixelFormat(handleDCOut, ourPixelFormat, &pfd))
	{
		MessageBox(NULL, L"SetPixelFormat() failed", L"Error", MB_OK);
		exit(1);
	}
}

void Window::executeEventCallback(const VkHelper::Event e)
{
	if (mCallback)mCallback(e);
}

Window::Window()
	: m_hInstance(GetModuleHandle(nullptr))
{
	const wchar_t* CLASS_NAME = L"Vulkan Win32 Window";
	WNDCLASS wndClass = {};
	wndClass.lpszClassName = CLASS_NAME;
	wndClass.hInstance = m_hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.lpfnWndProc = WindowProcStatic;

	RegisterClass(&wndClass);

	DWORD style = WS_MINIMIZEBOX | WS_SIZEBOX | WS_OVERLAPPEDWINDOW ;

	/*int width = 640;
	int height = 480;*/

	RECT rect;
	rect.left = 250;
	rect.right = rect.left + _width;
	rect.top = 250;
	rect.bottom = rect.top + _width;

	AdjustWindowRect(&rect, style, false);
	_windowBeingCreated = this;
	m_hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Title",
		style,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		NULL,
		NULL,
		m_hInstance,
		NULL
	);
	HDC dc = GetDC(m_hwnd);
	Create_PixelFormat(dc);
	ShowWindow(m_hwnd, SW_SHOW);
}

//static void framebufferResizedCallback(Window pwindow, int width, int height) 
//{
//	Window::Window().framebufferResized = true;
//}


Window::~Window()
{
	ReleaseDC(m_hwnd, GetDC(m_hwnd)); //Release device context
	DestroyWindow(m_hwnd); //destroy window handle

	const wchar_t* CLASS_NAME = L"Vulkan Win32 Window";
	UnregisterClass(CLASS_NAME, m_hInstance);

}

bool Window::ProcessMessages()
{
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return true;
}

}