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
	class DX11GeometryShader;
	struct ElementRef;
	enum class DXshaderType
	{
		None =0,VS, PS, GS
	};
	class DX11Shader;
	class DXShader : public Shader
	{
	private:
		struct ErrorInfo
		{
			std::string profile;
			std::string meesage;
		};
		struct Data
		{
			Ref<DX11VertexShader> pvs;
			Ref<DX11PixelShader> pps;
			Ref<DX11GeometryShader> pgs;
		};
		static const DXShader* s_CurrentlyBound;
	public:
		static const DXShader* CurrentlyBound() { return s_CurrentlyBound; }
		static std::string GetShaderName(const std::string& path);
		static Ref<DX11Shader> CreateShaderFromNative(const std::string& name, const std::string& src, DXshaderType type);
		static Ref<DX11Shader> CreateShaderFromFile(const std::string& path, DXshaderType type);
		static Microsoft::WRL::ComPtr<ID3DBlob> Compile(const DXshaderType& type, const  std::string& src);
	private:
		std::unordered_map<DXshaderType, std::string> PreProcess(const std::string& source);
		static Microsoft::WRL::ComPtr<ID3DBlob> Compile(const std::string& src, const std::string& profile, const std::string& main, ErrorInfo& info);

		
	public:
		DXShader() = default;
		DXShader(const std::string& shaderPath);
		DXShader(const std::string& name, const std::string& vertexFile, const std::string& pixelFile,const std::string& geoFile = "");
		~DXShader();
	public:
		virtual void Bind()const override;
		virtual void UpdateBuffer() const override;
		virtual void UnBind()const override;

		virtual void SetInt(const std::string& name, int value, ShaderType type) override;
		virtual void SetIntArray(const std::string& name, int* values, unsigned int count, ShaderType type) override;

		virtual void SetFloat(const std::string& name, const float& value, ShaderType type) override;
		virtual void SetFloatArray(const std::string name, float* values, unsigned int count, ShaderType type) override;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value, ShaderType type) override;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value, ShaderType type) override;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value, ShaderType type) override;
		virtual void SetMat4(const std::string& name, const glm::mat4& value, ShaderType type) override;

		virtual void SetBool(const std::string& name, const bool& value, ShaderType type) override;

		virtual void SetFloat3u(const std::string& uniformname, const std::string& valuename, const glm::vec3& value) override;
		virtual void SetFloat4u(const std::string& uniformname, const std::string& valuename, const glm::vec4& value) override;
		virtual void SetMat4u(const std::string& uniformname, const std::string& valuename, const glm::mat4& value) override;

		virtual const std::string& GetName()const override { return m_name; }
	public:
		std::pair<Ref<DX11ConstantBuffer>,ElementRef> FindVariable(const std::string& name, ShaderType type);
		bool UpdateVertexConstantBuffer(const std::string& name,const void* data);
		bool UpdatePixelConstantBuffer(const std::string& name,const void* data);


		inline Data& GetData() const { return m_Data; }
	private:
	private:
		std::string m_name;
		mutable Data m_Data;
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
		virtual void UpdateBuffer() const=0;
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
		std::vector<char>& GetData();
	public:
	    virtual void Bind()const ;
		virtual void UpdateBuffer() const;
	    virtual void UnBind()const ;
	   
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
		 virtual void UpdateBuffer() const;
		 void UnBind()const ;
		 Microsoft::WRL::ComPtr <ID3D11ShaderReflection>& GetReflector() { return pReflector; };
		const std::string& GetName()const  { return m_name; }
	private:
		//PixelShader
		Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	};
	class DX11GeometryShader : public DX11Shader
	{
	public:
		DX11GeometryShader(const std::string& name, const std::string& path);
		DX11GeometryShader(const std::string& name, const Microsoft::WRL::ComPtr<ID3DBlob>& pBlob);
		~DX11GeometryShader();
	public:
		Microsoft::WRL::ComPtr <ID3D11ShaderReflection>& GetReflector() { return pReflector; };
	public:
		virtual void Bind()const;
		virtual void UpdateBuffer() const;
		virtual void UnBind()const;

		const std::string& GetName()const { return m_name; }
	private:
		//VertexShader
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGeometryShader;
		std::vector<char> data;
	};
	
}