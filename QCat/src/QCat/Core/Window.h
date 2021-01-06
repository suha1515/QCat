#pragma once
#include "qcpch.h"
#include "QCat/Core/QCatCore.h"
#include "QCat/Events/Event.h"

namespace QCat
{
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(const std::string& title = "QCat Engine",
			uint32_t width = 1280,
			uint32_t height = 720)
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

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
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
			uint32_t Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};
	protected:
		WindowData m_data;
	};
}