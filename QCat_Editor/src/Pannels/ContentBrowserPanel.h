#pragma once
#include <QCat.h>
#include <filesystem>
#include <QCat/Renderer/Texture.h>
#include <Imgui/imgui.h>
namespace QCat
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		
		void ReloadImage();
		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;
		Ref<Texture> m_DirectoryIcon;
		Ref<Texture> m_FileIcon;
		Ref<Texture> m_ImageIcon;
		ImVec2 uv1;
		ImVec2 uv2;
	};

}
