#pragma once
#include "QCat/Renderer/Buffer.h"
#include "QGfxDeviceDX11.h"

namespace QCat
{
	static ShaderDataType DXShaderDataConvert(const D3D11_SHADER_TYPE_DESC& info)
	{

		if (info.Class == D3D_SVC_SCALAR)
		{
			if (info.Type == D3D_SVT_FLOAT)
				return ShaderDataType::Float;
			else if (info.Type == D3D_SVT_INT)
				return ShaderDataType::Int;
			else if (info.Type == D3D_SVT_BOOL)
			{
				return ShaderDataType::Bool;
			}
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
	}
	class QCAT_API DX11VertexBuffer : public VertexBuffer
	{
	public:
		DX11VertexBuffer(float* vertices, unsigned int size,unsigned int stride=0);
		~DX11VertexBuffer(){}

		virtual void Bind() const override ;
		virtual void UnBind() const override ;

		virtual const Ref<BufferLayout>& GetLayout() const override { return m_pbufferLayout; };
		virtual void SetLayout(BufferLayout* layout) override { m_pbufferLayout.reset(layout); };
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
		bool CrossBoundary(unsigned int offset, unsigned int size) noexcept
		{
			const auto end = offset + size;
			const auto pageStart = offset / 16u;
			const auto pageEnd = end / 16u;
			return (pageStart != pageEnd && end % 16u != 0) || size > 16u;
		}
		void CalculatePadding()
		{
			unsigned int nextoffset = 0;
			for (auto& element : bufferElements)
			{
				if (CrossBoundary(nextoffset, element.size))
				{
					nextoffset = nextoffset + (16u - nextoffset % 16u) % 16u;;
				}
				element.offset = nextoffset;
				nextoffset += element.size;
			}
			nextoffset = CalculateSize(nextoffset);
			data.resize(nextoffset);
		}
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
		DX11ConstantBuffer(QGfxDeviceDX11& gfx, unsigned int slot ,const void* data,unsigned int size)
			:slot(slot)
		{
			this->pgfx = &gfx;
		
			D3D11_BUFFER_DESC bd;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0u;
			bd.MiscFlags = 0u;
			bd.ByteWidth = CalculateSize(size);
			bd.StructureByteStride = 0u;

			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem = data;
			HRESULT result = gfx.GetDevice()->CreateBuffer(&bd, &sd, &pConstantBuffer);
			int a = 0;
		}
		DX11ConstantBuffer(QGfxDeviceDX11& gfx, unsigned int slot, unsigned int size)
			:slot(slot)
		{
			this->pgfx = &gfx;

			D3D11_BUFFER_DESC bd;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0u;
			bd.MiscFlags = 0u;
			bd.ByteWidth = CalculateSize(size);
			bd.StructureByteStride = 0u;

			pgfx->GetDevice()->CreateBuffer(&bd, nullptr, &pConstantBuffer);
		}
		DX11ConstantBuffer(std::vector<BufferElement>& bufferElement, unsigned int slot)
			:slot(slot), bufferElements(bufferElement)
		{
			this->pgfx = QGfxDeviceDX11::GetInstance();
			CalculatePadding();
			D3D11_BUFFER_DESC bd;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.CPUAccessFlags = 0u;
			bd.MiscFlags = 0u;
			bd.ByteWidth = data.size();
			bd.StructureByteStride = 0u;

			pgfx->GetDevice()->CreateBuffer(&bd, nullptr, &pConstantBuffer);
		}
		void UpdateData(QGfxDeviceDX11& gfx,const void* pdata)
		{
			gfx.GetContext()->UpdateSubresource(pConstantBuffer.Get(), 0, 0, pdata, 0u, 0u);
			memcpy(data.data(), pdata, data.size());
		}
		void UpdateElement(const std::string& name, const void* pdata)
		{
			for (auto& element : bufferElements)
			{
				if (element.name == name)
				{
					// TODO : Check the type we have to make own dataType for type check
					ShaderDataType type = element.type;
					unsigned int size = element.size;
					memcpy(data.data(), pdata, size);
					pgfx->GetContext()->UpdateSubresource(pConstantBuffer.Get(), 0, 0, data.data(), 0u, 0u);
					return;
				}
			}
		}
		~DX11ConstantBuffer() 
		{
		}
		virtual void Bind()
		{
			/*switch (type) 
			{
				case Type::None: QCAT_CORE_ASSERT(false,"ConstantBuffer type error : type is none")break;
				case Type::Pixel: gfx->GetContext()->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf()); break;
				case Type::Vertex: gfx->GetContext()->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf()); break;
			}*/
		}
	protected:
		QGfxDeviceDX11* pgfx;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
		std::vector<BufferElement> bufferElements;
		std::vector<char> data;
		unsigned int slot;
		std::string name;
	};
	class QCAT_API VertexConstantBuffer : public DX11ConstantBuffer
	{
	public:
		using DX11ConstantBuffer::DX11ConstantBuffer;
		virtual void Bind() override
		{
			pgfx->GetContext()->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		}
	};
	class QCAT_API PixelConstantBuffer : public DX11ConstantBuffer
	{
	public:
		using DX11ConstantBuffer::DX11ConstantBuffer;
		virtual void Bind() override
		{
			pgfx->GetContext()->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf());
		}
	};
}