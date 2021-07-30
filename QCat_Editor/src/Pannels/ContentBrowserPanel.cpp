#include "ContentBrowserPanel.h"
#include <QCat/Renderer/RenderAPI.h>

namespace QCat
{
	static const std::filesystem::path s_AssetPath = "Asset";

	ContentBrowserPanel::ContentBrowserPanel()
		:m_CurrentDirectory(s_AssetPath)
	{
		Sampler_Desc desc;
		m_DirectoryIcon = TextureLibrary::Load("DirectoryIcon", "Resources/ContentBrowser/folder.png");
		m_FileIcon = TextureLibrary::Load("FileIcon", "Resources/ContentBrowser/files.png");
		m_ImageIcon = TextureLibrary::Load("ImageIocn", "Resources/ContentBrowser/image.png");
		
		
		uv1 = { 0,1 };
		uv2 = { 1,0 };
		if (RenderAPI::GetAPI() == RenderAPI::API::DirectX11)
		{
			uv1 = { 0,0 };
			uv2 = { 1,1 };
		}
		
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Content Browser");

		if (m_CurrentDirectory != std::filesystem::path(s_AssetPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}
		static float padding = 16.0f;
		static float thumbnailSize = 128.f;
		float cellSize = thumbnailSize + padding;
		// 272
		// 1000
		float pannelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = int(pannelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;
		ImGui::Columns(columnCount,0,false);
		
		//iterate directory
		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
		{
			const auto path = directoryEntry.path();
			auto relativePath = std::filesystem::relative(path, s_AssetPath);
			std::string filenameString = relativePath.filename().string();

			Ref<Texture>& icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;

			ImGui::ImageButton(icon->GetTexture(), { thumbnailSize,thumbnailSize },uv1,uv2);
			if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();

			}
			ImGui::TextWrapped(filenameString.c_str());
	
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
		ImGui::SliderFloat("Thumbnail Size",& thumbnailSize,16,512);
		ImGui::SliderFloat("Padding ", &padding, 0, 32);
		ImGui::End();
	}
}