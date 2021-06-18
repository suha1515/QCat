#pragma once
#include <filesystem>

namespace QCat
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel();
		
		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;
	};

}
