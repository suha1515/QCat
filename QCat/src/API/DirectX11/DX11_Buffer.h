#pragma once
#include "QCat/Renderer/Buffer.h"
#include "DX11_BufferLayout.h"
#include "QGfxDeviceDX11.h"

namespace QCat
{
	static ShaderDataType DXShaderDataConvert(const D3D11_SHADER_TYPE_DESC& info)
	{
		if (info.Class == D3D_SVC_SCALAR)
		{
			if (info.Type == D3D_SVT_FLOAT)
				return ShaderDataType::Float;
			else if (info.Type == D3D_SVT_INT )
				return ShaderDataType::Int;
			else if (info.Type == D3D_SVT_BOOL)
				return ShaderDataType::Bool;
			
		}
		else if (info.Class == D3D_SVC_VECTOR)
		{
			if (info.Type == D3D_SVT_FLOAT)
			{
				if (info.Columns == 2)
					return ShaderDataType::Float2;
				else if (info.Columns == 3)
					return ShaderDataType::Float3;
				else if (info.Columns == 4)
					return ShaderDataType::Float4;
				
			}
			else if (info.Type == D3D_SVT_INT)
			{
				if (info.Columns == 2)
					return ShaderDataType::Int2;
				else if (info.Columns == 3)
					return ShaderDataType::Int3;
				else if (info.Columns == 4)
					return ShaderDataType::Int4;
			}
		}
		else if (info.Class == D3D_SVC_MATRIX_COLUMNS)
		{
			if (info.Type == D3D_SVT_FLOAT)
			{
				if (info.Columns == 4)
					return ShaderDataType::Mat4;
				else if(info.Columns==3)
					return ShaderDataType::Mat3;
			}
		}
		else if (info.Class == D3D_SVC_STRUCT)
		{
			return ShaderDataType::Struct;
		}
		
	}
	
	
	class QCAT_API DX11VertexBuffer : public VertexBuffer
	{
	public:
		DX11VertexBuffer(unsigned int size);
		DX11VertexBuffer(float* vertices, unsigned int size,unsigned int stride=0);
		~DX11VertexBuffer(){}

		virtual void Bind() const override ;
		virtual void UnBind() const override ;
		virtual void SetData(void* data, unsigned int size) override;

		virtual const Ref<BufferLayout>& GetLayout() const override { return m_pbufferLayout; };
		virtual void SetLayout(Ref<BufferLayout> layout) override { m_pbufferLayout = layout; };
	private:
		std::string m_tag;
		UINT m_stride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
		Ref<BufferLayout> m_pbufferLayout;
	};
	class QCAT_API DX11IndexBuffer : public IndexBuffer
	{
	public:
		DX11IndexBuffer(unsigned int* indices ,unsigned int size);
		~DX11IndexBuffer() {}

		virtual void Bind() const override;
		virtual void UnBind() const override;
		virtual unsigned int GetCount() const override { return m_count; }
	private:
		std::string m_tag;
		UINT m_count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
	};
	class QCAT_API DX11ConstantBuffer
	{
	public:
		unsigned int CalculateSize(unsigned int size)
		{
			unsigned int realSize = 0;
			if (size % 16 == 0)
				realSize = size;
			else
			{
				realSize = size + (16u - size % 16u) % 16u;;
			}
			return realSize;
		}
		DX11ConstantBuffer(QGfxDeviceDX11& gfx, unsigned int slot, const void* data, unsigned int size, bool dynamic = false);
		DX11ConstantBuffer(QGfxDeviceDX11& gfx, unsigned int slot, unsigned int size, bool dynamic = false);
		DX11ConstantBuffer(ElementLayout& layout, unsigned int slot,bool dynamic=false);
		
		ElementRef FindVariable(const std::string& name)
		{
			ElementRef ref;
			ref = AccessElement(name);
			return ref;
		}
		void UpdateData();
		void FinalizeUpdate()
		{
			QCAT_PROFILE_FUNCTION();

			if (IsDataChanged)
			{
				UpdateData();
				IsDataChanged = false;
			}
		};
		void UpdateElements(const void* pData)
		{
			memcpy(bufferElements.GetData(), pData, bufferElements.GetSizeInBytes());
		}
		template<typename T>
		void UpdateElement(const std::string& name, const T& pdata )
		{
			ElementRef ref;
			ref = AccessElement(name);
			if (ref.GetDataType() == ShaderDataType::None)
				return;
			ref = pdata;

			IsDataChanged = true;
			//bufferElements[name] = pdata;
		}
		ElementRef AccessElement(const std::string& name);
		~DX11ConstantBuffer() 
		{
		}
		virtual void Bind()
		{			
		}
	public:
		bool IsDataChanged = false;
	protected:
		QGfxDeviceDX11* pgfx;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		Buffer bufferElements;
		unsigned int slot;
		std::string name;
		bool IsDynamic = false;
	};
	class QCAT_API VertexConstantBuffer : public DX11ConstantBuffer
	{
	public:
		using DX11ConstantBuffer::DX11ConstantBuffer;
		virtual void Bind() override
		{
			QCAT_PROFILE_FUNCTION();
			pgfx->GetContext()->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		}
	};
	class QCAT_API PixelConstantBuffer : public DX11ConstantBuffer
	{
	public:
		using DX11ConstantBuffer::DX11ConstantBuffer;
		virtual void Bind() override
		{
			QCAT_PROFILE_FUNCTION();
			if (IsDataChanged)
			{
				UpdateData();
				IsDataChanged = false;
			}
			pgfx->GetContext()->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		}
	};
	class QCAT_API GeometryConstantBuffer : public DX11ConstantBuffer
	{
	public:
		using DX11ConstantBuffer::DX11ConstantBuffer;
		virtual void Bind() override
		{
			QCAT_PROFILE_FUNCTION();
			if (IsDataChanged)
			{
				UpdateData();
				IsDataChanged = false;
			}
			pgfx->GetContext()->GSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		}
	};

	class QCAT_API DX11StructureBuffer : public ShaderBuffer
	{
	public:
		DX11StructureBuffer(uint32_t size, uint32_t count, void* pData=nullptr);
		virtual ~DX11StructureBuffer() {};

		virtual void Bind(uint32_t slot, ShaderType type = ShaderType::NONE) const override;
		virtual void ReadData(std::vector<char>& data) override;

	private:
		D3D11_BUFFER_DESC desc;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pBufferView;

	};
	class QCAT_API DX11RWStructureBuffer : public ShaderBuffer
	{
	public:
		DX11RWStructureBuffer(uint32_t size, uint32_t count, void* pData = nullptr);
		virtual ~DX11RWStructureBuffer() {};

		virtual void Bind(uint32_t slot, ShaderType type = ShaderType::NONE) const override;
		virtual void ReadData(std::vector<char>& data) override;
	private:
		D3D11_BUFFER_DESC desc;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pBufferView;

	};
}