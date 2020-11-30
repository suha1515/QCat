#include "qcpch.h"
#include "ImGuiLayer.h"
#include "imgui.h"

#include "QCat/Application.h"
#include "Platform/ImGui/Windows_Directx11/imgui_impl_dx11.h"
#include "Platform/ImGui/Windows_Directx11/imgui_impl_win32.h"

#include "Platform/Windows/WindowsWindow.h"
#include "API/DirectX11/QGfxDeviceDX11.h"

#include "QCat/Log.h"

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

		//ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(window->Gfx().GetDevice().Get(), window->Gfx().GetContext().Get());
	}
	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplDX11_Shutdown();
		//ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	void ImGuiLayer::OnUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::GetInstance();
		io.DisplaySize = ImVec2((float)app.GetWindow()->GetWidth(), (float)app.GetWindow()->GetHeight());
		io.DeltaTime = (float)timer.Mark();
		QCAT_CORE_INFO("Imgui Layer update");
		ImGui_ImplDX11_NewFrame();
		//ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
	void ImGuiLayer::OnEvent(Event& event)
	{
	}
}