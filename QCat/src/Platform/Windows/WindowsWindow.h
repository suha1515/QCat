#pragma once
#include "QCat/Core/Window.h"
#include "QCat/InputDevice/KeyBoard/KeyBoard.h"
#include "QCat/InputDevice/Mouse/Mouse.h"

#include "QCat/Renderer/Graphics.h"


namespace QCat
{
	class WindowsWindow :public Window
	{
	public:
		struct WindowMessage
		{
			HWND hwnd;
			UINT msg; 
			WPARAM wParam; 
			LPARAM lParam;
		};
		WindowsWindow(int width, int height, const char* pWndName);
		WindowsWindow(const WindowsWindow&) = delete;
		WindowsWindow& operator=(const WindowsWindow&) = delete;
		~WindowsWindow();

		Graphics* Gfx();

		static LRESULT CALLBACK MsgSetUp(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		static LRESULT CALLBACK MsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		LRESULT MsgHandle(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

		virtual bool OnMessageUpdate() override;
		std::optional<int> UpdateMessage() noexcept;
	public:
		HWND GetHandle() noexcept;


		int GetKeyState(int keycode) noexcept { return keyboard.GetKey(keycode); }
		int GetMouseState(int button) noexcept { return mouse.GetButton(button);}
		std::pair<int, int> GetMousePos() noexcept { return mouse.GetMousePos(); } ;
		WindowMessage GetMessage();
	public:
		void SetICon(HICON ico);

		// Window을(를) 통해 상속됨
		virtual void OnBegin() override;
		virtual void OnEnd() override;

 		virtual uint32_t GetWidth() const override;
		virtual uint32_t GetHeight() const override;
		inline virtual void* GetNativeWindow() { return this; };

		inline void SetEventCallback(const EventCallbackFn& callback) override { m_data.EventCallback = callback; }
		virtual void SetVSync(bool enabled) override;
		virtual void SetWindowSize(unsigned int width, unsigned int hieght);
		virtual bool IsVSync() const override;


	private:
		Keyboard keyboard;
		MouseDevice mouse;
	private:
		uint32_t width = 1280;
		uint32_t height = 720;
		HWND hWnd;
		HINSTANCE hInstance;
		const char* wndClassName;
		const char* wndName;
		bool windowOpen;
		bool initialized = false;

		std::unique_ptr<Graphics> pGfx;
	
	};
}
