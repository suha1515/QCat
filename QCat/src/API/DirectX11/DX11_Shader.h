#pragma once
#include<Renderer/Shader.h>
#include <API/DirectX11/QGfxDeviceDX11.h>
#include <string>
#include <wrl.h>

#include "API/DirectX11/DX11_Buffer.h"
namespace QCat
{
	class DX11VertexShader;
	class DX11PixelShader;
	class DX11Shader : public Shader
	{
	public:
		DX11Shader(const std::string& vertexShaderPath, const std::string& pixelShaderPath);
		~DX11Shader();
	public:
		virtual void Bind()const override;
		virtual void UnBind()const override;
	public:
		void AddVertexConstantBuffer(const std::string& name, Ref<VertexConstantBuffer>& pvertexConstantBuffer);
		void AddPixelConstantBuffer(const std::string& name, Ref<PixelConstantBuffer>& ppixelConstantBuffer);
		void UpdateVertexConstantBuffer(const std::string& name,const void* data);
		void UpdatePixelConstantBuffer(const std::string& name,const void* data);
		std::vector<char>& GetVerexData();
	private:
		DX11VertexShader* pvs;
		DX11PixelShader* pps;
		std::unordered_map<std::string,Ref<VertexConstantBuffer>> m_vertexConstantBuffers;
		std::unordered_map<std::string, Ref<PixelConstantBuffer>> m_pixelConstantBuffers;
	};
	class DX11VertexShader
	{
	public:
		DX11VertexShader(Graphics& gfx,const std::string& path);
		~DX11VertexShader();
	public:
		std::vector<char>& DX11VertexShader::GetData();
	public:
		 void Bind() ;
		 void UnBind();
	private:
		//VertexShader
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
		QGfxDeviceDX11* gfx;
		std::vector<char> data;
	};
	class DX11PixelShader
	{
	public:
		DX11PixelShader(Graphics& gfx, const std::string& path);
		~DX11PixelShader();
	public:
		 void Bind() ;
		 void UnBind();
	private:
		//PixelShader
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
		QGfxDeviceDX11* gfx;
	};

}