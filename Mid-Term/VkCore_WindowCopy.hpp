#pragma once
#include <Windows.h>
#include "VkCore_Keyboard_Mouse.hpp"
#include "Event.hpp"

#include <functional>

LRESULT CALLBACK WindowProcStatic(HWND hwnd, UINT msg, WPARAM wparam,
	LPARAM lparam);

namespace VkHelper
{
	class Window
	{
	public:
		inline static int _width = 800;
		inline static int _height = 600;

		Window();
		Window(const Window&) = delete;
		Window& operator =(const Window&) = delete;
		~Window();

		//bool WindowResized() { return framebufferResized };
		//void ResetWindowResizedFlag() { framebufferResized = false; }
		//static void framebufferResizedCallback(Window pwindow, int width, int height);
		LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

		void executeEventCallback(const VkHelper::Event e);

		std::function<void(const VkHelper::Event e)> mCallback;
		bool ProcessMessages();
		bool running;
		//bool framebufferResized = false;
	//private:
		HINSTANCE m_hInstance;
		HWND m_hwnd;
		EventQueue* mEventQueue;
	};

	static thread_local Window* _windowBeingCreated = nullptr;
	static thread_local std::unordered_map<HWND, Window*> _hwndMap = {};
}