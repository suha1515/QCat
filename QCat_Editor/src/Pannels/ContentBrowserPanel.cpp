#include "ContentBrowserPanel.h"
#include <QCat/Renderer/RenderAPI.h>

namespace QCat
{
	extern const std::filesystem::path g_AssetPath = "Asset";

	ContentBrowserPanel::ContentBrowserPanel()
		:m_CurrentDirectory(g_AssetPath)
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

		if (m_CurrentDirectory != std::filesystem::path(g_AssetPath))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}
		static float padding = 16.0f;
		static float thumbnailSize =64.f;
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
			auto relativePath = std::filesystem::relative(path, g_AssetPath);
			std::string filenameString = relativePath.filename().string();

			ImGui::PushID(filenameString.c_str());
			Ref<Texture> icon;
			if (directoryEntry.is_directory())
				icon = m_DirectoryIcon;
			else
			{
				if (path.extension() == ".jpg" || path.extension() == ".png" || path.extension() == ".tga" || path.extension() == ".jpeg"
					|| path.extension() == ".hdr" || path.extension() == ".ibl")
				{
					icon = m_ImageIcon;
				}
				else
					icon = m_FileIcon;
			}
		
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::ImageButton(icon->GetTexture(), { thumbnailSize,thumbnailSize },uv1,uv2);

			if (ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BRWOSER_ITEM", itemPath, (wcslen(itemPath)+1) * sizeof(wchar_t),ImGuiCond_Once);
				ImGui::Image(icon->GetTexture(), { 64.f,64.f }, uv1, uv2);
				ImGui::Text(filenameString.c_str());
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();
			if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				if (directoryEntry.is_directory())
					m_CurrentDirectory /= path.filename();

			}
			ImGui::TextWrapped(filenameString.c_str());
	
			ImGui::NextColumn();
			ImGui::PopID();
		}
		ImGui::Columns(1);
		//ImGui::SliderFloat("Thumbnail Size",& thumbnailSize,16,512);
		//ImGui::SliderFloat("Padding ", &padding, 0, 32);
		ImGui::End();
	}
}