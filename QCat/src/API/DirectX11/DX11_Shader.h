#pragma once
#include<Renderer/Shader.h>
#include <API/DirectX11/QGfxDeviceDX11.h>
#include <string>
#include <wrl.h>

namespace QCat
{
	class DX11VertexShader : public Shader
	{
	public:
		DX11VertexShader(Graphics& gfx,const std::string& path);
		~DX11VertexShader();

		virtual void Bind() override;
		virtual void UnBind() override;
		virtual std::vector<char>& GetData() override;
	private:
		//VertexShader
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
		QGfxDeviceDX11* gfx;
	};
	class DX11PixelShader : public Shader
	{
	public:
		DX11PixelShader(Graphics& gfx, const std::string& path);
		~DX11PixelShader();

		virtual void Bind() override;
		virtual void UnBind() override;
		virtual std::vector<char>& GetData() override;
	private:
		//PixelShader
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
		QGfxDeviceDX11* gfx;
	};
}