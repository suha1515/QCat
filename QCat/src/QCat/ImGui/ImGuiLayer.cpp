#include "qcpch.h"
#include "ImGuiLayer.h"
#include "imgui.h"

#include "QCat/Application.h"
#include "Platform/ImGui/Windows_Directx11/imgui_impl_dx11.h"
#include "Platform/ImGui/Windows_Directx11/imgui_impl_win32.h"

#include "Platform/Windows/WindowsWindow.h"
#include "API/DirectX11/QGfxDeviceDX11.h"

#include "QCat/Log.h"

#define BIND_EVENT_FN(x) std::bind(&ImGuiLayer::x, this, std::placeholders::_1)

namespace QCat
{
	ImGuiLayer::ImGuiLayer()
		:Layer("ImGuiLayer")
	{
	}
	ImGuiLayer::~ImGuiLayer()
	{
	}
	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		
		WindowsWindow* window = dynamic_cast<WindowsWindow*>(Application::GetInstance().GetWindow());
		QCAT_CORE_ASSERT(window, "Imgui Attach Error :: Window Nullptr");
		HWND hwnd = window->GetHandle();
		//ImGuiIO& io = ImGui::GetIO();
		//io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
		//io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
		//io.BackendPlatformName = "imgui_impl_win32";

		//// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
		//io.KeyMap[ImGuiKey_Tab] = VK_TAB;
		//io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
		//io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
		//io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
		//io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
		//io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
		//io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
		//io.KeyMap[ImGuiKey_Home] = VK_HOME;
		//io.KeyMap[ImGuiKey_End] = VK_END;
		//io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
		//io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
		//io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
		//io.KeyMap[ImGuiKey_Space] = VK_SPACE;
		//io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
		//io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
		//io.KeyMap[ImGuiKey_KeyPadEnter] = VK_RETURN;
		//io.KeyMap[ImGuiKey_A] = 'A';
		//io.KeyMap[ImGuiKey_C] = 'C';
		//io.KeyMap[ImGuiKey_V] = 'V';
		//io.KeyMap[ImGuiKey_X] = 'X';
		//io.KeyMap[ImGuiKey_Y] = 'Y';
		//io.KeyMap[ImGuiKey_Z] = 'Z';

		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(window->Gfx().GetDevice().Get(), window->Gfx().GetContext().Get());
	}
	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	void ImGuiLayer::OnUpdate()
	{
		//ImGuiIO& io = ImGui::GetIO();
		//Application& app = Application::GetInstance();
		//io.DisplaySize = ImVec2((float)app.GetWindow()->GetWidth(), (float)app.GetWindow()->GetHeight());
		//io.DeltaTime = (float)timer.Mark();
		//io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		//QCAT_CORE_INFO("Imgui Layer update");
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	void ImGuiLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(OnMouseButtonPressEvent));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(OnMouseButtonReleaseEvent));
		dispatcher.Dispatch<MouseMoveEvent>(BIND_EVENT_FN(OnMouseButtonMoveEvent));
		dispatcher.Dispatch<MouseScrollEvent>(BIND_EVENT_FN(OnMouseButtonScrollEvent));
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(OnKeyPressedEvent));
		dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(OnKeyReleasedEvent));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResizedEvent));
	}
	bool ImGuiLayer::OnMouseButtonPressEvent(MouseButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.GetMouseButton()] = true;

		return false;
	}
	bool ImGuiLayer::OnMouseButtonReleaseEvent(MouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.GetMouseButton()] = false;

		return false;
	}
	bool ImGuiLayer::OnMouseButtonMoveEvent(MouseMoveEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.GetX(), e.GetY());

		return false;
	}
	bool ImGuiLayer::OnMouseButtonScrollEvent(MouseScrollEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += e.GetOffsetX();
		io.MouseWheel += e.GetOffsetY();

		return false;
	}
	bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		return false;
	}
	bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent& e)
	{
		return false;
	}
	bool ImGuiLayer::OnWindowResizedEvent(WindowResizeEvent& e)
	{
		//ImGuiIO& io = ImGui::GetIO();
		//io.DisplaySize = ImVec2((float)e.GetWidth(), (float)e.GetHeight());
		//io.DisplayFramebufferScale = ImVec2(1.0f,1.0f);
		//io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		return false;
	}
}