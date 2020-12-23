#pragma once
#include <QCat/Renderer/Shader.h>
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
		enum class shaderType
		{
			VS=0,PS,GS
		};
		static std::string GetShaderName(const std::string& path);
		static Ref<Shader> CreateShaderFromNative(const std::string& name, const std::string& src);
		static Ref<Shader> CreateShaderFromFile(const std::string& path);
		static Ref<DX11VertexShader> CreateVertexShaderFromNative(const std::string& name, const std::string& src);
		static Ref<DX11PixelShader> CreatePixelShaderFromNative(const std::string& name, const std::string& src);
		static Ref<DX11VertexShader> CreateVertexShaderFromFile(const std::string& path);
		static Ref<DX11PixelShader> CreatePixelShaderFromFile(const std::string& path);
		static Microsoft::WRL::ComPtr<ID3DBlob> Compile(const shaderType& type, const  std::string& src);
		
	public:
		DX11Shader(const std::string& name ,const Ref<Shader>& vertexShader,const Ref<Shader>& pixelShader);
		DX11Shader(const std::string& name, const std::string& vertexName, const std::string& vertexSrc, const std::string& pixelName, const std::string& pixelSrc,bool compile=false);
		~DX11Shader();
	public:
		virtual void Bind()const override;
		virtual void UnBind()const override;
		virtual const std::string& GetName()const override { return m_name; }
	public:
		void AddVertexConstantBuffer(const std::string& name, Ref<VertexConstantBuffer>& pvertexConstantBuffer);
		void AddPixelConstantBuffer(const std::string& name, Ref<PixelConstantBuffer>& ppixelConstantBuffer);
		void UpdateVertexConstantBuffer(const std::string& name,const void* data);
		void UpdatePixelConstantBuffer(const std::string& name,const void* data);
		std::vector<char>& GetVerexData();
	private:
		Ref<DX11VertexShader> pvs;
		Ref<DX11PixelShader> pps;
		std::unordered_map<std::string,Ref<VertexConstantBuffer>> m_vertexConstantBuffers;
		std::unordered_map<std::string, Ref<PixelConstantBuffer>> m_pixelConstantBuffers;
		std::string m_name;
	};
	class DX11VertexShader : public Shader
	{
	public:
		DX11VertexShader(const std::string& name,const std::string& path);
		DX11VertexShader(const std::string& name, const Microsoft::WRL::ComPtr<ID3DBlob>& pBlob);
		~DX11VertexShader();
	public:
		std::vector<char>& DX11VertexShader::GetData();
	public:
		virtual void Bind()const override;
		virtual void UnBind()const override;
		virtual const std::string& GetName()const override { return m_name; }
	private:
		//VertexShader
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
		QGfxDeviceDX11* gfx;
		std::vector<char> data;
		std::string m_name;
	};
	class DX11PixelShader : public Shader
	{
	public:
		DX11PixelShader(const std::string& name, const std::string& path);
		DX11PixelShader(const std::string& name, const Microsoft::WRL::ComPtr<ID3DBlob>& pBlob);
		~DX11PixelShader();
	public:
		virtual void Bind()const override;
		virtual void UnBind()const override;
		virtual const std::string& GetName()const override { return m_name; }
	private:
		//PixelShader
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
		QGfxDeviceDX11* gfx;
		std::string m_name;
	};
	
}