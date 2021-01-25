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
	struct ElementRef;
	enum class DXshaderType
	{
		VS = 0, PS, GS
	};
	class DXShader : public Shader
	{
	public:
		static std::string GetShaderName(const std::string& path);
		//static Ref<Shader> CreateShaderFromNative(const std::string& name, const std::string& src);
		//static Ref<Shader> CreateShaderFromFile(const std::string& path);
		static Ref<DX11VertexShader> CreateVertexShaderFromNative(const std::string& name, const std::string& src);
		static Ref<DX11PixelShader> CreatePixelShaderFromNative(const std::string& name, const std::string& src);
		static Ref<DX11VertexShader> CreateVertexShaderFromFile(const std::string& path);
		static Ref<DX11PixelShader> CreatePixelShaderFromFile(const std::string& path);
		static Microsoft::WRL::ComPtr<ID3DBlob> Compile(const DXshaderType& type, const  std::string& src);
		
	public:
		DXShader() = default;
		DXShader(const std::string& name, const std::string& vertexFile, const std::string& pixelFile);
		DXShader(const std::string& name, const std::string& vertexName, const std::string& vertexSrc, const std::string& pixelName, const std::string& pixelSrc,bool compile=false);
		~DXShader();
	public:
		virtual void Bind()const override;
		virtual void UnBind()const override;

		virtual void SetInt(const std::string& name, int value) override;
		virtual void SetIntArray(const std::string& name, int* values, unsigned int count) override;

		virtual void SetFloat(const std::string& name, const float& value) override;
		virtual void SetFloatArray(const std::string name, float* values, unsigned int count) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

		virtual void SetFloat3u(const std::string& uniformname, const std::string& valuename, const glm::vec3& value) override;
		virtual void SetFloat4u(const std::string& uniformname, const std::string& valuename, const glm::vec4& value) override;
		virtual void SetMat4u(const std::string& uniformname, const std::string& valuename, const glm::mat4& value) override;

		virtual const std::string& GetName()const override { return m_name; }
	public:
		//void UpdateConstantBuffer(const std::string& uniformname, const::std::string& name, const void* pdata);

		std::pair<Ref<DX11ConstantBuffer>,ElementRef> FindVariable(const std::string& name);
		template<typename T>
		void UpdateConstantBuffer(const std::string& uniformname, const T& pdata)
		{
			QCAT_PROFILE_FUNCTION();

			auto& vsConstantBuffers = pvs->GetConstantBuffers();
			for (auto& iter : vsConstantBuffers)
			{
				iter.second->UpdateElement(uniformname, pdata);
			}
			auto& psConstantBuffers = pps->GetConstantBuffers();
			for (auto& iter : psConstantBuffers)
			{
				iter.second->UpdateElement(uniformname, pdata);
			}
		}
		bool UpdateVertexConstantBuffer(const std::string& name,const void* data);
		bool UpdatePixelConstantBuffer(const std::string& name,const void* data);
		std::vector<char>& GetVerexData();
	protected:
		
	private:
		Ref<DX11VertexShader> pvs;
		Ref<DX11PixelShader> pps;
		std::string m_name;
	};
	class DX11Shader
	{
	public:
		void MakeBufferElement();
		void AddConstantBuffer(const std::string& name, Ref<DX11ConstantBuffer>& pConstantBuffer);
		Ref<DX11ConstantBuffer> GetConstantBuffer(const std::string name);
		std::unordered_map<std::string, Ref<DX11ConstantBuffer>>& GetConstantBuffers() { return m_ConstantBuffers; }
	public:
		virtual void Bind()const =0 ;
		virtual void UnBind()const =0 ;
	protected:
		Microsoft::WRL::ComPtr <ID3D11ShaderReflection> pReflector;
		std::unordered_map<std::string, Ref<DX11ConstantBuffer>> m_ConstantBuffers;
		QGfxDeviceDX11* gfx;
		std::string m_name;
		DXshaderType type;
	};
	class DX11VertexShader : public DX11Shader
	{
	public:
		DX11VertexShader(const std::string& name,const std::string& path);
		DX11VertexShader(const std::string& name, const Microsoft::WRL::ComPtr<ID3DBlob>& pBlob);
		~DX11VertexShader();
	public:
		Microsoft::WRL::ComPtr <ID3D11ShaderReflection>& GetReflector() { return pReflector; };
		std::vector<char>& DX11VertexShader::GetData();
	public:
	    void Bind()const ;
	    void UnBind()const ;
	   
	    const std::string& GetName()const { return m_name; }
	private:
		//VertexShader
		Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;
		std::vector<char> data;
	};
	class DX11PixelShader : public DX11Shader
	{
	public:
		DX11PixelShader(const std::string& name, const std::string& path);
		DX11PixelShader(const std::string& name, const Microsoft::WRL::ComPtr<ID3DBlob>& pBlob);
		~DX11PixelShader();
	public:
		 void Bind()const ;
		 void UnBind()const ;
		 Microsoft::WRL::ComPtr <ID3D11ShaderReflection>& GetReflector() { return pReflector; };
		const std::string& GetName()const  { return m_name; }
	private:
		//PixelShader
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	};
	
}