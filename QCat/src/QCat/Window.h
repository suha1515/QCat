#pragma once
#include "qcpch.h"
#include "QCat/QCatCore.h"
#include "QCat/Events/Event.h"

namespace QCat
{
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "QCat Engine",
			unsigned int width = 1280,
			unsigned int height = 720)
			: Title(title), Width(width), Height(height)
		{
 		}
	};
	class QCAT_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window(){}

		virtual void OnBegin() = 0;
		virtual void OnEnd() = 0;
		virtual bool OnMessageUpdate()=0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual void* GetNativeWindow()=0;

		//  Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual void SetWindowSize(unsigned int width, unsigned int hieght) = 0;
		virtual bool IsVSync() const = 0;

		
		static Window* Create(const WindowProps& props = WindowProps());

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};
	protected:
		WindowData m_data;
	};
}