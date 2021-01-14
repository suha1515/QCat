#include "qcpch.h"
#include "ImGuiLayer.h"
#include "imgui.h"

#include "QCat/Core/Application.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_opengl3.h"

#include "QCat/Renderer/RenderCommand.h"

#include "Platform/Windows/WindowsWindow.h"

#include "API/DirectX11/QGfxDeviceDX11.h"
#include "API/Opengl/QCatOpengl.h"

//#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM


#include "QCat/Core/Log.h"

namespace QCat
{
	ImGuiLayer::ImGuiLayer()
		:Layer("ImGuiLayer")
	{
	}
	ImGuiLayer::~ImGuiLayer()
	{
		OnDetach();
	}
	void ImGuiLayer::OnAttach()
	{
		QCAT_PROFILE_FUNCTION();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		
		WindowsWindow* window = dynamic_cast<WindowsWindow*>(Application::GetInstance().GetWindow());
		QCAT_CORE_ASSERT(window, "Imgui Attach Error :: Window Nullptr");
		HWND hwnd = window->GetHandle();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Win

		io.Fonts->AddFontFromFileTTF("Asset/fonts/opensans/OpenSans-Bold.ttf", 18.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Asset/fonts/opensans/OpenSans-Regular.ttf", 18.0f);

		// When viewports are enabled we tweak WindowRounding / WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle & style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		//SetDarkThemeColors();
		SetNatrueThemeColors();
		ImGui_ImplWin32_Init(hwnd);
#if defined(QCAT_DX11)
		QGfxDeviceDX11* pgfx = dynamic_cast<QGfxDeviceDX11*>(window->Gfx());
		ImGui_ImplDX11_Init(pgfx->GetDevice().Get(), pgfx->GetContext().Get());
#elif defined(QCAT_OPENGL)
		ImGui_ImplOpenGL3_Init("#version 450");
#endif  
	
	}
	void ImGuiLayer::OnDetach()
	{
		QCAT_PROFILE_FUNCTION();

#if defined(QCAT_DX11)
		ImGui_ImplDX11_Shutdown();
#elif defined(QCAT_OPENGL)
		ImGui_ImplOpenGL3_Shutdown();
#endif  
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
	void ImGuiLayer::OnBegin()
	{
		QCAT_PROFILE_FUNCTION();
#if defined(QCAT_DX11)
		ImGui_ImplDX11_NewFrame();
#elif defined(QCAT_OPENGL)
		ImGui_ImplOpenGL3_NewFrame();
#endif  
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
	void ImGuiLayer::OnEnd()
	{
		QCAT_PROFILE_FUNCTION();

		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO();
#if defined(QCAT_DX11)
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#elif defined(QCAT_OPENGL)
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif  
		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
#if defined(QCAT_OPENGL)
			WindowsWindow* window = dynamic_cast<WindowsWindow*>(Application::GetInstance().GetWindow());
			QCatOpengl* pgfx = dynamic_cast<QCatOpengl*>(window->Gfx());
			pgfx->MakeCurrent();
#endif  	
		}
	}
	void ImGuiLayer::SetDarkThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f,0.105f,0.11f,1.0f };

		//Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f,0.205f,0.21f,1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f,0.305f,0.31f,1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f,0.1505f,0.151f,1.0f };

		//Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f,0.205f,0.21f,1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f,0.305f,0.31f,1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f,0.1505f,0.151f,1.0f };

		// Frame Bg
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f,0.205f,0.21f,1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f,0.305f,0.31f,1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f,0.1505f,0.151f,1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f,0.1505f,0.151f,1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f,0.3805f,0.381f,1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f,0.2805f,0.281f,1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f,0.1505f,0.151f,1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f,0.205f,0.21f,1.0f };

		// Title Bg
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f,0.1505f,0.151f,1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f,0.1505f,0.151f,1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f,0.1505f,0.151f,1.0f };

	}
	void ImGuiLayer::SetNatrueThemeColors()
	{
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f,0.205f,0.11f,1.0f };

		//Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f,0.405f,0.21f,1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f,0.405f,0.31f,1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f,0.2505f,0.151f,1.0f };

		//Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f,0.405f,0.21f,1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f,0.405f,0.31f,1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f,0.2505f,0.151f,1.0f };

		// Frame Bg
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f,0.405f,0.21f,1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f,0.405f,0.31f,1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f,0.2505f,0.151f,1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f,0.2505f,0.151f,1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f,0.4805f,0.381f,1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f,0.4805f,0.281f,1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f,0.2505f,0.151f,1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f,0.405f,0.21f,1.0f };

		// Title Bg
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f,0.2505f,0.151f,1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f,0.2505f,0.151f,1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f,0.2505f,0.151f,1.0f };

	}
	void ImGuiLayer::OnEvent(Event& e)
	{
		if (m_BlockEvent)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}
}