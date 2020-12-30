#pragma once
#include "QCat/Renderer/Texture.h"
#include "QGfxDeviceDX11.h"

namespace QCat
{
	class DX11Texture2D : public Texture2D
	{
	public:
		DX11Texture2D(unsigned int width,unsigned int height);
		DX11Texture2D(const std::string& path);
		virtual ~DX11Texture2D();

		virtual unsigned int GetWidth() const override { return m_width; }
		virtual unsigned int GetHeight() const override { return m_height; }

		virtual void SetData(void* data, unsigned int size) override;

		virtual void Bind(unsigned int slot = 0) const override;
	private:
		std::string m_path;
		unsigned int m_width, m_height;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		DXGI_FORMAT m_dataFormat;
	};
}
