#include "VkCore_Window.hpp"

namespace VkHelper {

	void Window::executeEventCallback(const Event e)
	{
		if (mCallback) mCallback(e);
	}

	LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		Window* window;
		LRESULT result = 0;
		Event e = Event(EventType::None, window);

		MSG msg{};
		switch (uMsg)
		{
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_SIZE:
		{
			Window::_width = LOWORD(lParam);
			Window::_height = HIWORD(lParam);
			break;
		}
		//case WM_SIZING:
		//{
		//	unsigned width, height;
		//	unsigned STEP = 1;
		//	PRECT rectp = (PRECT)msg.lParam;
		//	//HWND hwnd = hwnd;

		//	// Get the window and client dimensions
		//	tagRECT wind, rect;
		//	GetWindowRect(hwnd, &wind);
		//	GetClientRect(hwnd, &rect);
		//	width = rectp->right - rectp->left;
		//	height = rectp->bottom - rectp->top;

		//	//redraw window to refresh it while resizing
		//	/*RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_NOERASE | RDW_INTERNALPAINT);

		//	e = VkHelper::Event(ResizeData(width, height, true), window);
		//	result = WVR_REDRAW;*/
		//	break;
		//}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	/*	if (e.type != EventType::None)
		{
			mQueue.emplace(e);
		}*/
		window->executeEventCallback(e);
		//return result;
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

	Window::Window()
		: m_hInstance(GetModuleHandle(nullptr))
	{
		const wchar_t* CLASS_NAME = L"Vulkan Win32 Window";
		WNDCLASS wndClass = {};
		wndClass.lpszClassName = CLASS_NAME;
		wndClass.hInstance = m_hInstance;
		wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.lpfnWndProc = WindowProc;

		RegisterClass(&wndClass);

		DWORD style = WS_MINIMIZEBOX | WS_SIZEBOX | WS_OVERLAPPEDWINDOW;

		/*int width = 640;
		int height = 480;*/

		RECT rect;
		rect.left = 250;
		rect.right = rect.left + _width;
		rect.top = 250;
		rect.bottom = rect.top + _width;

		AdjustWindowRect(&rect, style, false);

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