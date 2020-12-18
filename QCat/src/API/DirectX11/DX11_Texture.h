#pragma once
#include "Renderer/Texture.h"
#include "QGfxDeviceDX11.h"

namespace QCat
{
	class DX11Texture2D : public Texture2D
	{
	public:
		DX11Texture2D(const std::string& path);
		virtual ~DX11Texture2D();

		virtual unsigned int GetWidth() const { return m_width; }
		virtual unsigned int GetHeight() const { return m_height; }

		virtual void Bind(unsigned int slot = 0) const;
	private:
		std::string m_path;
		unsigned int m_width, m_height;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	};
}
