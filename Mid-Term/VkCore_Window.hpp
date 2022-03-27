#pragma once
#include <Windows.h>
#include <functional>
#include "VkCore_Keyboard_Mouse.hpp"

namespace VkHelper {
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	class Window
	{
	public:
		inline static int _width = 640;
		inline static int _height = 480;
		
		Window();
		Window(const Window&) = delete;
		Window& operator =(const Window&) = delete;
		~Window();

		LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);//, Window* window);

		void executeEventCallback(const Event e);
		std::function<void(const Event e)> mCallback;

		bool ProcessMessages();
		bool running;
		//bool framebufferResized = false;
	//private:
		HINSTANCE m_hInstance;
		HWND m_hwnd;

	};
}