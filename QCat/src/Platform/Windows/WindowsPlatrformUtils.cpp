#include "qcpch.h"
#include "QCat/Uitiliy/PlatformUtils.h"
#include "QCat/Core/Application.h"

#include <commdlg.h>
#include "WindowsWindow.h"


namespace QCat
{
	std::string FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;			
		char szFile[260] = { 0 };	
		
		ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = reinterpret_cast<WindowsWindow*>(Application::GetInstance().GetWindow()->GetNativeWindow())->GetHandle();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::string();
	}
	std::string FileDialogs::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		char szFile[260] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = reinterpret_cast<WindowsWindow*>(Application::GetInstance().GetWindow()->GetNativeWindow())->GetHandle();
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			return ofn.lpstrFile;
		}
		return std::string();
	}
}